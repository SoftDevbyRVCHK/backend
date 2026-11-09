#ifndef ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H
#define ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <span>

namespace allocator {

    struct free_list_resource {

        struct node;
        
        struct chunk_handle {

            struct info {
            
                info () noexcept : size(0), flags(0) {}
                
                void load  (void *chunk_begin) {std::memcpy(this, chunk_begin, sizeof(info));}
                void store (void *chunk_begin) {std::memcpy(chunk_begin, this, sizeof(info));}
                
                void load  (const chunk_handle& handle) {load(handle.chunk_begin);}
                void store (const chunk_handle& handle) {store(handle.chunk_begin);}

                bool current_is_use () noexcept {return flags & flags::cuse;}
                void current_reset  () noexcept {flags&= (~flags::cuse);}
                void current_set    () noexcept {flags|= flags::cuse;}
                
                bool previous_is_use () noexcept {return flags & flags::puse;}
                void previous_reset  () noexcept {flags&= (~flags::puse);}
                void previous_set    () noexcept {flags|= flags::puse;}
                
                std::size_t size;
            
            private:
                struct flags {
                    
                    static constexpr std::uint8_t nuse {0};
                    static constexpr std::uint8_t cuse {1 << 0};
                    static constexpr std::uint8_t puse {1 << 1};
                };

                std::uint8_t flags;
            };

            chunk_handle (std::byte* chunk_begin, std::size_t chunk_size) :
                chunk_begin(chunk_begin),
                chunk_size(chunk_size)
            {}
            
            std::byte* prepare (std::size_t alignb, std::size_t sizeb) const {

                std::byte* cchunk_begin {this->chunk_begin};
                std::size_t cchunk_size {this->chunk_size};

                if (cchunk_size < sizeof(info) + sizeof(std::ptrdiff_t) + sizeb) 
                    return nullptr;

                std::byte* ptr {cchunk_begin + sizeof(info) + sizeof(std::ptrdiff_t)};
                cchunk_size-= sizeof(info) + sizeof(std::ptrdiff_t);
                
                if (ptr > next_chunk_begin())
                    return nullptr;
                
                void* aligned = ptr;

                if (!std::align(
                        alignb,
                        sizeb,
                        aligned,
                        cchunk_size
                    )
                ) {
                    return nullptr;
                }

                ptr = static_cast<std::byte*>(aligned);
                
                std::ptrdiff_t distance = ptr - cchunk_begin;
                if (distance > std::numeric_limits<std::ptrdiff_t>::max())
                    return nullptr;
                
                const std::ptrdiff_t offset_value = static_cast<std::ptrdiff_t>(distance);

                std::memcpy(
                    ptr - sizeof(std::ptrdiff_t),
                    &offset_value,
                    sizeof(offset_value)
                );
                
                return ptr;
            }

            node* get_node () const {
                node* prevnode;
                std::memcpy(
                    &prevnode,
                    chunk_begin + chunk_size - sizeof(node*),
                    sizeof(node*)
                );
                return prevnode;
            }
            node* get_prev_node () const {
                node* prevnode;
                std::memcpy(
                    &prevnode,
                    chunk_begin - sizeof(node*),
                    sizeof(node*)
                );
                return prevnode;
            }
            void set_node (node* newnode) const {
                std::memcpy(
                    chunk_begin + chunk_size - sizeof(node*),
                    &newnode,
                    sizeof(node*)
                );
            }
            
            std::byte* next_chunk_begin () const {return chunk_begin + chunk_size;}

            static inline chunk_handle from_ptr (void* ptr) {
                
                std::ptrdiff_t offset;
                std::memcpy(
                    &offset,
                    static_cast<std::byte*>(ptr) - sizeof(std::ptrdiff_t),
                    sizeof(std::ptrdiff_t)
                );

                return from_chunk_begin(
                    static_cast<std::byte*>(ptr) - offset
                );
            }
            
            static inline chunk_handle from_chunk_begin (std::byte* begin) {

                info chunk_info;
                chunk_info.load(begin);
                return chunk_handle {
                    begin,
                    chunk_info.size
                };
            }

            std::byte* chunk_begin;
            std::size_t chunk_size;
        };

        struct node {

            void include_after (node *prev) {
                
                prev->next->prev = this;
                this->prev = prev;
                
                this->next = prev->next;
                prev->next = this;
            }
            void include_before (node *next) {
                
                next->prev->next = this;
                this->next = next;
                
                this->prev = next->prev;
                next->prev = this;
            }

            static inline void exclude (node *n) {
                if (is_loop(n))
                    n->next = n->prev = nullptr;
                else {
                    n->prev->next = n->next;
                    n->next->prev = n->prev;
                }
            }

            static inline void replace (node *node_old, node *node_new) {
                if (is_loop(node_old)) 
                    loop(node_new);
                else {

                    node_old->prev->next = node_new;
                    node_old->next->prev = node_new;
                    
                    *node_new = *node_old;
                }
            }

            static inline bool is_loop (node *n) {
                return n->next == n;
            }
            static inline void loop (node *n) {
                n->prev = n;
                n->next = n;
            }
            
            static inline node* from_use_handle (const chunk_handle& handle) {
                node* nd { 
                    reinterpret_cast<node*>(
                        handle.prepare(alignof(node), sizeof(node) + sizeof(node*))
                    )
                };
                
                if (nd == nullptr) return nullptr;
                
                typename chunk_handle::info info;
                info.size = handle.chunk_size;
                info.current_reset();
                info.previous_set();
                info.store(handle);
                
                handle.set_node(nd);
                
                return nd;
            }

            node *prev;
            node *next;
        };
        
        free_list_resource (std::span<std::byte> storage_chunk) : 
            storage(storage_chunk),
            head (
                node::from_use_handle(
                    chunk_handle(
                        storage_chunk.data(),
                        storage_chunk.size()
                    )
                )
            )
        {
            if (head != nullptr) 
                node::loop(head);
        }
        
        ~free_list_resource () {
            storage = std::span<std::byte>(); 
            head = nullptr;
        }
                                                        
        free_list_resource (const free_list_resource &) = delete;
        free_list_resource& operator= (const free_list_resource &) = delete;
        
        free_list_resource (free_list_resource &&) = delete;
        free_list_resource& operator= (free_list_resource &&) = delete;

        template <typename T>
        T* allocate () { 
            return reinterpret_cast<T*>(
                allocate(1 * sizeof(T), alignof(T))
            );
        }
        template <typename T>
        T* allocate (std::size_t n) { 
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
                return nullptr;
                
            return reinterpret_cast<T*>(
                allocate(n * sizeof(T), alignof(T))
            ); 
        }
        void* allocate (std::size_t sizeb, std::size_t alignb) {
            if (head == nullptr)
                return nullptr;
                
            std::byte* ptr;

            node* cur {head}, *new_cur, copy;

            do {
                copy = *cur;
                
                ptr = allocate_imp(
                    alignb,
                    sizeb,
                    chunk_handle::from_ptr(cur),
                    new_cur
                );
                
                if (ptr != nullptr) {
                    if (new_cur != nullptr) {
                        
                        if (copy.next == cur)
                            node::loop(new_cur);
                        else
                            node::replace(&copy, new_cur);
                        
                        head = new_cur;
                    }
                    else
                        if (copy.next == cur) {
                            head = nullptr;
                        }
                        else {
                            node::exclude(&copy);
                            head = copy.next;
                        }

                    return ptr;
                }
            }
            while ((cur = cur->next) != head);
            
            return nullptr;
        }

        void deallocate (void* ptr) {
            if (ptr == nullptr)
                return; 
            
            node *cur;
            typename chunk_handle::info info;

            chunk_handle cur_handle {chunk_handle::from_ptr(ptr)};
            info.load(cur_handle);

            if (cur_handle.chunk_begin > storage.data() && !info.previous_is_use()) {
                
                cur = cur_handle.get_prev_node();

                cur_handle = chunk_handle::from_ptr(cur);
                cur_handle.chunk_size+= info.size;

                info.load(cur_handle);
                    info.size = cur_handle.chunk_size;
                    info.current_reset();
                info.store(cur_handle);
                
                cur_handle.set_node(cur);
            }
            else {                                                
                cur = node::from_use_handle(cur_handle);
                cur->prev = nullptr;
            }
            
            chunk_handle next_handle {
                cur_handle.next_chunk_begin(),
                0
            };
            if (next_handle.chunk_begin < storage.data() + storage.size()) {

                next_handle = chunk_handle::from_chunk_begin(next_handle.chunk_begin);
                info.load(next_handle);
                
                if (info.current_is_use()) {
                    info.previous_reset();
                    info.store(next_handle);
                }
                else { 
                    node *next {next_handle.get_node()};

                    if (cur->prev == nullptr) 
                        node::replace(next, cur);
                    else
                        node::exclude(next);
                        
                    head = cur;
                    
                    cur_handle.chunk_size+= info.size;
                        info.load(cur_handle);
                        info.size = cur_handle.chunk_size;
                    info.store(cur_handle);
                    
                    cur_handle.set_node(cur);
                }                        
            }
            if (cur->prev == nullptr) { 
                
                if (head == nullptr)
                    node::loop(cur);
                else 
                    cur->include_before(head);
                    
                head = cur;
            }
        }
        
    protected:

        std::span<std::byte> storage;
        node* head;
        
        std::byte* allocate_imp (
            std::size_t alignb,
            std::size_t sizeb,
            chunk_handle handle,
            node*& nd
        )
        {
            std::byte* ptr;
            
            if (sizeb < sizeof(node) + sizeof(node*)) 
                sizeb = sizeof(node) + sizeof(node*);
            if (alignb < alignof(node))
                alignb = alignof(node);
                
            ptr = handle.prepare(alignb, sizeb);

            if (ptr != nullptr) {
                
                chunk_handle node_handle {
                    ptr + sizeb,
                    handle.chunk_size - (ptr + sizeb - handle.chunk_begin)
                };
                typename chunk_handle::info info;
                
                nd = node::from_use_handle(node_handle);

                if (nd == nullptr) {
                    
                    std::byte* nextchunk_begin = handle.next_chunk_begin();
                    
                    if (nextchunk_begin < storage.data() + storage.size()) {
                        info.load(nextchunk_begin);
                            info.previous_set();
                        info.store(nextchunk_begin);
                    }
                }
                else 
                    handle.chunk_size-= node_handle.chunk_size;
                
                info.load(handle);
                    info.size = handle.chunk_size;
                    info.current_set();
                info.store(handle);
            }
            else 
                nd = nullptr;
            
            return ptr;
        }
    };
}

#endif /* ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H */

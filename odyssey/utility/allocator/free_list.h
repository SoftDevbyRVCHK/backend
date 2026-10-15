#ifndef ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H
#define ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <cstring>
#include <span>
#include <new>
#include <memory>
#include <atomic>
#include <mutex>
#include <utility>

namespace odyssey {

    namespace utility {
        
        namespace allocator {

            template <
                typename size_mt  = std::size_t, 
                typename flags_t  = std::uint8_t,
                typename offset_t = std::uint16_t
            >
            struct free_list {
            
                struct header_t {
                    
                    header_t () : _size(0), _flags(0) {}
                    
                    void init (void *chunk_start) {
                        std::memcpy(this, chunk_start, sizeof(header_t));
                    }
                    void copy (void *chunk_start) {
                        std::memcpy(chunk_start, this, sizeof(header_t));
                    }
                    std::size_t size () {
                        return  static_cast<std::size_t>(_size);
                    }
                    void size_set (std::size_t newsize) {
                        _size = static_cast<size_mt>(newsize);
                    }
                    
                    bool cuse () {
                        return _flags & flags::cuse;
                    }
                    void cuse_0 () {
                        _flags&= (~flags::cuse);
                    }
                    void cuse_1 () {
                        _flags|= flags::cuse;
                    }
                    
                    bool puse () {
                        return _flags & flags::puse;
                    }
                    void puse_0 () {
                        _flags&= (~flags::puse);
                    }
                    void puse_1 () {
                        _flags|= flags::puse;
                    }
                    
                    struct flags {
                        
                        static constexpr flags_t nuse {0};
                        static constexpr flags_t cuse {1 << 0};
                        static constexpr flags_t puse {1 << 1};
                    };
                    
                private:
                    
                    size_mt _size;
                    flags_t _flags;
                };

                struct fchunk_t {
                    
                    fchunk_t *prev;
                    fchunk_t *next;
                };
                
                free_list (std::span<std::byte> chunk) 
                    : storage(chunk) {
                    head = reinterpret_cast<fchunk_t*>(
                        fchunk_prepare(
                            chunk.data(),
                            chunk.size()
                        )
                    );
                    if (head != nullptr) {
                        head->prev = head;
                        head->next = head;
                    }
                }
                
               ~free_list () {
                    storage = std::span<std::byte>(); 
                    head = nullptr;
                }

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
                void *allocate (std::size_t sizeb, std::size_t alignb) {
                    if (head == nullptr)
                        return nullptr;
                        
                    std::byte *ptr {nullptr}, *chunk_start;
                    
                    header_t header;
                    
                    fchunk_t *cur {head}, *fchunk_new, copy;

                    do {
                        copy = *cur;
                        
                        chunk_start = header_ptr(cur);
                        header.init(chunk_start);
                        
                        prepare(
                            alignb,
                            sizeb,
                            chunk_start,
                            header.size(),
                            ptr,
                            reinterpret_cast<std::byte*&>(fchunk_new)
                        );
                        
                        if (ptr != nullptr) {
                            if (fchunk_new != nullptr) {
                                
                                if (copy.next == cur) {
                                    fchunk_new->prev = fchunk_new;
                                    fchunk_new->next = fchunk_new;
                                }
                                else
                                    fchunk_replace(&copy, fchunk_new);
                                
                                head = fchunk_new;
                            }
                            else {
                                if (copy.next == cur)
                                    head = nullptr;
                                else {
                                
                                    fchunk_cut(&copy);
                                    
                                    head = copy.next;
                                }
                            }

                            break;
                        }
                    }
                    while ((cur = cur->next) != head);
                    
                    return ptr;
                }

                void deallocate (void *ptr) {
                    if (ptr == nullptr)
                        return; 
                        
                    std::byte *chunk_start {header_ptr(ptr)};
                    header_t header;
                    header.init(chunk_start);
                    
                    if (!header.cuse())
                        return;
        
                    std::size_t chunk_size {header.size()};
                        
                    fchunk_t *cur;
                    
                    if (chunk_start > storage.data() && !header.puse()) { // предыдущий свободен
                    
                        std::memcpy(&cur, 
                            chunk_start - sizeof(fchunk_t*),              // получаем заголовок
                            sizeof(fchunk_t*)
                        );

                        chunk_start = header_ptr(cur);                    // расширяем
                        header.init(chunk_start);                         // до текущего
                        chunk_size+= header.size();                       // чанка
                        header.size_set(chunk_size);                      //
                        header.cuse_0();                                  //
                        header.copy(chunk_start);                         //
                                
                        std::memcpy(
                            chunk_start + chunk_size - sizeof(fchunk_t*), // подготавливаем футер
                            &cur, 
                            sizeof(fchunk_t*)
                        );
                    }
                    else {                                                // предыдущий занят
                        cur =  reinterpret_cast<fchunk_t*> (              // переподготавливаем чанк,
                            fchunk_prepare(                               // но пока его невозможно включить в список
                                chunk_start,                              // (в карйнем случае добавляем перед head)
                                chunk_size
                            )                                               
                        );
                        cur->prev = nullptr;
                    }
                    
                    std::byte *next_start = chunk_start + chunk_size;
                    if (next_start < storage.data() + storage.size()) {   // в любом случае смотрим на следующий 

                        header.init(next_start);
                        
                        if (header.cuse()) {                                    // следующий занят
                            header.puse_0();                                    // для него предыдущий свободен
                            header.copy(next_start);                            // (cur уже в списке или будет вставлен позже)
                        }
                        else {                                                  // следующий свободен
                            
                            fchunk_t *next;
                            std::memcpy(                                        // получаем следующий чанк
                                &next, 
                                next_start + header.size() - sizeof(fchunk_t*), 
                                sizeof(fchunk_t*)
                            );
                            
                            if (cur->prev == nullptr)                           // предыдущий занят, тогда
                                fchunk_replace(next, cur);                      // переносим next в cur 
                            else                                                // иначе    
                                fchunk_cut(next);                               // вырезаем next
                                
                            head = cur;
                            
                            chunk_size+= header.size();                         // расширяем
                            header.init(chunk_start);                           //
                            header.size_set(chunk_size);                        // до следующего 
                            header.copy(chunk_start);                           // чанка
                            
                            std::memcpy(
                                chunk_start + chunk_size - sizeof(fchunk_t*),   // подготавливаем футер
                                &cur, 
                                sizeof(fchunk_t*)
                            );
                        }                        
                    }
                    if (cur->prev == nullptr) {       // если соседи заняты
                        
                        if (head == nullptr) {        // есть ли свободные блоки, тогда
                                                      // зацикливаем
                            cur->next = cur;          
                            cur->prev = cur;
                        }
                        else                          // иначе вставляем до
                            fchunk_before(head, cur);
                            
                        head = cur;
                    }
                }
                                                                
                free_list (const free_list &) = delete;
                free_list& operator= (const free_list &) = delete;
                
                free_list (free_list &&) = delete;
                free_list& operator= (free_list &&) = delete;
                
            protected:
                
                std::span<std::byte> storage;
                fchunk_t *head;
                
                std::byte* chunk_prepare (
                    std::size_t alignb,
                    std::size_t sizeb,
                    std::byte *chunk_start, 
                    std::size_t chunk_size
                ) {
                    if (chunk_size < sizeof(header_t) + sizeof(offset_t) + sizeb) 
                        return nullptr;

                    std::byte* ptr {chunk_start + sizeof(header_t) + sizeof(offset_t)};
                    chunk_size-= sizeof(header_t) + sizeof(offset_t);
                    
                    if (ptr > storage.data() + storage.size())
                        return nullptr;
                    
                    if (!std::align(alignb, sizeb, reinterpret_cast<void*&>(ptr), chunk_size))
                        return nullptr;
                    
                    std::ptrdiff_t distance = ptr - chunk_start;
                    if (distance > std::numeric_limits<offset_t>::max())
                        return nullptr;
                    
                    offset_t offset {static_cast<offset_t>(distance)};
                    std::memcpy(
                        offset_ptr(ptr),
                        &offset,
                        sizeof(offset_t)
                    );
                    
                    return ptr;
                }
                
                std::byte* fchunk_prepare (
                    std::byte *chunk_start, 
                    std::size_t chunk_size
                ) {
                    std::byte *ptr = chunk_prepare(
                        alignof(fchunk_t), 
                        sizeof(fchunk_t) + sizeof(fchunk_t*),
                        chunk_start,
                        chunk_size
                    );
                    
                    if (ptr == nullptr)
                        return nullptr;
                    
                    header_t header;
                        header.size_set(chunk_size);
                        header.cuse_0();
                        header.puse_1();
                    header.copy(chunk_start);
                    
                    std::memcpy(
                        chunk_start + chunk_size - sizeof(fchunk_t*),
                        &ptr,
                        sizeof(fchunk_t*)
                    );
                    
                    return ptr;
                }
                void fchunk_after (fchunk_t *prev, fchunk_t *ptr) {
                    
                    prev->next->prev = ptr;
                    ptr->prev = prev;
                    
                    ptr->next = prev->next;
                    prev->next = ptr;
                }
                void fchunk_before (fchunk_t *next, fchunk_t *ptr) {
                    
                    next->prev->next = ptr;
                    ptr->next = next;
                    
                    ptr->prev = next->prev;
                    next->prev = ptr;
                }
                void fchunk_cut (fchunk_t *ptr) {
                    
                    ptr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                }
                void fchunk_replace (fchunk_t *fchunk_old, fchunk_t *fchunk_new) {
                    if (fchunk_old->next == fchunk_old) {
                        fchunk_new->prev = fchunk_new;
                        fchunk_new->next = fchunk_new;
                        return;
                    }
    
                    fchunk_old->prev->next = fchunk_new;
                    fchunk_old->next->prev = fchunk_new;
                    
                    *fchunk_new = *fchunk_old;
                }
                
                void prepare (
                    std::size_t alignb,
                    std::size_t sizeb,
                    std::byte *chunk_start, 
                    std::size_t chunk_size,
                    std::byte* &ptr_data,
                    std::byte* &ptr_fchk
                ) {                    
                    if (sizeb < sizeof(fchunk_t) + sizeof(fchunk_t*)) 
                        sizeb = sizeof(fchunk_t) + sizeof(fchunk_t*);
                    if (alignb < alignof(fchunk_t))
                        alignb = alignof(fchunk_t);
                        
                    ptr_data = chunk_prepare(
                        alignb, 
                        sizeb, 
                        chunk_start, 
                        chunk_size
                    );
                    if (ptr_data != nullptr) {
                        
                        header_t header;

                        std::byte *next_start {ptr_data + sizeb};
                        std::size_t next_size {chunk_size - (next_start - chunk_start)};
                        
                        ptr_fchk = fchunk_prepare(
                            next_start,
                            next_size
                        );

                        if (ptr_fchk == nullptr) {
                            
                            next_start = chunk_start + chunk_size;
                            
                            if (next_start < storage.data() + storage.size()) {
                                header.init(next_start);
                                header.puse_1();
                                header.copy(next_start);
                            }
                        }
                        else 
                            chunk_size-= next_size;
                        
                        header.init(chunk_start);
                            header.size_set(chunk_size);
                            header.cuse_1();
                        header.copy(chunk_start);
                    }
                    else {
                        ptr_fchk = nullptr;
                    }
                }
                
                std::byte* offset_ptr (void *ptr) {
                    return static_cast<std::byte*>(ptr) - sizeof(offset_t);
                }
                std::byte* header_ptr (void *ptr) {
                    
                    offset_t offset;
                    std::memcpy(
                        &offset,
                        offset_ptr(ptr),
                        sizeof(offset_t)
                    );
                    
                    return static_cast<std::byte*>(ptr) - offset;
                }
            
            
            public:
            
                #include "free_list/traits.h"
            
            };
            
            
        }
    }
}

#endif /* ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H */
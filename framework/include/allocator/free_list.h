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

    namespace framework {
        
        namespace allocator {

            template <
                typename size_mt  = std::size_t, 
                typename flags_t  = std::uint8_t,
                typename offset_t = std::uint16_t
            >
            struct free_list {
                
                #include "free_list/traits.h"
                
            protected:
            
                #include "free_list/header_t.h"
                
                struct fchunk_t {
                    
                    fchunk_t *prev;
                    fchunk_t *next;
                };
                
            public:

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
                    
                    if (chunk_start > storage.data() && !header.puse()) {
                    
                        std::memcpy(&cur, 
                            chunk_start - sizeof(fchunk_t*),
                            sizeof(fchunk_t*)
                        );

                        chunk_start = header_ptr(cur);
                        header.init(chunk_start);
                        chunk_size+= header.size();
                        header.size_set(chunk_size);                      
                        header.cuse_0();                                  
                        header.copy(chunk_start);                         
                                
                        std::memcpy(
                            chunk_start + chunk_size - sizeof(fchunk_t*), 
                            &cur, 
                            sizeof(fchunk_t*)
                        );
                    }
                    else {                                                
                        cur =  reinterpret_cast<fchunk_t*> (              
                            fchunk_prepare(                               
                                chunk_start,                              
                                chunk_size
                            )                                               
                        );
                        cur->prev = nullptr;
                    }
                    
                    std::byte *next_start = chunk_start + chunk_size;
                    if (next_start < storage.data() + storage.size()) {

                        header.init(next_start);
                        
                        if (header.cuse()) {                                    
                            header.puse_0();                                    
                            header.copy(next_start);                            
                        }
                        else {                                                  
                            
                            fchunk_t *next;
                            std::memcpy(                                        
                                &next, 
                                next_start + header.size() - sizeof(fchunk_t*), 
                                sizeof(fchunk_t*)
                            );
                            
                            if (cur->prev == nullptr)                           
                                fchunk_replace(next, cur);                       
                            else                                                
                                fchunk_cut(next);                               
                                
                            head = cur;
                            
                            chunk_size+= header.size();                         
                            header.init(chunk_start);                           
                            header.size_set(chunk_size);                         
                            header.copy(chunk_start);                           
                            
                            std::memcpy(
                                chunk_start + chunk_size - sizeof(fchunk_t*),   
                                &cur, 
                                sizeof(fchunk_t*)
                            );
                        }                        
                    }
                    if (cur->prev == nullptr) { 
                        
                        if (head == nullptr) { 

                            cur->next = cur;          
                            cur->prev = cur;
                        }
                        else 
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
            };
            
            
        }
    }
}

#endif /* ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H */

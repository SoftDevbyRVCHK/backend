#ifndef ODYSSEY_UTILITY_ALLOCATOR_FREELISTALLOCATOR_H
#define ODYSSEY_UTILITY_ALLOCATOR_FREELISTALLOCATOR_H

namespace odyssey {

    namespace utility {
        
        namespace allocator {
            
            struct free_list_allocator {
                
                struct free_memb {
                    free_memb *next;
                    free_memb *prev;
                };
                
                static free_list_allocator* prepare (std::span<std::byte> memb) {
                    
                    return align(alignof(free_list_allocator), sizeof(free_list_allocator), memb) ?
                           ::new(memb.data()) free_list_allocator(memb.subspan(sizeof(free_list_allocator))) :
                           nullptr;
                }
                static void release (free_list_allocator *heap) {
                    
                    heap->~free_list_allocator();
                }

                template <typename T>
                T* allocate () {return allocate<T>(1);}
                template <typename T>
                T* allocate (std::size_t n) { // заглушка
                    return static_cast<T*>(::operator new(n * sizeof(T)));
                }

                template <typename T>
                void deallocate (T* ptr) { // заглушка
                    ::operator delete(ptr);
                }
                
                void clear () {
                    *this = free_list_allocator(storage);
                }
                
            protected:
                
                std::span<std::byte> storage;
                free_memb *head;
                
                free_list_allocator (std::span<std::byte> memb) : storage(memb), head(prepare<free_memb>(storage, 1)){
                    head->prev = nullptr;
                    head->next = nullptr;
                }
                                
                free_list_allocator (const free_list_allocator &) = default;
                free_list_allocator& operator= (const free_list_allocator &) = default;
                
                free_list_allocator (free_list_allocator &&) = default;
                free_list_allocator& operator= (free_list_allocator &&) = default;
                
               ~free_list_allocator () {storage = std::span<std::byte>(); head = nullptr;}
                
                template <typename T>
                T* prepare (std::span<std::byte> memb, std::size_t n) {
                    
                    std::byte  *prep_block = memb.data();
                    std::size_t prep_size  = memb.size();
                    
                    std::size_t sizeb {n * sizeof(T)};
                    memb = memb.subspan(sizeof(std::size_t) + sizeof(std::byte*));
                    
                    if (!align(alignof(T), sizeb, memb))
                        return nullptr;

                    std::memcpy(prep_block, &prep_size, sizeof(std::size_t));
                    std::memcpy(memb.data() - sizeof(std::byte*), &prep_block, sizeof(std::byte*));
                    
                    return reinterpret_cast<T*>(memb.data());
                }
            };
        }
    }
}

#endif /* ODYSSEY_UTILITY_ALLOCATOR_FREELISTALLOCATOR_H */
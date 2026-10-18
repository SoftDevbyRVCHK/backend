#ifndef ODYSSEY_UTILITY_ALLOCATOR_BUMP_H
#define ODYSSEY_UTILITY_ALLOCATOR_BUMP_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <cstring>
#include <span>
#include <new>
#include <memory>

namespace odyssey {

    namespace utility {
        
        namespace allocator {
            
            struct bump {
           
                bump (std::span<std::byte> memb) 
                    : storage(memb), cur(memb.data()) {}
               
               ~bump () {
                    storage = std::span<std::byte>();
                    cur = nullptr;
                }

                template <typename T>
                T* allocate () { 
                    return reinterpret_cast<T*>(allocate(1 * sizeof(T), alignof(T)));
                }
                template <typename T>
                T* allocate (std::size_t n) { 
                    return reinterpret_cast<T*>(allocate(n * sizeof(T), alignof(T))); 
                }
                void* allocate (std::size_t sizeb, std::size_t alignb) {
                    
                    std::size_t space {
                        storage.size() - static_cast<size_t>(cur - storage.data())
                    };
                    void* ptr {std::align(alignb, sizeb, reinterpret_cast<void*&>(cur), space)};
                    
                    if (ptr != nullptr)
                        cur+= sizeb;
                    
                    return ptr;  
                }

                void deallocate () {
                    cur = storage.data();
                }
                                
                bump (const bump &) = delete;
                bump& operator= (const bump &) = delete;
                
                bump (bump &&) = delete;
                bump& operator= (bump &&) = delete;
                
            protected:

                std::span<std::byte> storage;
                std::byte *cur;
            };
        }
    }
}

#endif /* ODYSSEY_UTILITY_ALLOCATOR_BUMP_H */
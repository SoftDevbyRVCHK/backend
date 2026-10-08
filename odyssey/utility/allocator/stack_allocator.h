#ifndef ODYSSEY_UTILITY_ALLOCATOR_STACKALLOCATOR_H
#define ODYSSEY_UTILITY_ALLOCATOR_STACKALLOCATOR_H

namespace odyssey {

    namespace utility {
        
        namespace allocator {
            
            struct stack_allocator {
                
                static stack_allocator* prepare (std::span<std::byte> memb) {
                    
                    return align(alignof(stack_allocator), sizeof(stack_allocator), memb) ?
                           ::new(memb.data()) stack_allocator(memb.subspan(sizeof(stack_allocator))) :
                           nullptr;
                }
                static void release (stack_allocator *stack) {
                    
                    stack->~stack_allocator();
                }

                template <typename T>
                T* allocate () { return allocate<T>(1); }
                template <typename T>
                T* allocate (std::size_t n) {
                    
                    std::byte *old_cur = cur;
                    
                    std::byte* *old_prev = prepare<std::byte*>(1);
                    if (old_prev == nullptr)  return nullptr;

                    T *result = prepare<T>(n);
                    if (result == nullptr)  {
                        cur = old_cur;
                        return nullptr;
                    }
                                        
                    *old_prev = prev;
                    prev = reinterpret_cast<std::byte*>(old_prev);
                    
                    return result;
                }

                void deallocate () {
                    if (prev != nullptr) {
                        
                        std::byte* old_prev = *reinterpret_cast<std::byte**>(prev);

                        cur = prev;
                        prev = old_prev;
                    }
                }
                
                void clear () {
                    *this = stack_allocator(storage);
                }
                
            protected:

                std::span<std::byte> storage;
                std::byte *prev;
                std::byte *cur;
                
                stack_allocator (std::span<std::byte> memb) : storage(memb), prev(nullptr), cur(memb.data()) {}
                
                stack_allocator (const stack_allocator &) = default;
                stack_allocator& operator= (const stack_allocator &) = default;
                
                stack_allocator (stack_allocator &&) = default;
                stack_allocator& operator= (stack_allocator &&) = default;
               
               ~stack_allocator () {storage = std::span<std::byte>(); prev = nullptr; cur = nullptr;}
                
                template <typename T>
                T* prepare (std::size_t n) {
                
                    std::size_t sizeb = n * sizeof(T);
                    std::span<std::byte> memb(storage.subspan(cur - storage.data()));
                        
                    if (!align(alignof(T), sizeb, memb))
                        return nullptr;
                    
                    cur = memb.data() + sizeb;
                    
                    return reinterpret_cast<T*>(memb.data());
                }
            };
        }
    }
}

#endif /* ODYSSEY_UTILITY_ALLOCATOR_STACKALLOCATOR_H */
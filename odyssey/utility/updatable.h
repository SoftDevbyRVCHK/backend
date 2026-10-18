#ifndef ODYSSEY_UTILITY_UPDATABLE_H
#define ODYSSEY_UTILITY_UPDATABLE_H

#include <cstddef>
#include <vector>

namespace odyssey {

    namespace utility {
                
        struct updatable {

            virtual void        update    () = 0;
            virtual            ~updatable () = default;
        };
        
        template <typename allocator_t>
        struct update_context {

            using vector_allocator_t = typename std::allocator_traits<allocator_t>::template rebind_alloc<updatable*>;
            using vector_t = std::vector<updatable*, vector_allocator_t>;

            explicit update_context(const allocator_t& alloc) 
                : ups(vector_allocator_t(alloc)) {}

            void update() {
                for (auto* up : ups) {
                    if (up) up->update();
                }
            }

            bool push(updatable *up) {
                if (up == nullptr) return false;
                
                try {
                    ups.push_back(up);
                } 
                catch (const std::bad_alloc&) {
                    return false;
                }
                return true;
            }
            
            void erase(updatable *up) {
                for (std::size_t i = 0; i < ups.size(); i++) {
                    if (up == ups[i]) {
                        ups[i] = ups.back();
                        ups.pop_back();
                        return;
                    }
                }
            }

        protected:
            vector_t ups;
        };
    }
}

#endif /* ODYSSEY_UTILITY_UPDATABLE_H */
#ifndef ODYSSEY_UTILITY_UPDATABLE_H
#define ODYSSEY_UTILITY_UPDATABLE_H

#include <cstddef>
#include <vector>

namespace odyssey {

    namespace utility {
                
        struct updatable {

            virtual std::size_t id        () = 0;
            virtual void        update    () = 0;
            virtual            ~updatable () = default;
        };
        
        struct update_context{
    
            update_context () : ups() {}

            void update () {
                for (std::size_t i = 0; i < ups.size(); i++)
                    ups[i]->update();
            }

            void erase (std::size_t id) {
                for (std::size_t i = 0; i < ups.size(); i++)
                    if (id == ups[i]->id()) {
                        ups[i] = ups.back();
                        ups.pop_back();
                        return;
                    }
            }

            bool push (updatable* up) {
                if (up == nullptr) return false;
                
                try {
                    ups.push_back(up);
                } 
                catch (const std::bad_alloc&) {
                    return false;
                }
                return true;
            }
            
            std::vector<updatable*> get_objects() {return ups;}
        protected:

            std::vector<updatable*> ups;
        };
    }
}

#endif /* ODYSSEY_UTILITY_UPDATABLE_H */
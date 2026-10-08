#ifndef ODYSSEY_FRAMEWORK_SCENE_H
#define ODYSSEY_FRAMEWORK_SCENE_H

#include <cstddef>
#include "../utility.h"

namespace odyssey {

    namespace framework {
        
        namespace scene {
            
            namespace allocator = odyssey::utility::allocator;
            
            struct scene_context : public utility::serializable{

                virtual void run           () = 0;
                virtual void stop          () = 0;
                
                virtual     ~scene_context () = default;
                
            protected:
                
                std::vector<std::byte> mem;
                
                allocator::stack_allocator *stack;
                allocator::free_list_allocator *heap;
            };
        }
    }
}

#include "scene/map.h"
#include "scene/planetarium.h"

#endif /* ODYSSEY_FRAMEWORK_SCENE_H */
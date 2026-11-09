#ifndef ODYSSEY_FRAMEWORK_ENGINE_SCHEDULER_H
#define ODYSSEY_FRAMEWORK_ENGINE_SCHEDULER_H

#include <tuple>
#include <vector>
#include <cstdint>
#include <memory>

#include "transformer.h"
#include "manipulator.h"

namespace odyssey::framework::engine {

    template <
        template <typename> typename allocator_t = std::allocator, 
        typename... interface_types
    > 
    struct scheduler { 
        
        void run ();

    private: 

        std::tuple< 
            std::vector<
                transformer<interface_types>,
                allocator_t<transformer<interface_types>>
            >... 
        > transformers; 

        std::tuple< 
            std::vector<
                manipulator<interface_types>,
                allocator_t<manipulator<interface_types>>
            >... 
        > manipulators; 
    };
}

#endif /* ODYSSEY_FRAMEWORK_ENGINE_SCHEDULER_H */
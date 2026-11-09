#ifndef ODYSSEY_FRAMEWORK_ENGINE_H
#define ODYSSEY_FRAMEWORK_ENGINE_H

namespace odyssey {

    namespace framework {

        namespace engine {

            template <
                template <typename> typename allocator, 
                typename... interface_types
            >
            struct sheduler;

            template <typename interface_t>
            struct transformer;

            template <typename interface_t>
            struct manipulator;
        }
    }
}

#include "engine/scheduler.h"
#include "engine/transformer.h"
#include "engine/manipulator.h"

#endif /* ODYSSEY_FRAMEWORK_ENGINE_H */
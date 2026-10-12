#ifndef ODYSSEY_FRAMEWORK_SCENE_PLANETARIUM_H
#define ODYSSEY_FRAMEWORK_SCENE_PLANETARIUM_H

#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

#include <cstddef>
#include <vector>

#include "../galactic/celestial.h"
#include "../../utility.h"

namespace odyssey {

    namespace framework {
        
        namespace scene {

            // игровая зона
            struct planetarium_context : public scene_context {                 

                planetarium_context () {}
                
            protected:
            
                std::atomic<bool> started;
                std::mutex upc_mutex;
                utility::update_context upc;
                
            };
        }
    }
}

#include "planetarium/client.h"

#endif /* ODYSSEY_FRAMEWORK_SCENE_PLANETARIUM_H */
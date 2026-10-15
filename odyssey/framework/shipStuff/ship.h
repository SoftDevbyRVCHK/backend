#ifndef ODYSSEY_FRAMEWORK_SHIPSTUFF_SHIP_H
#define ODYSSEY_FRAMEWORK_SHIPSTUFF_SHIP_H

#include "../../utility.h"

namespace odyssey {

    namespace framework {
        
        namespace shipStuff {
            
            using namespace utility;
            
            template <typename scalar_t>
            struct ship : public physics::dimension2::rigidbody<scalar_t>{
                
                
            };
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_SHIPSTUFF_SHIP_H */

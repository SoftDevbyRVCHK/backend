#ifndef ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_H
#define ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_H

namespace odyssey {

    namespace framework {
        
        namespace physics {
            
            namespace movement {
                
                template <typename scalar_t>
                struct movable : virtual public framework::physics::dimension2::rigidbody<scalar_t> {

                    using vector_t = framework::physics::dimension2::rigidbody<scalar_t>::vector_t;
                    using vector_in = const vector_t&;
                    using scalar_in = const scalar_t&;

                    virtual void move (scalar_in dt) = 0;
                    virtual ~movable () = default;
                };
            }
        }
    }
}

#include "movement/accumulator.h"
#include "movement/orbiter.h"
#include "movement/accorb.h"

#endif /* ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_H */
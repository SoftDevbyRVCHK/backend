#ifndef ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_ACCUMULATOR_H
#define ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_ACCUMULATOR_H

#include <cmath>

namespace odyssey {

    namespace framework {

        namespace physics {

            namespace movement {

                template <typename scalar_t>
                struct accumulator : virtual public movable<scalar_t> {

                    using movable<scalar_t>::v;
                    using movable<scalar_t>::w;
                    using movable<scalar_t>::pos;
                    using movable<scalar_t>::i;
                    using movable<scalar_t>::j;
                    using movable<scalar_t>::_inv_mass;
                    using movable<scalar_t>::_inv_inertia;

                    using vector_t = movable<scalar_t>::vector_t;
                    using vector_in = movable<scalar_t>::vector_in;
                    using scalar_in = movable<scalar_t>::scalar_in;

                    void accumulate (scalar_in dt) {

                        v+= _force_accumulator * _inv_mass * dt;

                        w+= _torque_accumulator * _inv_inertia * dt;

                        _force_accumulator = vector_t(0.0, 0.0);
                        _torque_accumulator = static_cast<scalar_t>(0);
                    }

                    void add_force  (vector_in force)   {_force_accumulator+= force;}
                    void add_torque (scalar_in torque) {_torque_accumulator+= torque;}

                    virtual void move (scalar_in dt) override {

                        accumulate(dt);

                        pos+= v * dt;

                        scalar_t d_theta = w * dt;
                        
                        vector_t roter(std::cos(d_theta), std::sin(d_theta));
                        
                        i.spin(roter);
                        j.spin(roter);
                        
                        i = i.normal();
                        j = j.normal();
                    }

                    virtual ~accumulator () = default;

                protected:
                    
                    vector_t _force_accumulator;
                    scalar_t _torque_accumulator;
                };
            }
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_ACCUMULATOR_H */
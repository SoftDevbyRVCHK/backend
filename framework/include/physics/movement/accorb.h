#ifndef ODYSSEY_GAME_MOVEMENT_ACCUMORBITER_H
#define ODYSSEY_GAME_MOVEMENT_ACCUMORBITER_H

namespace odyssey {

    namespace framework {
        
        namespace physics {

            namespace movement {

                template <typename scalar_t>
                struct accorb : virtual public accumulator<scalar_t>, virtual public orbiter<scalar_t> {

                    using movement::movable<scalar_t>::pos;
                    using movement::movable<scalar_t>::v;
                    using movement::movable<scalar_t>::w;
                    using movement::movable<scalar_t>::i;
                    using movement::movable<scalar_t>::j;
                    using movement::movable<scalar_t>::_inv_mass;
                    using movement::movable<scalar_t>::_inv_inertia;

                    using scalar_in = movable<scalar_t>::scalar_in;
                    using vector_in = movable<scalar_t>::vector_in;
                    using orbit_t   = orbiter<scalar_t>::orbit_t;
                    using orbit_in = const orbit_t &;

                    accorb() {
                        like_accumulator();
                    }

                    virtual void move(scalar_in dt) override {
                        current_movement_subobject->move(dt);
                    }

                    void like_orbiter (orbit_in target_orbit, scalar_in true_anomaly, vector_in center_pos, scalar_in mu) {

                        this->set_orbit_state(target_orbit, mu, true_anomaly);

                        current_movement_subobject = static_cast<orbiter<scalar_t>*>(this);
                    }

                    void like_accumulator () {

                        this->_force_accumulator = typename movable<scalar_t>::vector_t(0.0, 0.0);
                        this->_torque_accumulator = static_cast<scalar_t>(0);

                        current_movement_subobject = static_cast<accumulator<scalar_t>*>(this);
                    }

                    virtual ~accorb () = default;

                protected:
                    movable<scalar_t>* current_movement_subobject;
                };
            }
        }
    }
}

#endif /* ODYSSEY_GAME_MOVEMENT_ACCUMORBITER_H */

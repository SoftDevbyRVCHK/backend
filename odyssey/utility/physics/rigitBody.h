#ifndef ODYSSEY_UTILITY_PHYSICS_RIGITBODY_H
#define ODYSSEY_UTILITY_PHYSICS_RIGITBODY_H

#include "../math.h"

#include <cmath>

namespace odyssey {

    namespace utility {
        
        namespace physics {
            
            namespace dimension2 {
                
                template <typename scalar_t>
                struct rigidbody {

                    rigidBody2 () 
                        : pos(0.0, 0.0) 
                        , i(1.0, 0.0) 
                        , j(0.0, 1.0)
                        ,  v(0.0, 0.0)
                        , w(0.0) 
                    {
                        setMass(static_cast<scalar_t>(1.0));
                        setInertia(static_cast<scalar_t>(1.0));
                    }
                    
                    void setMass (const scalar_t &new_mass) {
                        if (new_mass > 0.0) {
                            mass = new_mass;
                            inv_mass = static_cast<scalar_t>(1.0) / mass;
                        }
                        else {
                            mass = 0.0;
                            inv_mass = 0.0;
                        }
                    }
                    
                    scalar_t getMass    () const {return mass;}
                    scalar_t getInvMass () const {return inv_mass;}
                    
                    void setInertia (const scalar_t &new_inertia) {
                        if (new_inertia > 0.0) {
                            inertia = new_inertia;
                            inv_inertia = static_cast<scalar_t>(1.0) / inertia;
                        }
                        else {
                            inertia = 0.0;
                            inv_inertia = 0.0;
                        }
                    }
                    
                    scalar_t getInertia    () const {return inertia;}
                    scalar_t getInvInertia () const {return inv_inertia;}

                    void updateState (const VectorT &force, const scalar_t &torque, const scalar_t dt) {
                        w += torque * inv_inertia * dt;
                        
                        scalar_t d_theta = w * dt;
                        
                        VectorT rotation_vector(std::cos(d_theta), std::sin(d_theta));
                        
                        i.spin(rotation_vector);
                        j.spin(rotation_vector);
                        
                        i = i.normal();
                        j = j.normal();

                        v += force * (inv_mass * dt);

                        pos += v * dt;
                    }
                    
                                 // положение на плоскости 
                    VectorT pos; // позиция  
                    VectorT i;   // направление "вперёд"
                    VectorT j;   // направление "вбок"
                    
                    
                    VectorT v; // поступательная скорость
                    scalar_t w; // вращательная скорость

                protected:

                    scalar_t mass;       // масса - мера инерции поступательного движения
                    scalar_t inv_mass;
                    
                    scalar_t inertia;    // момент инерции - мера инерции вращательного движения
                    scalar_t inv_inertia;
                };
            }
        }
    }
}

#endif /* ODYSSEY_UTILITY_PHYSICS_RIGITBODY_H */
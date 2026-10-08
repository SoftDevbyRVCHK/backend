#ifndef ODYSSEY_FRAMEWORK_PHYSICS_RIGITBODY_H
#define ODYSSEY_FRAMEWORK_PHYSICS_RIGITBODY_H

#include <cmath>
#include "../../concepts/math.h"

namespace odyssey {

    namespace framework {
        
        namespace physics {
            
            template <typename VectorT>
            //requires concepts::math::iVector2<VectorT>
            struct rigidBody2 {
                
                using ScalarT = concepts::math::VectorScalarT<VectorT>;

                rigidBody2 () 
                    : pos(0.0, 0.0) 
                    , i(1.0, 0.0) 
                    , j(0.0, 1.0)
                    ,  v(0.0, 0.0)
                    , w(0.0) 
                {
                    setMass(static_cast<ScalarT>(1.0));
                    setInertia(static_cast<ScalarT>(1.0));
                }
                
                void setMass (const ScalarT &new_mass) {
                    if (new_mass > 0.0) {
                        mass = new_mass;
                        inv_mass = static_cast<ScalarT>(1.0) / mass;
                    }
                    else {
                        mass = 0.0;
                        inv_mass = 0.0;
                    }
                }
                
                ScalarT getMass    () const {return mass;}
                ScalarT getInvMass () const {return inv_mass;}
                
                void setInertia (const ScalarT &new_inertia) {
                    if (new_inertia > 0.0) {
                        inertia = new_inertia;
                        inv_inertia = static_cast<ScalarT>(1.0) / inertia;
                    }
                    else {
                        inertia = 0.0;
                        inv_inertia = 0.0;
                    }
                }
                
                ScalarT getInertia    () const {return inertia;}
                ScalarT getInvInertia () const {return inv_inertia;}

                void updateState (const VectorT &force, const ScalarT &torque, const ScalarT dt) {
                    w += torque * inv_inertia * dt;
                    
                    ScalarT d_theta = w * dt;
                    
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
                ScalarT w; // вращательная скорость

            protected:

                ScalarT mass;       // масса - мера инерции поступательного движения
                ScalarT inv_mass;
                
                ScalarT inertia;    // момент инерции - мера инерции вращательного движения
                ScalarT inv_inertia;
            };
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_PHYSICS_RIGITBODY_H */
#ifndef ODYSSEY_FRAMEWORK_PHYSICS_RIGITBODY_H
#define ODYSSEY_FRAMEWORK_PHYSICS_RIGITBODY_H

#include "../math.h"

#include <cmath>

namespace odyssey {

    namespace framework {
        
        namespace physics {
            
            namespace dimension2 {
                
                template <typename scalar_t>
                struct rigidbody {
                    
                    using vector_t = math::dimension2::vector<scalar_t>;
                    using vector_in = const vector_t &;
                    using scalar_in = const scalar_t &;

                    rigidbody () 
                        : pos(0.0, 0.0) 
                        , i(1.0, 0.0) 
                        , j(0.0, 1.0)
                        ,  v(0.0, 0.0)
                        , w(0.0) 
                    {
                        mass(static_cast<scalar_t>(1.0));
                        inertia(static_cast<scalar_t>(1.0));
                    }

                    rigidbody (vector_in pos, vector_in i, vector_in j, scalar_in init_mass, scalar_in init_inertia) 
                        : pos(pos) 
                        , i(i) 
                        , j(j)
                        , v(0.0, 0.0)
                        , w(0.0) 
                    {
                        mass(init_mass);
                        inertia(init_inertia);
                    }
                    
                    void mass (scalar_in new_mass) {
                        if (new_mass > 0.0) {
                            _mass = new_mass;
                            _inv_mass = static_cast<scalar_t>(1.0) / _mass;
                        }
                        else {
                            _mass = 0.0;
                            _inv_mass = 0.0;
                        }
                    }
                    
                    scalar_in mass    () const {return _mass;}
                    scalar_in invmass () const {return _inv_mass;}
                    
                    void inertia (scalar_in new_inertia) {
                        if (new_inertia > 0.0) {
                            _inertia = new_inertia;
                            _inv_inertia = static_cast<scalar_t>(1.0) / _inertia;
                        }
                        else {
                            _inertia = 0.0;
                            _inv_inertia = 0.0;
                        }
                    }
                    
                    scalar_in inertia    () const {return _inertia;}
                    scalar_in invinertia () const {return _inv_inertia;}
                    
                                  // положение на плоскости 
                    vector_t pos; // позиция  
                    vector_t i;   // направление "вперёд"
                    vector_t j;   // направление "вбок"
                    
                    
                    vector_t v; // поступательная скорость
                    scalar_t w; // вращательная скорость

                protected:

                    scalar_t _mass;       // масса - мера инерции поступательного движения
                    scalar_t _inv_mass;
                    
                    scalar_t _inertia;    // момент инерции - мера инерции вращательного движения
                    scalar_t _inv_inertia;
                };
            }
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_PHYSICS_RIGITBODY_H */
#ifndef ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H
#define ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H

#include "../../polymorf/math/iVector2.h"
#include "../../polymorf/math/iEllipse2.h"

namespace odyssey {
    
    namespace framework {
        
        namespace galactic {
            
            /*
                объект взаимодествия игрока по орбите на игровом пространстве.
            */
            
            template <typename VectorT, typename ElipseT> 
            requires polymorf::math::iVector2<VectorT> && polymorf::math::iEllipse2<ElipseT>
            class celestial {
            public:
                struct orbitT : public ElipseT {
                public:
                
                    using ElipseT::ElipseT;
                    
                    VectorT::valueT proculToPt (const VectorT &pt);
                };
                
                orbitT orbit;
                VectorT::valueT mass;
                VectorT::valueT radius;
                VectorT::valueT gravity;
            };
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H */
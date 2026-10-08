#ifndef ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H
#define ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H

#include <optional>
#include "../../concepts/math.h"
#include "../../utility/serialization.h"
#include "../../utility/updatable.h"
#include "../physics/rigitBody.h"

namespace odyssey {
    
    namespace framework {
        
        namespace galactic {
            
            template <typename VectorT, typename EllipseT>
            requires concepts::math::iVector2<VectorT> && concepts::math::iEllipse2<EllipseT>
            struct orbit : public EllipseT {

                using ScalarT = concepts::math::VectorScalarT<VectorT>;

                                           // Физические параметры движения Кеплера по эллиптическим рельсам:
                ScalarT mus;               // Гравитационный параметр центрального тела (GM)
                ScalarT mean_motion;       // Среднее движение (угловая скорость вращения по орбите), рад/с
                ScalarT mean_anomaly;      // Средняя аномалия M (линейное время, пересчитанное в угол)

                
                using EllipseT::ellipse;   // Наследуем конструкторы вашего базового класса эллипса
            };

            template <typename VectorT, typename EllipseT>
            requires concepts::math::iVector2<VectorT> && concepts::math::iEllipse2<EllipseT>
            struct celestial {

                using ScalarT = concepts::math::VectorScalarT<VectorT>;
                using orbitT = orbit<VectorT, EllipseT>;

                ScalarT GM;          // Гравитационный параметр небесного тела
                ScalarT radiusSur;   // Физический радиус поверхности (твердая земля)
                ScalarT radiusAtm;   // Радиус верхних слоев атмосферы (зона торможения)
                VectorT pos;         // Глобальная позиция центра тела
                VectorT v;           // Глобальный вектор скорости тела 

                orbitT orb;          // Собственная орбита этого тела вокруг центрального светила

                std::optional<orbitT> getOrbitFrom (const VectorT &ship_pos, const VectorT &ship_v);

                void move (const ScalarT &dt);
            };
            
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H */
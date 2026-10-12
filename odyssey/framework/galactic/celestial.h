#ifndef ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H
#define ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H

#include <optional>
#include "../../utility.h"

namespace odyssey {
    
    namespace framework {
        
        namespace galactic {
            
            template <typename scalar_t>
            struct orbit_t : public utility::math::dimension2::ellipse<scalar_t> {
                
                using ellipse_t = utility::math::dimension2::ellipse<scalar_t>;
                
                                               // Физические параметры движения Кеплера по эллиптическим рельсам:
                scalar_t mus;                  // Гравитационный параметр центрального тела (GM)
                scalar_t mean_motion;          // Среднее движение (угловая скорость вращения по орбите), рад/с
                scalar_t mean_anomaly;         // Средняя аномалия M (линейное время, пересчитанное в угол)

                using ellipse_t::ellipse_t;    // Наследуем конструкторы вашего базового класса эллипса
            };

            template <typename scalar_t>
            struct celestial {
                
                using vector_t = utility::math::dimension2::vector<scalar_t>;
                
                scalar_t GM;                     // Гравитационный параметр небесного тела
                scalar_t radiusSur;              // Физический радиус поверхности (твердая земля)
                scalar_t radiusAtm;              // Радиус верхних слоев атмосферы (зона торможения)
                vector_t pos;                    // Глобальная позиция центра тела
                vector_t v;                      // Глобальный вектор скорости тела 

                orbit_t<scalar_t> orb;           // Собственная орбита этого тела вокруг центрального светила

                std::optional<orbit_t<scalar_t>> getOrbitFrom (const vector_t &ship_pos, const vector_t &ship_v);

                void move (const scalar_t &dt);
            };
            
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_GALACTIC_CELESTIAL_H */
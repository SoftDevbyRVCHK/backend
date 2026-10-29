#ifndef ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_ORBITER_H
#define ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_ORBITER_H

#include <cmath>

namespace odyssey {

    namespace framework {

        namespace physics {

            namespace movement {

                template <typename scalar_t>
                struct orbiter : virtual public movable<scalar_t> {

                    using movable<scalar_t>::v;
                    using movable<scalar_t>::w;
                    using movable<scalar_t>::pos;
                    using movable<scalar_t>::i;
                    using movable<scalar_t>::j;
                    using movable<scalar_t>::_inv_mass;
                    using movable<scalar_t>::_inv_inertia;

                    using vector_t = typename movable<scalar_t>::vector_t;
                    using vector_in = typename movable<scalar_t>::vector_in;
                    using scalar_in = typename movable<scalar_t>::scalar_in;
                    
                    struct orbit_t {
                        
                        using vector_t  = typename framework::math::dimension2::vector<scalar_t>;
                        using scalar_in = const scalar_t &;

                        scalar_t periapsis_distance;     // Минимальное расстояние до центра масс (r_p)
                        scalar_t eccentricity;           // Эксцентриситет (e): 0 - круг, <1 - эллипс
                        scalar_t argument_of_periapsis;  // Угол поворота линии апсид (перицентра) в радианах (omega)

                        // Конструктор по умолчанию
                        orbit_t() 
                            : periapsis_distance(0.0), eccentricity(0.0), argument_of_periapsis(0.0) {}

                        // Инициализирующий构造тор
                        orbit_t(scalar_in rp, scalar_in e, scalar_in omega)
                            : periapsis_distance(rp), eccentricity(e), argument_of_periapsis(omega) {}

                        // Получение радиус-вектора точки на орбите относительно фокуса по истинной аномалии t
                        vector_t getFromT (scalar_in t) const {
                            if (periapsis_distance <= static_cast<scalar_t>(0)) {
                                return vector_t(0.0, 0.0);
                            }

                            // 1. Вычисляем универсальный фокальный параметр p = r_p * (1 + e)
                            scalar_t p = periapsis_distance * (static_cast<scalar_t>(1.0) + eccentricity);

                            // 2. Находим полярный радиус (расстояние от фокуса до объекта под углом t)
                            scalar_t denominator = static_cast<scalar_t>(1.0) + eccentricity * std::cos(t);
                            
                            if (std::abs(denominator) < static_cast<scalar_t>(1e-6)) {
                                return vector_t(0.0, 0.0); 
                            }
                            scalar_t r = p / denominator;

                            // 3. Вычисляем позицию в локальной системе координат орбиты (ось X смотрит на перицентр)
                            vector_t local_pos(r * std::cos(t), r * std::sin(t));

                            // 4. Поворачиваем локальный вектор на аргумент перицентра (ориентация в пространстве)
                            vector_t rotator(std::cos(argument_of_periapsis), std::sin(argument_of_periapsis));
                            
                            return local_pos.spin(rotator);
                        }
                    };
           
                    using orbit_in = const orbit_t &;

                    // Метод для инициализации орбиты извне (например, из менеджера или класса celestial)
                    void set_orbit_state(orbit_in new_orbit, scalar_t mu, scalar_t true_anomaly) {
                        orbit = new_orbit;
                        _mu = mu;
                        
                        scalar_t rp = orbit.periapsis_distance;
                        scalar_t e = orbit.eccentricity;
                        
                        if (rp > static_cast<scalar_t>(0) && _mu > static_cast<scalar_t>(0)) {
                            // Большая полуось: a = rp / (1 - e)
                            scalar_t a = rp / (static_cast<scalar_t>(1.0) - e);
                            
                            // Третий закон Кеплера: среднее движение n = sqrt(mu / a^3)
                            _mean_motion = std::sqrt(_mu / (a * a * a));
                            
                            // Переводим геометрический угол (true_anomaly) в эксцентрическую аномалию E
                            scalar_t E = static_cast<scalar_t>(2.0) * std::atan(std::sqrt((static_cast<scalar_t>(1.0) - e) / (static_cast<scalar_t>(1.0) + e)) * std::tan(true_anomaly / static_cast<scalar_t>(2.0)));
                            
                            // Находим изначальную среднюю аномалию M (которая будет меняться со временем линейно)
                            _M = E - e * std::sin(E);
                        }
                    }

                    virtual void move (scalar_in dt) override {

                        // 1. Средняя аномалия меняется строго линейно во времени: M = M0 + n * dt
                        _M += _mean_motion * dt;

                        // Нормализуем значение угла M в границы [0, 2*pi]
                        const scalar_t twopi = static_cast<scalar_t>(2.0 * M_PI);
                        _M = std::fmod(_M, twopi);
                        if (_M < static_cast<scalar_t>(0)) {
                            _M += twopi;
                        }

                        scalar_t e = orbit.eccentricity;

                        // 2. Численное решение уравнения Кеплера: E - e * sin(E) = M
                        // Метода Ньютона-Рафсона из 5 итераций более чем достаточно для точности типов с плавающей точкой
                        scalar_t E = _M; 
                        for (int iter = 0; iter < 5; ++iter) {
                            E -= (E - e * std::sin(E) - _M) / (static_cast<scalar_t>(1.0) - e * std::cos(E));
                        }

                        // 3. Находим истинную аномалию (угол t) из эксцентрической аномалии E
                        scalar_t true_anomaly = static_cast<scalar_t>(2.0) * std::atan2(
                            std::sqrt(static_cast<scalar_t>(1.0) + e) * std::sin(E / static_cast<scalar_t>(2.0)), 
                            std::sqrt(static_cast<scalar_t>(1.0) - e) * std::cos(E / static_cast<scalar_t>(2.0))
                        );

                        // 4. Сохраняем старые координаты для апдейта физического вектора скорости
                        vector_t old_pos = pos;

                        // 5. Вычисляем новую позицию на орбите
                        pos = orbit.getFromT(true_anomaly);

                        // 6. Рассчитываем актуальную мгновенную линейную скорость v = dx / dt
                        v = (pos - old_pos) / dt;

                        // 7. Поведение собственного вращения твердого тела вокруг своей оси (параметр w)
                        scalar_t d_theta = w * dt;
                        vector_t roter(std::cos(d_theta), std::sin(d_theta));
                        
                        i.spin(roter);
                        j.spin(roter);
                        
                        i = i.normal();
                        j = j.normal();
                    }

                    virtual ~orbiter () = default;

                protected:
                    
                    orbit_t orbit;

                    scalar_t _mu {0.0};          // Гравитационный параметр центрального тела (GM)
                    scalar_t _M {0.0};           // Текущая средняя аномалия
                    scalar_t _mean_motion {0.0}; // Скорость изменения средней аномалии (угловая частота n)
                };
            }
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_PHYSICS_MOVEMENT_ORBITER_H */

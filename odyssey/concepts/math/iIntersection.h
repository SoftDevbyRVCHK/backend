#ifndef ODYSSEY_POLYMORF_INTERSECTION_H
#define ODYSSEY_POLYMORF_INTERSECTION_H

#include <concepts>
#include <type_traits>

namespace odyssey {
    
    namespace concepts {
        
        namespace math {
            
            template <typename InfoT, typename T>
            concept isIntersectionInfo = requires(const InfoT info) {
                requires std::same_as<decltype(info.cnt), const int*>;
                requires std::same_as<decltype(info.t_type1), const T*>;
                requires std::same_as<decltype(info.t_type2), const T*>;
            };
            
            template <typename IntersectionT, typename Geom1T, typename Geom2T, typename T, int N>
            concept iIntersection2 = requires (
                IntersectionT intrsct,
                const Geom1T& g1,
                const Geom2T& g2
            ) {
                requires std::same_as<decltype(intrsct.cnt), int>;

                { intrsct(g1, g2) } -> std::same_as<void>;
                { intrsct(g2, g1) } -> std::same_as<void>;

                { intrsct.get_info() } -> isIntersectionInfo<T>;

                { IntersectionT::intrsct(g1, g2) } -> std::same_as<int>;
                { IntersectionT::intrsct(g2, g1) } -> std::same_as<int>;
            } && std::default_initializable<IntersectionT>;
        }
    }
}

#endif /* ODYSSEY_POLYMORF_INTERSECTION_H */
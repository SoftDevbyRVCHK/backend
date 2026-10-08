#ifndef ODYSSEY_POLYMORF_IELLIPSE2_H
#define ODYSSEY_POLYMORF_IELLIPSE2_H

#include <concepts>
#include <type_traits>

#include "iVector2.h"

namespace odyssey {
    
    namespace concepts {
        
        namespace math {
            
            template <typename Ellipse2T>
            concept iEllipse2 = requires ( // требования к эллипсу использующемся в данном ПО.
                const Ellipse2T celps, 
                Ellipse2T elps,
                std::remove_cvref_t<decltype(std::declval<Ellipse2T>().pos)> vectorT,
                std::remove_cvref_t<decltype(std::declval<Ellipse2T>().pos.x())> valT
            ) {
                requires iVector2 <decltype(vectorT)>;
                
                { celps.getFromR(vectorT) } -> std::same_as <decltype(vectorT)>;
                { celps.getFromT(valT) }    -> std::same_as <decltype(vectorT)>;
                
                requires std::same_as <decltype((celps.pos)), const decltype(vectorT)&>;
                requires std::same_as <decltype((celps.i)),   const decltype(vectorT)&>;
                requires std::same_as <decltype((celps.j)),   const decltype(vectorT)&>;
                
                requires std::same_as <decltype((elps.pos)),  decltype(vectorT)&>;
                requires std::same_as <decltype((elps.i)),    decltype(vectorT)&>;
                requires std::same_as <decltype((elps.j)),    decltype(vectorT)&>;
            }
            && std::regular<Ellipse2T>
            && std::constructible_from<Ellipse2T, 
                const std::remove_cvref_t<decltype(std::declval<Ellipse2T>().pos)>&,
                const std::remove_cvref_t<decltype(std::declval<Ellipse2T>().pos)>&, 
                const std::remove_cvref_t<decltype(std::declval<Ellipse2T>().pos)>&>;
        }
    }
}

#endif /* ODYSSEY_POLYMORF_IELLIPSE2_H */
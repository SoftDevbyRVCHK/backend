#ifndef ODYSSEY_POLYMORF_ILINE2_H
#define ODYSSEY_POLYMORF_ILINE2_H

#include <concepts>
#include <type_traits>

#include "iVector2.h"

namespace odyssey {
    
    namespace concepts {
        
        namespace math {

            template <typename Line2T>
            concept iLine2 = requires ( //  требования к линии использующейся в данном ПО.
                const Line2T cln, 
                Line2T ln,
                std::remove_cvref_t<decltype(std::declval<Line2T>().pos)> vectorT,
                std::remove_cvref_t<decltype(std::declval<Line2T>().pos.x())> valT
            ) {
                requires iVector2 <decltype(vectorT)>;
                
                { cln.getFromT(valT) } -> std::same_as <decltype(vectorT)>;
                
                requires std::same_as <decltype((cln.pos)), const decltype(vectorT)&>;
                requires std::same_as <decltype((cln.dir)), const decltype(vectorT)&>;
                
                requires std::same_as <decltype((ln.pos)),  decltype(vectorT)&>;
                requires std::same_as <decltype((ln.dir)),  decltype(vectorT)&>;
            }
            && std::regular<Line2T>
            && std::constructible_from<Line2T, 
                const std::remove_cvref_t<decltype(std::declval<Line2T>().pos)>&,
                const std::remove_cvref_t<decltype(std::declval<Line2T>().pos)>&>;
        }
    }
}

#endif /* ODYSSEY_POLYMORF_ILINE2_H */
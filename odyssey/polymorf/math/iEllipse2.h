#ifndef ODYSSEY_POLYMORF_IELLIPSE2_H
#define ODYSSEY_POLYMORF_IELLIPSE2_H

#include <concepts>

#include "iVector2.h"

namespace odyssey {
    
    namespace polymorf {
        
        namespace math {
            
            /*
                требования к эллипсу использующемся в данном ПО.
            */
            
            template <typename dim2EllipseT>
            concept iEllipse2 = requires (
                const dim2EllipseT celps, 
                dim2EllipseT elps, 
                dim2EllipseT::valueT valT
            ) {
                requires iVector2 <typename dim2EllipseT::valueT>;
                
                { celps.getFromR(valT) } -> std::same_as <typename dim2EllipseT::valueT>;
                
                requires std::same_as <decltype((celps.pos)), const typename dim2EllipseT::valueT&>;
                requires std::same_as <decltype((celps.i)),   const typename dim2EllipseT::valueT&>;
                requires std::same_as <decltype((celps.j)),   const typename dim2EllipseT::valueT&>;
                
                requires std::same_as <decltype((elps.pos)),  typename dim2EllipseT::valueT&>;
                requires std::same_as <decltype((elps.i)),    typename dim2EllipseT::valueT&>;
                requires std::same_as <decltype((elps.j)),    typename dim2EllipseT::valueT&>;
            }
            && std::regular<dim2EllipseT>
            && std::constructible_from<dim2EllipseT, typename dim2EllipseT::valueT, typename dim2EllipseT::valueT, typename dim2EllipseT::valueT>;
        }
    }
}

#endif /* ODYSSEY_POLYMORF_IELLIPSE2_H */
#ifndef ODYSSEY_POLYMORF_ILINE2_H
#define ODYSSEY_POLYMORF_ILINE2_H

#include <concepts>

#include "iVector2.h"

namespace odyssey {
    
    namespace polymorf {
        
        namespace math {
            
            /*
                требования к линии использующейся в данном ПО.
            */
            
            template <typename dim2LineT>
            concept iLine2 = requires (
                const dim2LineT cln, 
                dim2LineT ln, 
                dim2LineT::valueT valT
            ) {
                requires iVector2 <typename dim2LineT::valueT>;
                
                { cln.getFromT(valT) } -> std::same_as <typename dim2LineT::valueT>;
                
                requires std::same_as <decltype((cln.pos)), const typename dim2LineT::valueT&>;
                requires std::same_as <decltype((cln.dir)), const typename dim2LineT::valueT&>;
                
                requires std::same_as <decltype((ln.pos)),  typename dim2LineT::valueT&>;
                requires std::same_as <decltype((ln.dir)),  typename dim2LineT::valueT&>;
            }
            && std::regular<dim2LineT>
            && std::constructible_from<dim2LineT, typename dim2LineT::valueT, typename dim2LineT::valueT>;
        }
    }
}

#endif /* ODYSSEY_POLYMORF_ILINE2_H */
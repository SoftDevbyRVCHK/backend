#ifndef ODYSSEY_POLYMORF_IVECTOR2_H
#define ODYSSEY_POLYMORF_IVECTOR2_H

#include <concepts>

namespace odyssey {
    
    namespace polymorf {
        
        namespace math {
            
            
            /*
                требование к вектору использующемся в данном ПО.
            */
            
            template <typename dim2VectorT>
            concept iVector2 = requires (
                dim2VectorT &v, 
                const dim2VectorT &cv1, 
                const dim2VectorT &cv2, 
                typename dim2VectorT::valueT valT
            ) {
                { v.x() } -> std::same_as <typename dim2VectorT::valueT&>;
                { v.y() } -> std::same_as <typename dim2VectorT::valueT&>;
                
                { cv1.x() } -> std::same_as <const typename dim2VectorT::valueT&>;
                { cv1.y() } -> std::same_as <const typename dim2VectorT::valueT&>;
                
                { cv1 + cv2  } -> std::same_as<dim2VectorT>;
                { cv1 - cv2  } -> std::same_as<dim2VectorT>;
                
                { cv1 * valT } -> std::same_as<dim2VectorT>;
                { valT * cv2 } -> std::same_as<dim2VectorT>;
                { cv1 / valT } -> std::same_as<dim2VectorT>;
                
                { cv1 * cv2  } -> std::same_as<typename dim2VectorT::valueT>;
                { cv1 ^ cv2  } -> std::same_as<typename dim2VectorT::valueT>;
                
                { cv1.spin(cv2) } -> std::same_as<dim2VectorT>;
                { cv1.length()  } -> std::same_as<typename dim2VectorT::valueT>;
                
                { v += cv1 }  -> std::same_as<dim2VectorT&>;
                { v -= cv1 }  -> std::same_as<dim2VectorT&>;
                { v *= valT } -> std::same_as<dim2VectorT&>;
                { v /= valT } -> std::same_as<dim2VectorT&>;
            }
            && std::convertible_to<int, typename dim2VectorT::valueT>
            && std::regular<dim2VectorT>
            && std::constructible_from<dim2VectorT, typename dim2VectorT::valueT, typename dim2VectorT::valueT>
            && alT<typename dim2VectorT::valueT>;
        }
    }
}

#endif /* ODYSSEY_POLYMORF_IVECTOR2_H */
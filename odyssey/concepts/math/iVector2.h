#ifndef ODYSSEY_POLYMORF_IVECTOR2_H
#define ODYSSEY_POLYMORF_IVECTOR2_H

#include <concepts>
#include <type_traits>

namespace odyssey {
    
    namespace concepts {
        
        namespace math {
            
            template <typename VectorT>
            using VectorScalarT = std::remove_cvref_t<decltype(std::declval<VectorT>().x())>;
            
            template <typename Vector2T>
            concept iVector2 = requires ( // требование к вектору использующемся в данном ПО.
                Vector2T &v, 
                const Vector2T &cv1, 
                const Vector2T &cv2,
                VectorScalarT<Vector2T> valT
            ) {
                { v.x() } -> std::same_as <decltype(valT)&>;
                { v.y() } -> std::same_as <decltype(valT)&>;
                
                { cv1.x() } -> std::same_as <const decltype(valT)&>;
                { cv1.y() } -> std::same_as <const decltype(valT)&>;
                
                { cv1 + cv2  } -> std::same_as<Vector2T>;
                { cv1 - cv2  } -> std::same_as<Vector2T>;
                
                { cv1 * valT } -> std::same_as<Vector2T>;
                { valT * cv2 } -> std::same_as<Vector2T>;
                { cv1 / valT } -> std::same_as<Vector2T>;
                
                { cv1 * cv2  } -> std::same_as<decltype(valT)>;
                { cv1 ^ cv2  } -> std::same_as<decltype(valT)>;
                
                { cv1.spin(cv2) } -> std::same_as<Vector2T>;
                { v.spin(cv2) } -> std::same_as<Vector2T&>;
                { cv1.length()  } -> std::same_as<decltype(valT)>;
                
                { v += cv1 }  -> std::same_as<Vector2T&>;
                { v -= cv1 }  -> std::same_as<Vector2T&>;
                { v *= valT } -> std::same_as<Vector2T&>;
                { v /= valT } -> std::same_as<Vector2T&>;
                
                requires std::regular<Vector2T>;
                requires std::constructible_from<Vector2T, const decltype(valT)&, const decltype(valT)&>;
                requires iArithmeticT<decltype(valT)>;
            };
        }
    }
}

#endif /* ODYSSEY_POLYMORF_IVECTOR2_H */
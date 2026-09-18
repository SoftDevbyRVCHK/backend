#ifndef ODYSSEY_POLYMORF_MATH_H
#define ODYSSEY_POLYMORF_MATH_H

namespace odyssey {
    
    namespace polymorf {
        
        namespace math {
            
            /*
                математические требования к объектам.
            */
            
            /*
                требование к арифметическому типу с возможностью сравнения.
            */
            
            template <typename T>
            concept alT = requires(T a, T b) {
                { a + b } -> std::same_as<T>;
                { a - b } -> std::same_as<T>;
                { a * b } -> std::same_as<T>;
                { a / b } -> std::same_as<T>;
            } 
            && std::regular<T> 
            && (std::three_way_comparable<T> || requires(T a, T b) {
                { a <=> b } -> std::same_as<bool>;
            });
        }
    }
}

#include "math/iVector2.h"
#include "math/iLine2.h"
#include "math/iEllipse2.h"

#endif /* ODYSSEY_POLYMORF_MATH_H */ 
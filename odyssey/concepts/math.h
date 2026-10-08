#ifndef ODYSSEY_POLYMORF_MATH_H
#define ODYSSEY_POLYMORF_MATH_H

#include <concepts>
#include <type_traits>

namespace odyssey {
    
    namespace concepts {
        
        namespace math {
            
            //
            //  математические требования к объектам.
            //
            
            template <typename T>
            concept iArithmeticT = requires(T a, T b) { // требование к арифметическому типу с возможностью сравнения.
                { a + b } -> std::same_as<T>;
                { a - b } -> std::same_as<T>;
                { a * b } -> std::same_as<T>;
                { a / b } -> std::same_as<T>;
                
                { a > b  } -> std::same_as<bool>;
                { a >= b } -> std::same_as<bool>;
                { a < b  } -> std::same_as<bool>;
                { a <= b } -> std::same_as<bool>;
                { a == b } -> std::same_as<bool>;
                { a != b } -> std::same_as<bool>;
            } 
            && std::regular<T>
            && std::convertible_to<double, T>
            && std::convertible_to<T, double>;
        }
    }
}

#include "math/iVector2.h"
#include "math/iLine2.h"
#include "math/iEllipse2.h"

#endif /* ODYSSEY_POLYMORF_MATH_H */ 
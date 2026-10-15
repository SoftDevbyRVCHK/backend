#ifndef ODYSSEY_UTILITY_MATH_H
#define ODYSSEY_UTILITY_MATH_H

#include <cmath>

namespace odyssey {
    
    namespace utility {
        
        namespace math {
            
            template <typename value_t>
            constexpr value_t eps {1e-7};
            
            template <typename value_t>
            inline bool is_zero (const value_t &v) {
                return std::abs(v) < eps<value_t>;
            }
        }
    }
}

#include "math/polynom.h"
#include "math/intersection.h"
#include "math/dimension2.h"

#include "math/graph.h"

#endif /* ODYSSEY_UTILITY_MATH_H */
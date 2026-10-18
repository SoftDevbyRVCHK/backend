#ifndef ODYSSEY_UTILITY_MATH_DIMENSION2_ELLIPSE2_H
#define ODYSSEY_UTILITY_MATH_DIMENSION2_ELLIPSE2_H

#include "vector.h"

#include <cmath>

namespace odyssey {
    
    namespace utility {
        
        namespace math {
            
            namespace dimension2 {
                
                template <typename valueT = float>
                struct ellipse {
                
                    vector<valueT> i;
                    vector<valueT> j;
                    vector<valueT> pos;
                    
                                   ellipse  () = default;
                                   ellipse  (const vector<valueT> &i, const vector<valueT> &j, const vector<valueT> &pos)       : i(i), j(j), pos(pos){}
                           
                    vector<valueT> getFromT (const valueT &t)                                                             const {return i*std::cos(t) + j*std::sin(t) + pos;}
                    vector<valueT> getFromR (const vector<valueT> &r)                                                     const {return i*r.x() + j*r.y() + pos;}
                };
            }
        }
    }
}

#endif /* ODYSSEY_UTILITY_MATH_DIMENSION2_ELLIPSE2_H */
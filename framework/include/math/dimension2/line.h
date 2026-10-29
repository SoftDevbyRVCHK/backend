#ifndef ODYSSEY_FRAMEWORK_MATH_DIMENSION2_LINE2_H
#define ODYSSEY_FRAMEWORK_MATH_DIMENSION2_LINE2_H

#include "vector.h"

namespace odyssey {
    
    namespace framework {
        
        namespace math {
            
            namespace dimension2 {
                
                template <typename valueT = float>
                struct line {
                
                    vector<valueT> dir;
                    vector<valueT> pos;
                    
                                   line     () = default;
                                   line     (const vector<valueT> &pt1, const vector<valueT> &pt2)       : dir(pt2 - pt1), pos(pt1){}

                    vector<valueT> getFromX (const valueT &x)                                      const {return vector<valueT>(x, dir.y()*(x - pos.x())/dir.x() + pos.y());}
                    vector<valueT> getFromY (const valueT &y)                                      const {return vector<valueT>(dir.x()*(y - pos.y())/dir.y() + pos.x(), y);}
                    vector<valueT> getFromT (const valueT &t)                                      const {return dir*t + pos;}
                };
            }
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_MATH_DIMENSION2_LINE2_H*/
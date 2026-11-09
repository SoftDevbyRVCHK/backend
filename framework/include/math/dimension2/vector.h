#ifndef ODYSSEY_FRAMEWORK_MATH_DIMENSION2_VECTOR2_H
#define ODYSSEY_FRAMEWORK_MATH_DIMENSION2_VECTOR2_H

#include <cmath>

namespace odyssey {
    
    namespace framework {
        
        namespace math {
            
            namespace dimension2 {
                
                template <typename valueT = float>
                struct vector {
                
                    valueT         coord[2];

                                   vector      () = default;
                                   vector      (valueT x, valueT y)                           : coord{x, y}{}
                                          
                    vector<valueT> operator-   ()                                       const {return vector(-x(), -y());}
                    vector<valueT> operator!   ()                                       const {return vector(-y(), x());}
                    
                    vector<valueT> operator+   (const vector &v)                        const {return vector(x() + v.x(), y() + v.y());}
                    vector<valueT> operator-   (const vector &v)                        const {return vector(x() - v.x(), y() - v.y());}
                    vector<valueT> operator*   (valueT s)                               const {return vector(s*x(), s*y());}
                    vector<valueT> operator/   (valueT s)                               const {return this->operator*(c_1 / s);}
                    valueT         operator*   (const vector &v)                        const {return x()*v.x() + y()*v.y();}
                    valueT         operator^   (const vector &v)                        const {return x()*v.y() - y()*v.x();}
                    
                    vector<valueT> operator+=  (const vector &v)                              {x()+= v.x(); y()+= v.y(); return *this;}
                    vector<valueT> operator-=  (const vector &v)                              {x()-= v.x(); y()-= v.y(); return *this;}
                    vector<valueT> operator*=  (const valueT &s)                              {x()*= s;     y()*= s;     return *this;}
                    vector<valueT> operator/=  (valueT s)                                     {s = c_1 / s; x()*= s; y()*= s; return *this;}
                    
                    vector<valueT> spin        (const vector &r)                        const {return (*this)*r.x() + (!*this)*r.y();}
                    vector<valueT> spin        (const vector &r)                              {return *this = static_cast<const vector<valueT>&>(*this).spin(r);}
                    valueT         length      ()                                       const {return std::sqrt( x()*x() + y()*y() );}
                    vector<valueT> normal      ()                                       const {return *this * c_1 / length();}
                    vector<valueT> toBase      (const vector &e1, const vector &e2)     const {
                                                                                                valueT d = c_1 / determinant(e1, e2);
                                                                                                return vector(determinant(*this, e2) * d, determinant(e1, *this) * d);
                                                                                              }
                    vector<valueT> toBase      (const vector &e1, const vector &e2)           {return *this = static_cast<const vector<valueT>&>(*this).toBase(e1, e2);}
                    
                    valueT&        first       ()                                             {return coord[0];}
                    valueT&        second      ()                                             {return coord[1];}
                    const valueT&  first       ()                                       const {return coord[0];}
                    const valueT&  second      ()                                       const {return coord[1];}
                    
                    valueT&        x           ()                                             {return first();}
                    valueT&        y           ()                                             {return second();}
                    const valueT&  x           ()                                       const {return first();}
                    const valueT&  y           ()                                       const {return second();}
                    
                    static valueT  determinant 
                           (const vector<valueT> &col1, const vector<valueT> &col2)          {return col1^col2;}
                    static constexpr valueT c_1 {1.0};
                };
                template <typename valueT>
                inline vector<valueT> operator* (valueT s, const vector<valueT> &v) {return v*s;}
            }
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_MATH_DIMENSION2_VECTOR2_H */
#ifndef ODYSSEY_UTILITY_MATH_INTERSECTION_H
#define ODYSSEY_UTILITY_MATH_INTERSECTION_H

#include "dimension2/vector.h"
#include "dimension2/line.h"
#include "dimension2/ellipse.h"

#include "polynom.h"

#include <cmath>

namespace odyssey {
    
    namespace utility {

        namespace math {
            
            template <typename value_t>
            struct intersection_info {
                    
                const int *cnt;
                const value_t *t_type1;
                const value_t *t_type2;
                
                intersection_info (const int *count, const value_t *t_t1, const value_t *t_t2) : cnt(count), t_type1(t_t1), t_type2(t_t2) {}
            };
            
            template<template <typename> typename T1, template <typename> typename T2, typename value_t = float>
            struct intersection;
            
            template <typename value_t> 
            struct intersection<dimension2::vector, dimension2::vector, value_t> {
                
                using Type1 = dimension2::vector<value_t>;
                using Type2 = dimension2::vector<value_t>;
                
                intersection () : cnt(0) {}
                
                void operator() (const Type1 &pt1, const Type2 &pt2) {
                    Type1 t {pt1 - pt2};
                    if (is_zero<value_t>(t*t))
                        cnt = -1; // overlap
                    else
                        cnt = 0;  // separate
                }
                static int intrsct (const Type1 &pt1, const Type2 &pt2) {
                    Type1 t {pt1 - pt2};
                    if (is_zero<value_t>(t*t))
                        return -1; // overlap
                    else
                        return 0;  // separate
                }
                intersection_info<value_t> get_info () {
                    return intersection_info<value_t>(&cnt, nullptr, nullptr);
                }
                int cnt;
            };
            
            template <typename value_t>
            struct intersection<dimension2::vector, dimension2::line, value_t> {
                
                using Type1 = dimension2::vector<value_t>;
                using Type2 = dimension2::line<value_t>;
                            
                intersection () : cnt(0) {}
                
                void operator() (const Type1 &pt, const Type2 &ln) {
                    Type1 t {pt - ln.pos};
                    if (is_zero<value_t>(t^ln.dir)) {
                        t_type2[0] = (t * ln.dir) / (ln.dir * ln.dir);

                        cnt = 1; // intersect
                    }
                    else {
                        cnt = 0; // separate
                    }
                }
                void operator() (const Type2 &ln, const Type1 &pt) {
                    operator()(pt, ln);
                }
                       
                static int intrsct (const Type1 &pt, const Type2 &ln) {
                    Type1 t(pt - ln.pos);
                    if (is_zero<value_t>(t^ln.dir))
                        return 1; // intersect
                    else
                        return 0; // separate
                }
                static int intrsct (const Type2 &ln, const Type1 &pt) {
                    return intrsct(pt, ln);
                }            
                intersection_info<value_t> get_info () {
                    return intersection_info<value_t>(&cnt, nullptr, t_type2);
                }
                int cnt;
                value_t t_type2[1];
            };
            
            template <typename value_t>
            struct intersection<dimension2::vector, dimension2::ellipse, value_t> {
                
                using Type1 = dimension2::vector<value_t>;
                using Type2 = dimension2::ellipse<value_t>;
                
                intersection () : cnt(0) {}
                
                void operator() (const Type1 &pt, const Type2 &elps) {
                    Type1 t {pt - elps.pos};
                    value_t n {elps.i^elps.j};
                    t = Type1(elps.i^t, elps.j^t);

                    if (is_zero<value_t>(t*t - n*n)) {
                        n = std::copysign(1.0f, n);
                        t_type2[0] = std::atan2(n*t.first(), -n*t.second());

                        cnt = 1; // intersect
                    }
                    else {
                        cnt = 0; // separate
                    }
                }
                void operator() (const Type2 &elps, const Type1 &pt) {
                    operator()(pt, elps);
                }
                static int intrsct (const Type1 &pt, const Type2 &elps) {
                    Type1 t(pt - elps.pos);
                    value_t n = elps.i^elps.j;
                    t = Type1(elps.i^t, elps.j^t);

                    if (is_zero<value_t>(t*t - n*n))
                        return 1; // intersect
                    else
                        return 0; // separate
                }
                static int intrsct (const Type2 &elps, const Type1 &pt) {
                    return intrsct(pt, elps);
                }
                intersection_info<value_t> get_info () {
                    return intersection_info<value_t>(&cnt, nullptr, t_type2);
                }
                int cnt;
                value_t t_type2[1];
            };

            
            template <typename value_t>
            struct intersection<dimension2::line, dimension2::line, value_t> {
                
                using Type1 = dimension2::line<value_t>;
                using Type2 = dimension2::line<value_t>;
                
                intersection () : cnt(0) {}
                
                void operator() (const Type1 &ln1, const Type2 &ln2) {
                    value_t n {ln2.dir^ln1.dir};

                    if (is_zero<value_t>(n)) {
                        switch(intersection<dimension2::vector, dimension2::line, value_t>::intrsct(ln2.pos, ln1)) {
                            case 1: 
                                cnt = -1; // overlap
                                break;
                            case 0:
                                cnt = 0;  // separate
                                break;
                        }
                    }
                    else {
                        dimension2::vector<value_t> dp {ln1.pos - ln2.pos};
                        
                        n = value_t(1.0) / n;
                        
                        t_type1[0] = (dp^ln2.dir) * n;
                        t_type2[0] = (dp^ln1.dir) * n;
                        cnt = 1; // intersect
                    }
                }
                static int intrsct (const Type1 &ln1, const Type2 &ln2) {
                    value_t n {ln2.dir^ln1.dir};

                    if (is_zero<value_t>(n)){
                        switch(intersection<dimension2::vector, dimension2::line, value_t>::intrsct(ln2.pos, ln1)) {
                            case 1: return -1; // overlap
                            case 0: return 0;  // separate
                        }
                    }
                    else
                        return 1; // intersect
                }
                intersection_info<value_t> get_info () {
                    return intersection_info<value_t>(&cnt, t_type1, t_type2);
                }
                int cnt;
                value_t t_type1[1];
                value_t t_type2[1];
            };
            
            template <typename value_t>
            struct intersection<dimension2::line, dimension2::ellipse, value_t> {
                
                using Type1 = dimension2::line<value_t>;
                using Type2 = dimension2::ellipse<value_t>;
                using Vector = dimension2::vector<value_t>;
                
                intersection () : cnt(0) {}
                
                void operator() (const Type1 &ln, const Type2 &elps) {
                    cnt = 0;
                    Vector a {elps.i^ln.dir, elps.j^ln.dir};
                    Vector b {ln.pos - elps.pos};
                    value_t n {elps.i^elps.j};

                    b = Vector(elps.i^b, elps.j^b);

                    std::complex<value_t> x[2];
                    int i;
                    int t_t1count = polynom<2, value_t>::roots(
                        a*a,
                        2*(a*b), 
                        b*b - n*n, 
                        x
                    );
                    n = std::copysign(c_1, n);

                    for (i = 0; i != t_t1count; i++)
                        if (is_zero<value_t>(x[i].imag())) {
                            t_type1[cnt] = x[i].real();
                            t_type2[cnt] = std::atan2(n*(a.first()*t_type1[cnt] + b.first()), -n*(a.second()*t_type1[cnt] + b.second()));
                            cnt++;
                        }
                    // intersect / separate
                }
                void operator() (const Type2 &elps, const Type1 &ln) {
                    operator()(ln, elps);
                }
                static int intrsct (const Type1 &ln, const Type2 &elps) {
                    Vector a {elps.i^ln.dir, elps.j^ln.dir};
                    Vector b {ln.pos - elps.pos};
                    value_t n {elps.i^elps.j};

                    b = Vector(elps.i^b, elps.j^b);

                    std::complex<value_t> x[2];
                    int i, cnt;
                    int t_t1count = polynom<2, value_t>::roots(
                        a*a,
                        2*(a*b),
                        b*b - n*n,
                        x
                    );
                    
                    cnt = 0;
                    for (i = 0; i != t_t1count; i++)
                        if (is_zero<value_t>(x[i].imag()))
                            cnt++;
                    
                    return cnt; // intersect / separate
                }
                static int intrsct (const Type2 &elps, const Type1 &ln) {
                    return intrsct(ln, elps);
                }
                intersection_info<value_t> get_info () {
                    return intersection_info<value_t>(&cnt, t_type1, t_type2);
                }
                int cnt;
                value_t t_type1[2];
                value_t t_type2[2];
            
            private:    
                static constexpr value_t c_1 {1.0};
            };
            
            template <typename value_t>
            struct intersection<dimension2::ellipse, dimension2::ellipse, value_t> {
                
                using Type1 = dimension2::ellipse<value_t>;
                using Type2 = dimension2::ellipse<value_t>;
                using Vector = dimension2::vector<value_t>;
                
                intersection () : cnt(0) {}
                
                void operator() (const Type1 &elps1, const Type2 &elps2) {
                    value_t el1_det {elps1.i^elps1.j};
                    if (is_zero(el1_det)) { cnt = 0; return; }
                    
                    Vector vx {(elps2.i^elps1.j) / el1_det, (elps1.i^elps2.i) / el1_det};
                    Vector vy {(elps2.j^elps1.j) / el1_det, (elps1.i^elps2.j) / el1_det};
                    Vector p  {(elps2.pos - elps1.pos) / el1_det};
                    p = Vector(p^elps1.j, elps1.i^p);
                    
                    value_t vx_sq {vx*vx};
                    value_t vy_sq {vy*vy};
                    
                    value_t a {vx_sq - vy_sq};
                    value_t b {c_2 * (vx*p)};
                    value_t c {p*p + vy_sq - c_1};
                    value_t d {c_2 * (vx*vy)};
                    value_t e {c_2 * (vy*p)};
                    
                    std::complex<value_t> x[4];
                    int t2_cnt = polynom<4, value_t>::roots(
                        a*a + d*d,
                        c_2*(a*b + d*e),
                        (c_2*a*c + b*b - d*d + e*e),
                        c_2*(b*c - d*e),
                        c*c - e*e,
                        x
                    );
                    
                    if (t2_cnt == -1) {
                        cnt = -1;
                        return;
                    }
                    cnt = 0;
                    
                    Vector dp = elps2.pos - elps1.pos; 
                    Vector e2, dir;
                    
                    int i;
                    for (i = 0; i < t2_cnt; i++)
                        if (is_zero(x[i].imag())) {
                            e2.x() = x[i].real();
                            
                            e2.y() = d * e2.x() + e;
                            if (is_zero(e2.y())) {
                                value_t clamp_x = std::clamp(e2.x(), -c_1, c_1);
                                e2.y() = std::sqrt(c_1 - clamp_x * clamp_x);
                            } else {
                                e2.y() = -(a*e2.x()*e2.x() + b*e2.x() + c) / e2.y();
                            }
                            
                            dir = elps2.i*e2.x() + elps2.j*e2.y() + dp;
                            
                            t_type1[cnt] = std::atan2((elps1.i^dir), (dir^elps1.j));
                            t_type2[cnt] = std::atan2(e2.y(), e2.x());
                            
                            cnt++;
                        }
                }

                static int intrsct (const Type1 &elps1, const Type2 &elps2) {
                    return 0;
                }
                intersection_info<value_t> get_info () {
                    return intersection_info<value_t>(&cnt, t_type1, t_type2);
                }
                int cnt;
                value_t t_type1[4];
                value_t t_type2[4];
            
            private:    
                static inline constexpr value_t c_0   {0.0};
                static inline constexpr value_t c_0p5 {0.5};
                static inline constexpr value_t c_1   {1.0};
                static inline constexpr value_t c_2   {2.0};
                static inline constexpr value_t c_4   {4.0};
            };
        }
    }
}

#endif /* ODYSSEY_UTILITY_MATH_INTERSECTION_H */
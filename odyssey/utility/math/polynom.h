#ifndef ODYSSEY_UTILITY_MATH_POLYNOM_H
#define ODYSSEY_UTILITY_MATH_POLYNOM_H

#include <cmath>
#include <complex>

namespace odyssey {
    
    namespace utility {
        
        namespace math {
            
            template<typename value_t>        
            inline bool is_zero (const std::complex<value_t> &v) {
                return is_zero<value_t>(v.real()) && is_zero<value_t>(v.imag());
            }
            
            template<int N, typename value_t>
            struct polynom;
            
            template <typename value_t>
            struct polynom<0, value_t> {
                static int roots 
                    (const std::complex<value_t> &a)
                    {
                        return is_zero<value_t>(a)? -1 : 0;
                    }
                 
                static int roots
                    ()
                    {
                        return -1;
                    }
            };
            
            template <typename value_t>
            struct polynom<1, value_t> {
                static int roots
                   (const std::complex<value_t> &a,
                    const std::complex<value_t> &b,
                    std::complex<value_t> x[1])
                    {
                        return is_zero<value_t>(a) ? 
                               polynom<0, value_t>::roots(b) :
                               roots(b/a, x);
                    }
                 
                static int roots
                   (const std::complex<value_t> &b,
                    std::complex<value_t> x[1])
                    {
                        x[0] = -b;
                        
                        return 1;
                    }
            };
            
            template <typename value_t>
            struct polynom<2, value_t> {
                static int roots
                   (const std::complex<value_t> &a,
                    const std::complex<value_t> &b,
                    const std::complex<value_t> &c,
                    std::complex<value_t> x[2])
                    {
                        return is_zero<value_t>(a) ?
                               polynom<1, value_t>::roots(b, c, x) :
                               roots(b/a, c/a, x);
                    }
            
                static int roots
                   (const std::complex<value_t> &b,
                    const std::complex<value_t> &c,
                    std::complex<value_t> x[2])
                {
                    std::complex<value_t> d = std::sqrt(b*b - c_4 * c);
                    
                    x[0] = c_0p5 * (d - b);
                    x[1] = c_0p5 * (-d - b);
                    
                    return is_zero<value_t>(d) ? 1 : 2;
                }
                
            private:
                static inline constexpr value_t c_0p5 {0.5};
                static inline constexpr value_t c_4   {4.0};
            };

            template <typename value_t>
            struct polynom<3, value_t> {
                static int roots
                   (const std::complex<value_t> &a,
                    const std::complex<value_t> &b,
                    const std::complex<value_t> &c,
                    const std::complex<value_t> &d,
                    std::complex<value_t> x[3])
                    {
                        return is_zero<value_t>(a) ?
                               polynom<2, value_t>::roots(b, c, d, x) :
                               roots(b/a * c_inv3, c/a * c_inv3, d/a, x);
                    }
           
                static int roots
                   (const std::complex<value_t> &b,
                    const std::complex<value_t> &c,
                    const std::complex<value_t> &d,
                    std::complex<value_t> x[3])
                    {
                        std::complex<value_t> b_sq {b*b};
                        std::complex<value_t> p {c - b_sq}, q {b_sq*b - c_1p5 * b*c + c_0p5 * d};
                        std::complex<value_t> Q {std::sqrt(p*p*p + q*q)};
                        
                        std::complex<value_t> u {std::pow(-q + Q, c_inv3)}, v {is_zero<value_t>(u)? std::pow(-q - Q, c_inv3) : -p/u};
                        
                        x[0] = u + v - b;
                        x[1] = u*w1 + v*w2 - b;
                        x[2] = u*w2 + v*w1 - b;
                        
                        int i, j, cnt = 1;
                        bool unique;
                        for (i = 1; i < 3; i++) {
                            unique = true;
                            for (j = 0; j < cnt; j++) {
                                if (is_zero<value_t>(x[i] - x[j])) {
                                    unique = false;
                                    break;
                                }
                            }
                            if (unique)
                                x[cnt++] = x[i];
                        }

                        return cnt;
                    }
            private:         
                static inline constexpr value_t c_inv3       {value_t(1.0) / value_t(3.0)};
                static inline constexpr value_t c_0p5        {0.5};
                static inline constexpr value_t c_1p5        {1.5};
                static inline constexpr value_t c_sqrt3      {std::sqrt(value_t(3.0))}; // 0.86602540378443864676
                static inline constexpr std::complex<value_t> w1 {-c_0p5, c_sqrt3 * c_0p5};  // -0.5 + i*0.5*sqrt(3);
                static inline constexpr std::complex<value_t> w2 {-c_0p5, -c_sqrt3 * c_0p5}; // -0.5 - i*0.5*sqrt(3);
            };
            
            template <typename value_t>
            struct polynom<4, value_t> {
                static int roots
                   (const std::complex<value_t> &a,
                    const std::complex<value_t> &b,
                    const std::complex<value_t> &c,
                    const std::complex<value_t> &d,
                    const std::complex<value_t> &e,
                    std::complex<value_t> x[4])
                    {
                        return is_zero<value_t>(a) ? 
                               polynom<3, value_t>::roots(b, c, d, e, x) :
                               roots(b/a * c_inv4, c/a * c_inv6, d/a * c_inv4, e/a, x);
                    }
            
                static int roots
                   (const std::complex<value_t> &b,
                    const std::complex<value_t> &c,
                    const std::complex<value_t> &d,
                    const std::complex<value_t> &e,
                    std::complex<value_t> x[4])
                    {
                        
                        std::complex<value_t> b_sq {b*b};
                        std::complex<value_t> p {c_3*(c - b_sq)}, q {c_4*(b_sq*b - c_1p5*b*c + c_0p5*d)}, r {e + c_6*b_sq*c - c_4*b*d - c_3*b_sq*b_sq};
                        
                        std::complex<value_t> t[3];
                        
                        int i, j, cnt, maxCnt;
                        
                        if (is_zero<value_t>(q)) {
                            
                            t[0] = std::sqrt(p*p - r);
                            t[1] = std::sqrt(-p - t[0]);
                            t[2] = std::sqrt(-p + t[0]);
                            
                            x[0] =  t[1];
                            x[1] = -t[1];
                            x[2] =  t[2];
                            x[3] = -t[2];
                            
                            maxCnt = 4;
                        }
                        else {
                            
                            maxCnt = polynom<3, value_t>::roots(
                                c_2,
                                c_4*p,
                                c_2*(p*p - r),
                                -q*q,
                                t
                            );
                            
                            j = 0;
                            for (i = 1; i < maxCnt; i++)
                                if (std::norm(t[i]) > std::norm(t[j]))
                                    j = i;
                            
                            t[0] = t[j];
                            t[1] = std::sqrt(c_2*t[0]); // sqrt(2t)
                            t[2] = p + t[0];            // p + t
                            t[0] = q / t[1];            // q / sqrt(2t)
                            
                            maxCnt = polynom<2, value_t>::roots(
                                c_1,
                                -t[1],          // -sqrt(2t)
                                t[2] + t[0],    // p + t + q / sqrt(2t)
                                x
                            );
                            maxCnt+= polynom<2, value_t>::roots(
                                c_1,
                                t[1],           // sqrt(2t)
                                t[2] - t[0],    // p + t - q / sqrt(2t)
                                x + maxCnt
                            );
                        }
                        
                        cnt = 0;
                        bool unique;
                        for (i = 0; i != maxCnt; i++)
                            x[i]-= b;
                            
                        if (maxCnt > 0) {
                            cnt = 1;
                            for (i = 1; i < maxCnt; i++) {
                                unique = true;
                                for (j = 0; j < cnt; j++) {
                                    if (is_zero<value_t>(x[i] - x[j])) {
                                        unique = false;
                                        break;
                                    }
                                }
                                if (unique)
                                    x[cnt++] = x[i];
                            }
                        }
                        return cnt;
                    }
                    
            private:        
                static inline constexpr value_t c_inv6 {value_t(1.0) / value_t(6.0)};
                static inline constexpr value_t c_inv4 {value_t(1.0) / value_t(4.0)};
                static inline constexpr std::complex<value_t> c_0 {value_t{0.0}, value_t{0.0}};
                static inline constexpr value_t c_0p5  {0.5};
                static inline constexpr std::complex<value_t> c_1 {value_t{1.0}, value_t{0.0}};
                static inline constexpr value_t c_1p5  {1.5};
                static inline constexpr std::complex<value_t> c_2 {value_t{2.0}, value_t{0.0}};
                static inline constexpr value_t c_3    {3.0};
                static inline constexpr value_t c_4    {4.0};
                static inline constexpr value_t c_6    {6.0};
            };

        }
    }
}

#endif /* ODYSSEY_UTILITY_MATH_POLYNOM_H */
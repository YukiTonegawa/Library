#ifndef _FRACTION_H_
#define _FRACTION_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include <type_traits>
#include <iostream>
#include <string>

// -inf <= a <= inf
// 0 <= b <= inf
// inf^2がオーバーフローしない
template<typename T, T inf, std::enable_if_t<inf <= (std::numeric_limits<T>::max() / inf)>* = nullptr>
struct fraction {
    using self_t = fraction<T, inf>;
    
    T a, b;
    fraction(T _a = 0, T _b = 1): a(_a), b(_b) { 
        assert(-inf <= a && a <= inf && 0 <= b && b <= inf); 
    }

    // 既約分数
    self_t modify() const {
        T x = a, y = b;
        if (x < y) std::swap(x, y);
        while(y) {
            T tmp = x %= y;
            x = y, y = tmp;
        }
        if (x == 0) return self_t(0, 0); // 元が0/0だった場合
        return self_t(a / x, b / x);
    }

    bool operator == (const self_t &r) const { return a * r.b == b * r.a; }
    bool operator < (const self_t &r) const { return a * r.b < b * r.a; }
    bool operator <= (const self_t &r) const { return a * r.b <= b * r.a; }
    bool operator > (const self_t &r) const { return a * r.b > b * r.a; }
    bool operator >= (const self_t &r) const { return a * r.b >= b * r.a; }
    
    long double to_ld() const {
        assert(b != 0);
        return (long double)a / b;
    }
};

template<typename T, T inf>
std::ostream &operator << (std::ostream &dest, const fraction<T, inf> &v) {
    dest << "(" << v.a << ", " << v.b << ")";
    return dest;
}
#endif
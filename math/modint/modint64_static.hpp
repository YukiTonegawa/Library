#ifndef _MODINT64_STATIC_H_
#define _MODINT64_STATIC_H_
#include <cassert>
#include <type_traits>
#include <iostream>
#include "../number_theory/miller_rabin64_constexpr.hpp"
#include "../base/gcd.hpp"

template <long long m, std::enable_if_t<(1 <= m)>* = nullptr>
struct modint64_static {
    using mint = modint64_static;
  public:
    static constexpr long long mod() { return m; }
    
    static mint raw(long long v) {
        mint x;
        x._v = v;
        return x;
    }
  
    modint64_static(): _v(0) {}
    
    template <class T>
    modint64_static(T v) { 
        T x = v % umod();
        if (x < 0) x += umod();
        _v = x;
    }

    unsigned long long val() const { return _v; }
    
    mint& operator ++ () {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator -- () {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator ++ (int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator -- (int) {
        mint result = *this;
        --*this;
        return result;
    }
    mint& operator += (const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator -= (const mint& rhs) {
        _v -= rhs._v;
        if (_v >= umod()) _v += umod();
        return *this;
    }
    mint& operator *= (const mint& rhs) {
        __uint128_t z = _v;
        z *= rhs._v;
        _v = (unsigned long long)(z % umod());
        return *this;
    }
    mint& operator /= (const mint& rhs) { return *this = *this * rhs.inv(); }
    mint operator + () const { return *this; }
    mint operator-() const { return mint() - *this; }
    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        if (prime) {
            assert(_v);
            return pow(umod() - 2);
        } else {
            auto eg = inv_gcd(_v, m);
            assert(eg.first == 1);
            return eg.second;
        }
    }
    friend mint operator + (const mint& lhs, const mint& rhs) { return mint(lhs) += rhs; }
    friend mint operator - (const mint& lhs, const mint& rhs) { return mint(lhs) -= rhs; }
    friend mint operator * (const mint& lhs, const mint& rhs) { return mint(lhs) *= rhs; }
    friend mint operator / (const mint& lhs, const mint& rhs) { return mint(lhs) /= rhs; }
    friend bool operator == (const mint& lhs, const mint& rhs) { return lhs._v == rhs._v; }
    friend bool operator != (const mint& lhs, const mint& rhs) { return lhs._v != rhs._v; }
  private:
    unsigned long long _v;
    static constexpr unsigned long long umod() { return m; }
    static constexpr bool prime = miller_rabin64_constexpr<m>();
};

template<int m>
std::ostream &operator<<(std::ostream &dest, const modint64_static<m> &a) {
    dest << a.val();
    return dest;
}
#endif
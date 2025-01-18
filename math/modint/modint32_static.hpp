#ifndef _MODINT_H_
#define _MODINT_H_

#include <cassert>
#include <type_traits>
#include <iostream>
#include "../number_theory/miller_rabin32.hpp"
#include "../base/gcd.hpp"

template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct modint32_static {
    using mint = modint32_static;
  public:
    static constexpr int mod() { return m; }
    
    static mint raw(int v) {
        mint x;
        x._v = v;
        return x;
    }
  
    modint32_static(): _v(0) {}
    
    template <class T>
    modint32_static(T v) { 
        long long x = v % (long long)umod();
        if (x < 0) x += umod();
        _v = x;
    }

    unsigned int val() const { return _v; }
    
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
        unsigned long long z = _v;
        z *= rhs._v;
        _v = (unsigned int)(z % umod());
        return *this;
    }
    mint& operator /= (const mint& rhs) { return *this = *this * rhs.inv(); }
    mint operator + () const { return *this; }
    mint operator - () const { return mint() - *this; }
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
    unsigned int _v;
    static constexpr unsigned int umod() { return m; }
    static constexpr bool prime = miller_rabin32<m>;
};

template<int m>
std::ostream &operator << (std::ostream &dest, const modint32_static<m> &a) {
    dest << a.val();
    return dest;
}

template<int m>
std::istream &operator >> (std::istream &dest, modint32_static<m> &a) {
    long long x;
    std::cin >> x;
    a = x;
    return dest;
}

using modint998244353 = modint32_static<998244353>;
using modint1000000007 = modint32_static<1000000007>;
#endif
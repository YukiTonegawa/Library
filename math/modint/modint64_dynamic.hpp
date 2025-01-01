#ifndef _MODINT64_H_
#define _MODINT64_H_

#include <cassert>
#include "../base/gcd.hpp"

// @param mod < 2^63, modは素数
template<int id> 
struct modint64_dynamic {
    using mint = modint64_dynamic;
  
  public:  
    static long long mod() { return m; }
    
    static void set_mod(long long _m) {
        assert(1 <= _m && _m < (1ULL << 63));
        m = _m;
    }

    static mint raw(unsigned long long v) {
        mint x;
        x._v = v;
        return x;
    }

    modint64_dynamic(): _v(0){}
    
    template <class T>
    modint64_dynamic(T v) {
        long long x = v % (long long)(mod());
        if(x < 0) x += mod();
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
        _v = __uint128_t(_v) * rhs._v % umod();
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
        auto eg = inv_gcd(_v, m);
        assert(eg.first == 1);
        return eg.second;
    }

    friend mint operator +  (const mint& lhs, const mint& rhs) { return mint(lhs) += rhs; }
    friend mint operator -  (const mint& lhs, const mint& rhs) { return mint(lhs) -= rhs; }
    friend mint operator *  (const mint& lhs, const mint& rhs) { return mint(lhs) *= rhs; }
    friend mint operator /  (const mint& lhs, const mint& rhs) { return mint(lhs) /= rhs; }
    friend bool operator == (const mint& lhs, const mint& rhs) { return lhs._v == rhs._v; }
    friend bool operator != (const mint& lhs, const mint& rhs) { return lhs._v != rhs._v; }
  
  private:
    unsigned long long _v;
    static long long m;
    static unsigned long long umod() { return m; }
};

template <int id>
long long modint64_dynamic<id>::m(998244353);
#endif
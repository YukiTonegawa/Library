#ifndef _MODINT32_DYNAMIC_H_
#define _MODINT32_DYNAMIC_H_

#include <cassert>
#include "../base/fast_mul_mod/barrett.hpp"
#include "../base/gcd.hpp"

template<int id> 
struct modint32_dynamic {
    using mint = modint32_dynamic;
  public:
    
    static int mod() { return (int)(bt.umod()); }
    static void set_mod(int m) {
        assert(1 <= m);
        bt = barrett(m);
    }
    static mint raw(int v) {
        mint x;
        x._v = v;
        return x;
    }
  
    modint32_dynamic(): _v(0) {}
    
    template <class T>
    modint32_dynamic(T v) {
        long long x = v % (long long)(mod());
        if (x < 0) x += mod();
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
        _v += mod() - rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator *= (const mint& rhs) {
        _v = bt.mul(_v, rhs._v);
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
        auto eg = inv_gcd(_v, mod());
        assert(eg.first == 1);
        return eg.second;
    }
    friend mint operator + (const mint& lhs, const mint& rhs) { return mint(lhs) += rhs; }
    friend mint operator - (const mint& lhs, const mint& rhs) { return mint(lhs) -= rhs; }
    friend mint operator * (const mint& lhs, const mint& rhs) { return mint(lhs) *= rhs; }
    friend mint operator / (const mint& lhs, const mint& rhs) { return mint(lhs) /= rhs; }
    friend bool operator == (const mint& lhs, const mint& rhs) { return lhs._v == rhs._v; }
    friend bool operator != (const mint& lhs, const mint& rhs) { return lhs._v != rhs._v; }
  private:
    unsigned int _v;
    static barrett bt;
    static unsigned int umod() { return bt.umod(); }
};

template <int id>
barrett modint32_dynamic<id>::bt(998244353);

template<int id>
std::ostream &operator<<(std::ostream &dest, const modint32_dynamic<id> &a) {
    dest << a.val();
    return dest;
}

// 関数の内部などで使う用
using temporary_modint32 = modint32_dynamic<std::numeric_limits<int>::min()>;
#endif
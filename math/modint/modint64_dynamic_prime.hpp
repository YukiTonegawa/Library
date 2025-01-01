#ifndef _MODINT64_PRIME_H_
#define _MODINT64_PRIME_H_
#include "../number_theory/miller_rabin64.hpp"
#include "../base/fast_mul_mod/montgomery64.hpp"

// @param mod < 2^63, modは素数
template<int id> 
struct modint64_dynamic_prime {
    using mint = modint64_dynamic_prime;
  
  public:  
    static long long mod() { return (long long)mr.mod(); }
    
    static void set_mod(long long m) {
        assert(1 <= m && m < (1ULL << 63));
        assert(miller_rabin64(m));
        mr = montgomery64(m);
    }

    static mint raw(unsigned long long v) {
        mint x;
        x._v = v;
        return x;
    }

    modint64_dynamic_prime(): _v(0){}
    
    template <class T>
    modint64_dynamic_prime(T v) {
        long long x = v % (long long)(mod());
        if(x < 0) x += mod();
        _v = mr.generate(x);
    }

    unsigned long long val() const { return mr.reduce(_v); }
    
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
        _v = mr.add(_v, rhs._v);
        return *this;
    }
    mint& operator -= (const mint& rhs) {
        _v = mr.sub(_v, rhs._v);
        return *this;
    }
    mint& operator *= (const mint& rhs) {
        _v = mr.mul(_v, rhs._v);
        return *this;
    }
    mint& operator /= (const mint& rhs) { return *this = *this * rhs.inv(); }
    mint operator + () const { return *this; }
    mint operator - () const { return mint() - *this; }
    mint pow(long long n) const {
        assert(0 <= n);
        return raw(mr.pow(_v, n));
    }
    mint inv() const {
        return raw(mr.pow(_v, mod() - 2));
    }
    friend mint operator +  (const mint& lhs, const mint& rhs) { return mint(lhs) += rhs; }
    friend mint operator -  (const mint& lhs, const mint& rhs) { return mint(lhs) -= rhs; }
    friend mint operator *  (const mint& lhs, const mint& rhs) { return mint(lhs) *= rhs; }
    friend mint operator /  (const mint& lhs, const mint& rhs) { return mint(lhs) /= rhs; }
    friend bool operator == (const mint& lhs, const mint& rhs) { return lhs._v == rhs._v; }
    friend bool operator != (const mint& lhs, const mint& rhs) { return lhs._v != rhs._v; }
  
  private:
    unsigned long long _v;
    static montgomery64 mr;
    static unsigned long long umod() { return mr.mod(); }
};

template <int id>
montgomery64 modint64_dynamic_prime<id>::mr(998244353);

// 関数の内部などで使う用
using temporary_modint64 = modint64_dynamic_prime<std::numeric_limits<int>::min()>;
#endif
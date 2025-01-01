#ifndef _POW_H_
#define _POW_H_

#include "safe_mod.hpp"
// x^n mod m
// @param n `0 <= n`
// @param m `1 <= m`
constexpr long long pow_mod_constexpr(long long x, long long n, int m) {
    if (m == 1) return 0;
    unsigned int _m = (unsigned int)(m);
    unsigned long long r = 1;
    unsigned long long y = safe_mod(x, m);
    while (n) {
        if (n & 1) r = (r * y) % _m;
        y = (y * y) % _m;
        n >>= 1;
    }
    return r;
}

constexpr __uint128_t pow_mod64_constexpr(__int128_t x, __uint128_t n, unsigned long long m) {
    if (m == 1) return 0;
    __uint128_t r = 1;
    if (x >= m) x %= m;
    if (x < 0) x += m;
    while (n) {
        if (n & 1) r = (r * x) % m;
        x = (x * x) % m;
        n >>= 1;
    }
    return r;
}

#endif
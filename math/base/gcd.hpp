#ifndef _GCD_H_
#define _GCD_H_
#include <cmath>
#include <algorithm>
#include <tuple>
#include "safe_mod.hpp"

// -10^18 <= _a, _b <= 10^18
long long gcd(long long _a, long long _b) {
    long long a = abs(_a), b = abs(_b);
    if (a == 0) return b;
    if (b == 0) return a;
    int shift = __builtin_ctzll(a | b);
    a >>= __builtin_ctzll(a);
    do{
        b >>= __builtin_ctzll(b);
        if(a > b) std::swap(a, b);
        b -= a;
    } while (b);
    return a << shift;
}

// 最大でa*b
// -10^18 <= a, b <= 10^18
// a, bは負でもいいが非負の値を返す
__int128_t lcm(long long a, long long b) {
    a = abs(a), b = abs(b);
    long long g = gcd(a, b);
    if (!g) return 0;
    return __int128_t(a) * b / g;
}

// {x, y, gcd(a, b)} s.t. ax + by = gcd(a, b)
// g >= 0
std::tuple<long long, long long, long long> extgcd(long long a, long long b) {
    long long x, y;
    for (long long u = y = 1, v = x = 0; a;) {
        long long q = b / a;
        std::swap(x -= q * u, u);
        std::swap(y -= q * v, v);
        std::swap(b -= q * a, a);
    }
    // x + k * (b / g), y - k * (a / g) も条件を満たす(kは任意の整数)
    return {x, y, b};
}

// @param b `1 <= b`
// @return pair(g, x) s.t. g = gcd(a, b), xa = g (mod b), 0 <= x < b/g
constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};
    long long s = b, t = a;
    long long m0 = 0, m1 = 1;
    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;
        auto tmp = s;
        s = t;
        t = tmp;
        tmp = m0;
        m0 = m1;
        m1 = tmp;
    }
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

#endif
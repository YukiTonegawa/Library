#ifndef _LINEAR_MOD_H_
#define _LINEAR_MOD_H_
#include <tuple>
#include <vector>
#include <cassert>
#include "floor_sum.hpp"
#include "../base/safe_mod.hpp"
#include "../base/sequence_sum.hpp"
#include "../base/gcd.hpp"

// (ai + b) % modのprefix_minumumを与える等差数列
std::vector<std::tuple<long long, long long, long long>> prefix_minimum(long long a, long long b, long long mod) {
    assert(0 < mod);
    a = safe_mod(a, mod);
    b = safe_mod(b, mod);
    if (a == 0 || b == 0) return {{0, 1, 1}};
    long long p = 0, q = 1, r = 1, s = 0, g = gcd(a, mod);
    a = (mod - a) / g;
    mod /= g;
    long long X = 0, Y = b;
    std::vector<std::tuple<long long, long long, long long>> res;
    auto step = [&](){
        long long P = p + r, Q = q + s;
        long long R = mod * r - a * s;
        long long S = a * q - mod * p;
        if (a * Q <= mod * P){
            long long x = R / S;
            r += p * x;
            s += q * x;
        } else {
            std::swap(R, S);
            long long x = R / S;
            long long y = std::max(0LL, (g * R - Y + g * S - 1) / (g * S));
            long long c = g * (a * (q + s * y) - mod * (p + r * y));
            if (c == 0) {
                s = mod;
                return;
            }
            if (y <= x && 0 < c && c <= Y) {
                long long k = Y / c;
                res.push_back({X, k + 1, q + s * y});
                Y -= c * k;
                X += (q + s * y) * k;
                p += r * std::min(x, y + 1);
                q += s * std::min(x, y + 1);
            }else{
                p += r * x;
                q += s * x;
            }
        }
    };
    
    while (q < mod && s < mod) step();
    if (q < mod && a * q - mod * p > 0) {
        long long c = g * (a * q - mod * p);
        long long k = Y / c;
        res.push_back({X, k + 1, q});
    }
    return res;
}

// min((ai + b) % mod)  0 <= i < r
long long min_linear_mod(long long l, long long r, long long a, long long b, long long mod) {
    assert(0 < mod && l <= 0 && l < r);
    a = safe_mod(a, mod);
    b = safe_mod(b + a * l, mod);
    r -= l;
    if (a == 0) return b;
    auto pm = prefix_minimum(a, b, mod);
    int idx = std::lower_bound(pm.begin(), pm.end(), std::tuple<long long, long long, long long>{r, 0, 0}) - pm.begin() - 1;
    auto [x0, k, d] = pm[idx];
    long long x = x0 + d * std::min(k - 1, (r - 1 - x0) / d);
    return (a * x + b) % mod;
}

// max((ai + b) % mod)  0 <= i < r
long long max_linear_mod(long long l, long long r, long long a, long long b, long long mod) {
    return mod - 1 - min_linear_mod(l, r, -a, -(b + 1), mod);
}

// l <= iかつ 0 <= (ai + b) % mod < rとなるような最小の整数i
// 存在しないなら-1
// 0 <= r, l
// 0 < mod
long long next_lower_linear_mod(long long l, long long r, long long a, long long b, long long mod) {
    a = safe_mod(a, mod);
    b = safe_mod(b + a * l, mod);
    assert(0 <= l && 0 <= r && 0 < mod);
    if (r >= mod) return l;
    if (a == 0) return b >= r ? -1 : l;
    auto pm = prefix_minimum(a, b, mod);
    for(auto [x0, k, d] : pm) {
        long long x_max = x0 + d * (k - 1);
        if ((a * x_max + b) % mod >= r) continue;
        long long y0 = (a * x0 + b) % mod;
        if (y0 < r) return l + x0;
        long long diff = mod - (d * a % mod); // xがd増加した時の減少量
        assert(diff != mod);
        long long ret = x0 + d * ((y0 - r + diff) / diff);
        assert(ret <= x_max);
        return l + ret;
    }
    return -1;
}

// 0 <= i < nについて, 0 <= (ai + b) % mod < rとなるようなiの数
// 0 < mod
// 0 <= n, r
long long count_lower_linear_mod(long long n, long long r, long long a, long long b, long long mod) {
    assert(0 <= n && 0 <= r);
    a = safe_mod(a, mod);
    b = safe_mod(b, mod);
    if (r >= mod) return n;
    long long x = floor_sum(n, a, b + mod, mod);
    long long y = floor_sum(n, a, b + mod - r, mod);
    return x - y;
}

// ∑(0 <= i < n) (ai + b) % mod
long long sum_linear_mod(long long n, long long a, long long b, long long mod) {
    a = safe_mod(a, mod);
    b = safe_mod(b, mod);
    // (ai + b) % mod = (ai + b) - mod * floor((ai + b) / mod)
    return arithmetic_sum<__int128_t>(a, b, 0, n) - (__int128_t)floor_sum(n, a, b, mod) * mod;
}

// 数列Sのi項目 S_i := ai + bとしたとき
// ∑{0 <= i < n} (S_i % mod) ^ 2
__int128_t sum_linear_mod_square(long long n, long long a, long long b, long long mod) {
    __int128_t x = square_sum<__int128_t>(a, 0, n) + arithmetic_sum<__int128_t>((__int128_t)2 * a * b, b * b, 0, n);
    __int128_t y = (__int128_t)mod * a * floor_sum_slope<__int128_t>(n, a, b, mod) + (__int128_t)mod * b * floor_sum(n, a, b, mod);
    __int128_t z = (__int128_t)mod * mod * floor_sum_square<__int128_t>(n, a, b, mod);
    return x - 2 * y + z;
}

// 数列Sのi項目 S_i := ai + bとしたとき
// ∑{0 <= i < n} (S_i % mod) * S_i
__int128_t sum_linear_weighted_mod(long long n, long long a, long long b, long long mod) {
    __int128_t x = square_sum<__int128_t>(a, 0, n) + arithmetic_sum<__int128_t>((__int128_t)2 * a * b, b * b, 0, n);
    __int128_t y = (__int128_t)mod * a * floor_sum_slope<__int128_t>(n, a, b, mod);
    __int128_t z = (__int128_t)mod * b * floor_sum(n, a, b, mod);
    return x - y - z;
}

// i項目がai + bの等差数列の要素のうち, 以下の条件を満たすものの和
// 0 <= i < n
// 0 <= (ci + d) % mod < r
long long sum_linear_mod_lower(long long n, long long r, long long a, long long b, long long c, long long d, long long mod) {
    __int128_t x = floor_sum_slope<__int128_t>(n, c, d + mod, mod) - floor_sum_slope<__int128_t>(n, c, d + mod - r, mod);
    long long y = floor_sum(n, c, d + mod, mod) - floor_sum(n, c, d + mod - r, mod);
    return a * x + b * y;
}

// i項目がfloor((ai + b) / c)の数列の要素のうち, 以下の条件を満たすものの和
// 0 <= i < n
// 0 <= (ai + b) % mod < r
long long floor_sum_linear_mod_lower(long long n, long long r, long long a, long long b, long long c, long long mod) {
    __int128_t x = floor_sum_square<__int128_t>(n, mod * a, mod * b + c * mod, mod * c);
    __int128_t y = floor_sum_square<__int128_t>(n, mod * a, mod * b + c * (mod - r), mod * c);
    long long count = floor_sum(n, a, b + mod, mod) - floor_sum(n, a, b + mod - r, mod); // 条件を満たす項の数
    return (x - y + count) / 2;
}
#endif
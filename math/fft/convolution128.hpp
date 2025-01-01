#ifndef _CONVOLUTION_INT128_H_
#define _CONVOLUTION_INT128_H_
#include "convolution_mod.hpp"
#include "../base/gcd.hpp"

// 答えが__uint128_tに収まる
std::vector<__uint128_t> convolution128(const std::vector<uint64_t> &a, const std::vector<uint64_t> &b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    // naive
    if (std::min(n, m) <= 60) {
        std::vector<__uint128_t> ans(n + m - 1, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                ans[i + j] += (__uint128_t)a[i] * b[j];
            }
        }
        return ans;
    }
    static constexpr unsigned long long MOD1 = 754974721;  // 2^24
    static constexpr unsigned long long MOD2 = 998244353;  // 2^23
    static constexpr unsigned long long MOD3 = 167772161;  // 2^25
    static constexpr unsigned long long MOD4 = 469762049;  // 2^26
    static constexpr unsigned long long MOD5 = 1224736769; // 2^24
    static constexpr unsigned long long M1M2 = MOD1 * MOD2;
    static constexpr __uint128_t M1M2M3 = (__uint128_t)M1M2 * MOD3;
    static constexpr __uint128_t M1M2M3M4 = M1M2M3 * MOD4;
    static constexpr unsigned long long IM1_M2 = inv_gcd(MOD1, MOD2).second;
    static constexpr unsigned long long IM1M2_M3 = inv_gcd(MOD1 * MOD2, MOD3).second;
    static constexpr unsigned long long IM1M2M3_M4 = inv_gcd((long long)(M1M2M3 % MOD4), MOD4).second;
    static constexpr unsigned long long IM1M2M3M4_M5 = inv_gcd((long long)(M1M2M3M4 % MOD5), MOD5).second;
    auto c1 = convolution_mod<MOD1>(a, b);
    auto c2 = convolution_mod<MOD2>(a, b);
    auto c3 = convolution_mod<MOD3>(a, b);
    auto c4 = convolution_mod<MOD4>(a, b);
    auto c5 = convolution_mod<MOD5>(a, b);
    std::vector<__uint128_t> c(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
        __uint128_t x = c1[i], y, t;
        t = ((c2[i] < x ? MOD2 + c2[i] - x : c2[i] - x) * IM1_M2) % MOD2;
        x += t * MOD1; // x < 2^60
        y = x % MOD3;
        t = ((c3[i] < y ? MOD3 + c3[i] - y : c3[i] - y) * IM1M2_M3) % MOD3;
        x += t * M1M2; // x < 2^90
        y = x % MOD4;
        t = ((c4[i] < y ? MOD4 + c4[i] - y : c4[i] - y) * IM1M2M3_M4) % MOD4;
        x += t * M1M2M3; // x < 2^120
        y = x % MOD5;
        t = ((c5[i] < y ? MOD5 + c5[i] - y : c5[i] - y) * IM1M2M3M4_M5) % MOD5;
        // 上の解の集合を x_5 + k * M1M2M3M4M5, 0 <= x_5 < M1M2M3M4M5とすると
        // 求めたい解が2^128未満にただ1つ存在することがわかっており, M1M2M3M4M5 > 2^128であるためk = 0
        /// t * M1M2M3M4 <= M1M2M3M4M5 - M1M2M3M4
        //             x_4 < M1M2M3M4
        // であるため, x_4 + t * M1M2M3M4 = x_5 < 2^128でオーバーフローしない
        x += t * M1M2M3M4;
        c[i] = x;
    }
    return c;
}
#endif
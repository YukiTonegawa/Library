#ifndef _SQUARE_128_H_
#define _SQUARE_128_H_
#include "square_mod.hpp"
#include "../base/gcd.hpp"

// 答えが__uint128_tに収まる
std::vector<__uint128_t> square128(const std::vector<uint64_t> &a) {
    int n = int(a.size());
    if (!n) return {};
    // naive
    if (n <= 60) {
        std::vector<__uint128_t> ans(n + n - 1, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                ans[i + j] += (__uint128_t)a[i] * a[j];
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
    auto c1 = square_mod<MOD1>(a);
    auto c2 = square_mod<MOD2>(a);
    auto c3 = square_mod<MOD3>(a);
    auto c4 = square_mod<MOD4>(a);
    auto c5 = square_mod<MOD5>(a);
    std::vector<__uint128_t> c(n + n - 1);
    for (int i = 0; i < n + n - 1; i++) {
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
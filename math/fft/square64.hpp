#ifndef _SQUARE64_H_
#define _SQUARE64_H_
#include "butterfly.hpp"
#include "../base/gcd.hpp"
#include "../base/safe_mod.hpp"
#include "square_mod.hpp"

// 答えがlong longに収まる
std::vector<long long> square64(const std::vector<long long>& a) {
    int n = int(a.size());
    if (!n) return {};
    static constexpr unsigned long long MOD1 = 754974721;  // 2^24
    static constexpr unsigned long long MOD2 = 167772161;  // 2^25
    static constexpr unsigned long long MOD3 = 469762049;  // 2^26
    static constexpr unsigned long long M2M3 = MOD2 * MOD3;
    static constexpr unsigned long long M1M3 = MOD1 * MOD3;
    static constexpr unsigned long long M1M2 = MOD1 * MOD2;
    static constexpr unsigned long long M1M2M3 = MOD1 * MOD2 * MOD3;

    static constexpr unsigned long long i1 = inv_gcd(MOD2 * MOD3, MOD1).second;
    static constexpr unsigned long long i2 = inv_gcd(MOD1 * MOD3, MOD2).second;
    static constexpr unsigned long long i3 = inv_gcd(MOD1 * MOD2, MOD3).second;
        
    static constexpr int MAX_AB_BIT = 24;
    static_assert(MOD1 % (1ull << MAX_AB_BIT) == 1, "MOD1 isn't enough to support an array length of 2^24.");
    static_assert(MOD2 % (1ull << MAX_AB_BIT) == 1, "MOD2 isn't enough to support an array length of 2^24.");
    static_assert(MOD3 % (1ull << MAX_AB_BIT) == 1, "MOD3 isn't enough to support an array length of 2^24.");
    assert(n + n - 1 <= (1 << MAX_AB_BIT));

    auto c1 = square_mod<MOD1>(a);
    auto c2 = square_mod<MOD2>(a);
    auto c3 = square_mod<MOD3>(a);

    std::vector<long long> c(n + n - 1);
    for (int i = 0; i < n + n - 1; i++) {
        unsigned long long x = 0;
        x += (c1[i] * i1) % MOD1 * M2M3;
        x += (c2[i] * i2) % MOD2 * M1M3;
        x += (c3[i] * i3) % MOD3 * M1M2;
        long long diff = c1[i] - safe_mod((long long)(x), (long long)(MOD1));
        if (diff < 0) diff += MOD1;
        static constexpr unsigned long long offset[5] = { 0, 0, M1M2M3, 2 * M1M2M3, 3 * M1M2M3 };
        x -= offset[diff % 5];
        c[i] = x;
    }
    return c;
}
#endif
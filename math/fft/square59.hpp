#ifndef _SQUARE59_H_
#define _SQUARE59_H_
#include <numeric>
#include "square_mod.hpp"

// 計算結果の各項が2^59未満
std::vector<long long> square59(const std::vector<long long> &a) {
    static constexpr long long MOD1 = 998244353;
    static constexpr long long MOD2 = 754974721;
    long long sum = std::accumulate(a.begin(), a.end(), 0LL);
    auto b = square_mod<MOD1, long long>(a);
    long long sum2 = std::accumulate(b.begin(), b.end(), 0LL);
    if (sum * sum == sum2) return b;
    auto c = square_mod<MOD2, long long>(a);
    static constexpr long long i1 = inv_gcd(MOD1, MOD2).second;
    // MOD1 * i1 * (c[i] - b[i]) + b[i]
    for (int i = 0; i < c.size(); i++) {
        c[i] = i1 * (c[i] - b[i] + 2 * MOD2) % MOD2;
        c[i] = c[i] * MOD1 + b[i];
    }
    return c;
}
#endif
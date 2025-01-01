#ifndef _CONVOLUTION59_H_
#define _CONVOLUTION59_H_
#include "convolution_mod.hpp"
#include <numeric>

// 計算結果の各項が2^59未満
std::vector<long long> convolution59(const std::vector<long long> &a, const std::vector<long long> &b) {
    static constexpr long long MOD1 = 998244353;
    static constexpr long long MOD2 = 754974721;
    long long sumA = std::accumulate(a.begin(), a.end(), 0LL);
    long long sumB = std::accumulate(b.begin(), b.end(), 0LL);
    auto c = convolution_mod<MOD1, long long>(a, b);
    long long sumC = std::accumulate(c.begin(), c.end(), 0LL);
    if (sumA * sumB == sumC) return c;
    auto d = convolution_mod<MOD2, long long>(a, b);
    static constexpr long long i1 = inv_gcd(MOD1, MOD2).second;
    for (int i = 0; i < d.size(); i++) {
        d[i] = i1 * (d[i] - c[i] + 2 * MOD2) % MOD2;
        d[i] = d[i] * MOD1 + c[i];
    }
    return d;
}
#endif
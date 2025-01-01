#ifndef _SQUARE_ARBITRARY_MOD_H_
#define _SQUARE_ARBITRARY_MOD_H_
#include "square_mod.hpp"

template<class mint>
std::vector<mint> square_arbitrary_mod(const std::vector<mint>& a) {
    int n = int(a.size());
    if (!n) return {};
    if (n <= 60) return square_naive(a);
    static constexpr long long MOD1 = 167772161;
    static constexpr long long MOD2 = 469762049;
    static constexpr long long MOD3 = 1224736769;
    static constexpr long long M1M2 = MOD1 * MOD2;
    static constexpr long long ix = inv_gcd(MOD1, MOD2).second;
    static constexpr long long i3 = inv_gcd(MOD1 * MOD2, MOD3).second;
    
    std::vector<long long> a2(n);
    for (int i = 0; i < n; i++) a2[i] = a[i].val();
    auto c1 = square_mod<MOD1>(a2);
    auto c2 = square_mod<MOD2>(a2);
    auto c3 = square_mod<MOD3>(a2);

    std::vector<mint> c(n + n - 1);
    int M1M2m = M1M2 % mint::mod();
    for (int i = 0; i < n + n - 1; i++) {
        long long v = (((long long)c2[i] - c1[i]) * ix) % MOD2;
        if (v < 0) v += MOD2;
        long long xxv = c1[i] + MOD1 * v;
        v = ((c3[i] - (xxv % MOD3)) * i3) % MOD3;
        if (v < 0) v += MOD3;
        c[i] = mint(xxv + M1M2m * v);
    }
    return c;
}
#endif
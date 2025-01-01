#ifndef _MILLER_RABIN64_CONSTEXPR_H_
#define _MILLER_RABIN64_CONSTEXPR_H_
#include "../base/pow_mod.hpp"
#include <cstdint>
#include <array>

// 遅い代わりにconstexpr
template<uint64_t n>
constexpr bool miller_rabin64_constexpr() {
    constexpr std::array<uint64_t, 7> A{2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    if (n % 2 == 0) return false;
    uint64_t d = n - 1;
    d >>= __builtin_ctzll(d);
    for (uint64_t a : A) {
        if (a % n == 0) continue;
        uint64_t d2s = d; // d * 2^s, 0 <= s <= (n - 1)が2で割れる回数
        uint64_t y = pow_mod64_constexpr(a, d, n);
        while (d2s != n - 1 && y != 1 && y != n - 1) {
            y = pow_mod64_constexpr(y, y, n);
            d2s <<= 1;
        }
        if (y != n - 1 && d2s % 2 == 0) return false;
    }
    return true;
}
#endif
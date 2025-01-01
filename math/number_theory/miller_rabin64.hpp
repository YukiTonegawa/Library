#ifndef _MILLER_RABIN64_H_
#define _MILLER_RABIN64_H_
#include "../base/fast_mul_mod/montgomery64.hpp"
#include <vector>

static constexpr int threshold_small_number = 1000;

bool is_prime_small(uint64_t x) {
    assert(0 <= x && x < threshold_small_number);
    static std::vector<bool> small;
    if (small.empty()) {
        small.resize(threshold_small_number, 1);
        small[0] = small[1] = false;
        for (int i = 2; i < threshold_small_number; i++) {
            if (!small[i]) continue;
            for (int j = 2 * i; j < threshold_small_number; j += i) {
                small[j] = false;
            }
        }
    }
    if (x <= 1) return false;
    return small[x];
}

bool miller_rabin_mr(uint64_t n, const montgomery64 &mr) {
    static std::vector<uint64_t> A{2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    static std::vector<uint64_t> B{2, 7, 61};
    assert(0 <= n);
    
    if (n < threshold_small_number) return is_prime_small(n);
    if (!(n & 1)) return false;

    uint64_t d = n - 1;
    uint64_t one = mr.one(), mone = mr.generate(n - 1);
    d >>= __builtin_ctzll(d);
    for (uint64_t a : (n >> 32) ? A : B) {
        if (a % n == 0) continue;
        uint64_t d2s = d; // d * 2^s, 0 <= s <= (n - 1)が2で割れる回数
        uint64_t y = mr.pow(mr.generate(a), d);
        while (d2s != n - 1 && y != one && y != mone) {
            y = mr.mul(y, y);
            d2s <<= 1;
        }
        if (y != mone && !(d2s & 1)) return false;
    }
    return true;
}

bool miller_rabin64(uint64_t n) {
    assert(0 <= n);
    if (n < threshold_small_number) return is_prime_small(n);
    if (n % 2 == 0) return n == 2 ? true : false;
    montgomery64 mr(n);
    return miller_rabin_mr(n, mr);
}
#endif
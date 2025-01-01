#ifndef _MATH64_H_
#define _MATH64_H_
#include <cstdint>
#include <numeric>

constexpr std::pair<uint64_t, bool> add64(uint64_t a, uint64_t b) {
    if (__builtin_add_overflow(a, b, &a)) {
        return {std::numeric_limits<uint64_t>::max(), true};
    }
    return {a, false};
}

constexpr std::pair<uint64_t, bool> mul64(uint64_t a, uint64_t b) {
    if (__builtin_mul_overflow(a, b, &a)) {
        return {std::numeric_limits<uint64_t>::max(), true};
    }
    return {a, false};
}

constexpr std::pair<uint64_t, bool> pow64(uint64_t a, uint64_t b) {
    uint64_t res = 1;
    while (b) {
        if ((b & 1) && __builtin_mul_overflow(res, a, &res)) {
            return {std::numeric_limits<uint64_t>::max(), true};
        }
        if (__builtin_mul_overflow(a, a, &a) && b) {
            return {std::numeric_limits<uint64_t>::max(), true};
        }
        b >>= 1;
    }
    return {res, false};
}

constexpr std::pair<uint64_t, bool> fac64(uint64_t n) {
    if (n > 20) return {std::numeric_limits<uint64_t>::max(), true};
    uint64_t res = 1;
    for (int i = 1; i <= n; i++) {
        res *= i;
    }
    return {res, false};
}


constexpr std::pair<uint64_t, bool> perm64(uint64_t n, uint64_t k) {
    if (n < 0 || k < 0 || n < k) return {0, false};
    uint64_t res = 1;
    for (uint64_t x = n; x > n - k; x--) {
        if (__builtin_mul_overflow(res, x, &res)) {
            return {std::numeric_limits<uint64_t>::max(), true};
        }
    }
    return {res, false};
}

constexpr std::pair<uint64_t, bool> comb64(uint64_t n, uint64_t k) {
    if (n < 0 || k < 0 || n < k) return {0, false};
    if (n - k < k) k = n - k;
    __uint128_t res = 1;
    for (uint64_t d = 1; d <= k; d++) {
        res *= n--;
        res /= d;
        if (res > std::numeric_limits<uint64_t>::max()) {
            return {std::numeric_limits<uint64_t>::max(), true};
        }
    }
    return {res, false};
}
#endif
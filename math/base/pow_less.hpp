#ifndef _POW_LESS_H_
#define _POW_LESS_H_

#include <cstdint>
#include <cassert>
#include <vector>

// a^k <= xを満たす最大の{a^k, k}
std::pair<uint64_t, int> pow_less(uint64_t a, uint64_t x) {
    assert(a > 1 && x > 0);
    if (a == 2) {
        int msb = 63 - __builtin_clzll(x);
        return {1ULL << msb, msb};
    }
    if (a > 10) {
        uint64_t y = 1, tmp;
        for (int i = 0; ; i++) {
            if (__builtin_mul_overflow(y, a, &tmp) || tmp > x) {
                return {y, i};
            } else {
                y = tmp;
            }
        }
    }
    static std::vector<std::vector<uint64_t>> pow_table(11);
    if (pow_table[a].empty()) {
        pow_table[a].push_back(1);
        uint64_t y = 1, tmp;
        while (true) {
            if (__builtin_mul_overflow(y, a, &tmp) || tmp > x) {
                break;
            } else {
                pow_table[a].push_back(y);
            }
        }
    }
    int ansidx = (std::upper_bound(pow_table[a].begin(), pow_table[a].end(), x) - pow_table[a].begin()) - 1;
    return {pow_table[a][ansidx], ansidx};
}
#endif
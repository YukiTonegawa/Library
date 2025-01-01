#ifndef _KTH_ROOT_INT_H_
#define _KTH_ROOT_INT_H_
#include <cstdint>
#include <cmath>
#include <cassert>
#include <vector>

// a^k <= x を満たす最大のa
uint64_t kth_root(uint64_t x, uint64_t k) {
    assert(1 <= k);
    assert(0 <= x);
    static std::vector<std::vector<uint64_t>> table;
    if (x <= 1) return (!k ? 1 : x);
    if (k == 1) return x;
    if (k == 2) {
        uint64_t y = sqrt((double)x), z;
        while (__builtin_mul_overflow(y, y, &z) || z > x) {
            y--;
        }
        while (!__builtin_mul_overflow(y + 1, y + 1, &z) && z <= x) {
            y++;
        }
        return y;
    }
    if (k >= 64 || (x >> k) == 0) return 1;

    if (table.empty()) {
        table.resize(64);
        for (int t = 5; t < 64; t++) {
            for (uint64_t x = 1;; x++) {
                uint64_t y = 1;
                bool ok = true;
                for (int i = 0; i < t; i++) {
                    if (__builtin_mul_overflow(x, y, &y)) {
                        ok = false;
                        break;
                    }
                }
                if (!ok) break;
                table[t].push_back(y);
            }
        }
    }

    // この時点で 3 <= k < 64
    if (k >= 5) {
        return std::upper_bound(table[k].begin(), table[k].end(), x) - table[k].begin();
    }

    if (k == 4) {
        uint64_t y = sqrt(sqrt((double)x)), z;
        while (__builtin_mul_overflow(y * y, y * y, &z) || z > x) {
            y--;
        }
        while (!__builtin_mul_overflow((y + 1) * (y + 1), (y + 1) * (y + 1), &z) && z <= x) {
            y++;
        }
        return y;
    }

    // k == 3
    uint64_t y = cbrt((double)x), z;
    while (__builtin_mul_overflow(y * y, y, &z) || z > x) {
        y--;
    }
    while (!__builtin_mul_overflow((y + 1) * (y + 1), (y + 1), &z) && z <= x) {
        y++;
    }
    return y;
}
#endif
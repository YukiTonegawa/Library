#ifndef _SELECT_H_
#define _SELECT_H_
#include <stdint.h>
#include <array>
#include <vector>
static constexpr uint64_t mask_48_64 = 0xFFFF000000000000;
static constexpr uint64_t mask_32_48 = 0x0000FFFF00000000;
static constexpr uint64_t mask_0_32  = 0x00000000FFFFFFFF;
static constexpr uint64_t mask_16_32 = 0x00000000FFFF0000;
static constexpr uint64_t mask_0_16  = 0x000000000000FFFF;
static constexpr int select_N = 16;
static constexpr int select_M = 1 << select_N;
using select_table_type = std::array<std::array<int8_t, select_N>, select_M>;

select_table_type select_build() {
    select_table_type res;
    for (int i = 0; i < select_M; i++) {
        res[i].fill(-1);
        int pcnt = 0;
        for (int j = 0; j < select_N; j++) {
            if((i >> j) & 1) res[i][pcnt++] = j;
        }
    }
    return res;
}

// k番目(0-indexed)の1の場所(0-indexed)を返す. 無い場合壊れる
int select_32bit(uint32_t x, int k) {
    static select_table_type table = select_build();
    int r = __builtin_popcount(x & mask_0_16);
    if (r > k) {
        return table[x & mask_0_16][k];
    } else {
        return 16 + table[(x & mask_16_32) >> 16][k - r];
    }
}

// k番目(0-indexed)の1の場所(0-indexed)を返す. 無い場合壊れる
int select_64bit(uint64_t x, int k) {
    static select_table_type table = select_build();
    int r = __builtin_popcount(x & mask_0_32);
    if (r > k) {
        int rr = __builtin_popcount(x & mask_0_16);
        if (rr > k) {
            return table[x & mask_0_16][k];
        } else {
            return 16 + table[(x & mask_16_32) >> 16][k - rr];
        }
    } else {
        k -= r;
        int lr = __builtin_popcountll(x & mask_32_48);
        if (lr > k) {
            return 32 + table[(x & mask_32_48) >> 32][k];
        } else {
            return 48 + table[(x & mask_48_64) >> 48][k - lr];
        }
    }
}
#endif
#ifndef _HILBERT_CURVE_H_
#define _HILBERT_CURVE_H_
#include <cassert>
#include <algorithm>

// {x, y}のヒルベルト曲線上の順序
// 通常のヒルベルト曲線と比べて2^Kの曲線上の順序を2^{K+1}の曲線でも保持するように適宜回転している (グリッドサイズを指定しなくて良くなる)
long long hilbert_curve_order(int x, int y) {
    assert(0 <= x && 0 <= y);
    if (x == 0 && y == 0) return 0;
    int msb = 31 - __builtin_clz(std::max(x, y));
    if (msb & 1) std::swap(x, y);
    long long res = 0;
    for (int i = msb; i >= 0; i--) {
        res <<= 2;
        int f = (x >> i) & 1, g = (y >> i) & 1;
        res += (f << 1) + (f ^ g);
        if (!g) {
            if (f) x = ~x, y = ~y;
            std::swap(x, y);
        }
    }
    return res;
}

/*
// {x, y, z}の3次元ヒルベルト曲線上の順序
long long hilbert_curve_order(int x, int y, int z) {
    assert(0 <= x && 0 <= y && 0 <= z);
    if (x == 0 && y == 0 && z == 0) return 0;
    int msb = 31 - __builtin_clz(std::max({x, y, z}));
    assert(msb < 20);
    long long res = 0;
    
    return res;
}
*/

#endif
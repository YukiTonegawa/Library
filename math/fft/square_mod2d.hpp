#ifndef _SQUARE_MOD2D_H_
#define _SQUARE_MOD2D_H_
#include "butterfly2d.hpp"
#include "../base/bit_ceil.hpp"

// n * m配列
template<typename mint>
std::vector<std::vector<mint>> square_mod2d(std::vector<std::vector<mint>> a) {
    int n = a.size();
    int m = a.empty() ? 0 : a[0].size();
    if (!n || !m) return {};
    int N = bit_ceil(2 * n - 1);
    int M = bit_ceil(2 * m - 1);
    a.resize(N);
    for(int i = 0; i < N; i++){
        assert(a[i].size() <= m);
        a[i].resize(M, 0);
    }
    butterfly2d<mint>(a);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            a[i][j] *= a[i][j];
        }
    }
    butterfly_inv2d<mint>(a);
    mint iz = (mint(N) * M).inv();
    a.resize(2 * n - 1);
    for (int i = 0; i < 2 * n - 1; i++) {
        a[i].resize(2 * m - 1);
        for (int j = 0; j < 2 * m - 1; j++) {
            a[i][j] *= iz;
        }
    }
    return a;
}
#endif
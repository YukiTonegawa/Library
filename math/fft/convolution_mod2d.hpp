#ifndef _CONVOLUTION_MOD2D_H_
#define _CONVOLUTION_MOD2D_H_
#include "butterfly2d.hpp"
#include "../base/bit_ceil.hpp"

// n * m配列
template<typename mint>
std::vector<std::vector<mint>> convolution_mod2d(std::vector<std::vector<mint>> a, std::vector<std::vector<mint>> b) {
    int n1 = a.size();
    int m1 = a.empty() ? 0 : a[0].size();
    int n2 = b.size();
    int m2 = b.empty() ? 0 : b[0].size();

    if (!n1 || !n2 || !m1 || !m2) return {};
    int N = bit_ceil(n1 + n2 - 1);
    int M = bit_ceil(m1 + m2 - 1);
    a.resize(N);
    b.resize(N);
    for(int i = 0; i < N; i++){
        a[i].resize(M, 0);
        b[i].resize(M, 0);
    }
    butterfly2d<mint>(a);
    butterfly2d<mint>(b);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            a[i][j] *= b[i][j];
        }
    }
    butterfly_inv2d<mint>(a);
    mint iz = (mint(N) * M).inv();
    a.resize(n1 + n2 - 1);
    for (int i = 0; i < n1 + n2 - 1; i++) {
        a[i].resize(m1 + m2 - 1);
        for (int j = 0; j < m1 + m2 - 1; j++) {
            a[i][j] *= iz;
        }
    }
    return a;
}
#endif
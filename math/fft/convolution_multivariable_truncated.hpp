#ifndef _CONVOLUTION_MULTI_TRUNCATED_H_
#define _CONVOLUTION_MULTI_TRUNCATED_H_
#include "butterfly.hpp"
#include <vector>
#include <cassert>

// L[i] := xiの最大次数
template<typename mint>
std::vector<mint> convolution_multivariable_truncated(const std::vector<int> &L, const std::vector<mint> &_f, const std::vector<mint> &_g) {
    int K = L.size(), M = 1;
    std::vector<int> Lprod(K);
    for (int i = 0; i < K; i++) {
        M *= L[i];
        Lprod[i] = L[i] * (i == 0 ? 1 : Lprod[i - 1]);
    }
    assert(M == int(_f.size()) && M == int(_g.size()));
    int M2 = bit_ceil(2 * M - 1);

    auto tdim = [&](int x) -> int {
        int sum = 0;
        for (int i = 0; i < K; i++) {
            sum += x / Lprod[i];
        }
        return sum % (K + 1);
    };

    std::vector<std::vector<mint>> f(K + 1, std::vector<mint>(M2, 0));
    auto g = f;
    auto h = f;
    for (int i = 0; i < M; i++) {
        int d = tdim(i);
        f[d][i] = _f[i];
        g[d][i] = _g[i];
    }
    for (int i = 0; i <= K; i++) {
        butterfly(f[i]);
        butterfly(g[i]);
    }
    for (int i = 0; i <= K; i++) {
        for (int j = 0; j <= K; j++) {
            int k = (i + j) % (K + 1);
            for (int l = 0; l < M2; l++) {
                h[k][l] += f[i][l] * g[j][l];
            }
        }
    }
    for (int i = 0; i <= K; i++) {
        butterfly_inv(h[i]);
    }
    std::vector<mint> res(M);
    mint i2 = mint(M2).inv();
    for (int i = 0; i < M; i++) {
        int d = tdim(i);
        res[i] = h[d][i] * i2;
    }
    return res;
}


template<typename mint>
std::vector<mint> square_multivariable_truncated(const std::vector<int> &L, const std::vector<mint> &_f) {
    int K = L.size(), M = 1;
    std::vector<int> Lprod(K);
    for (int i = 0; i < K; i++) {
        M *= L[i];
        Lprod[i] = L[i] * (i == 0 ? 1 : Lprod[i - 1]);
    }
    assert(M == int(_f.size()));
    int M2 = bit_ceil(2 * M - 1);

    auto tdim = [&](int x) -> int {
        int sum = 0;
        for (int i = 0; i < K; i++) {
            sum += x / Lprod[i];
        }
        return sum % (K + 1);
    };
    
    std::vector<std::vector<mint>> f(K + 1, std::vector<mint>(M2, 0));
    auto h = f;
    for (int i = 0; i < M; i++) {
        int d = tdim(i);
        f[d][i] = _f[i];
    }
    for (int i = 0; i <= K; i++) {
        butterfly(f[i]);
    }
    for (int i = 0; i <= K; i++) {
        for (int j = i + 1; j <= K; j++) {
            int k = (i + j) % (K + 1);
            for (int l = 0; l < M2; l++) {
                h[k][l] += 2 * f[i][l] * f[j][l];
            }
        }
        {
            int k = (2 * i) % (K + 1);
            for (int l = 0; l < M2; l++) {
                h[k][l] += f[i][l] * f[i][l];
            }
        }
    }
    for (int i = 0; i <= K; i++) {
        butterfly_inv(h[i]);
    }
    std::vector<mint> res(M);
    mint i2 = mint(M2).inv();
    for (int i = 0; i < M; i++) {
        int d = tdim(i);
        res[i] = h[d][i] * i2;
    }
    return res;
}

#endif
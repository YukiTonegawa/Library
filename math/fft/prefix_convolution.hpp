#ifndef _PREFIX_CONVOLUTION_H_
#define _PREFIX_CONVOLUTION_H_
#include "butterfly.hpp"
#include <algorithm>
#include <vector>

// N = Q = 1e5 : 200ms
// N = Q = 2e5 : 700ms
// N = Q = 3e5 : 1500ms
template<typename mint>
struct prefix_convolution {
    int N, M, n, m;
    static constexpr int S = 2048;
    mint iz;
    std::vector<mint> _A, _B;
    std::vector<std::vector<mint>> A, B;
    std::vector<std::vector<std::vector<mint>>> C;
    
    prefix_convolution(const std::vector<mint> &_A, const std::vector<mint> &_B) : iz(mint(2 * S).inv()), _A(_A), _B(_B) {
        N = _A.size(), M = _B.size();
        n = (N + S - 1) / S, m = (M + S - 1) / S;
        A.resize(n, std::vector<mint>(2 * S, 0));
        B.resize(m, std::vector<mint>(2 * S, 0));
        C.resize(n, std::vector<std::vector<mint>>(m));
        for (int i = 0; i < N; i++) {
            int j = i / S, k = i % S;
            A[j][k] = _A[i];
        }
        for (int i = 0; i < M; i++) {
            int j = i / S, k = i % S;
            B[j][k] = _B[i];
        }
        for (int i = 0; i < n; i++) butterfly(A[i]);
        for (int i = 0; i < m; i++) butterfly(B[i]);

        std::vector<mint> tmp(2 * S, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                for (int k = 0; k < 2 * S; k++) {
                    tmp[k] = A[i][k] * B[j][k];
                }
                butterfly_inv(tmp);
                C[i][j] = tmp;
            }
        }
    }
    
    // A[0, ra) * B[0, rb)のx^dの係数
    mint query(int ra, int rb, int d) {
        assert(0 <= ra && ra <= N);
        assert(0 <= rb && rb <= M);
        int RA = ra / S, RB = rb / S;
        if (2 * ra > S * (2 * RA + 1)) RA++;
        if (2 * rb > S * (2 * RB + 1)) RB++;
        mint res = 0;
        for (int i = 0; i < RA; i++) {
            for (int j = (d - i * S) / S, k = 0; k < 2; j--, k++) {
                if (0 <= j && j < RB) {
                    int pos = d - (i + j) * S;
                    if (0 <= pos && pos < 2 * S) {
                        res += C[i][j][pos];
                    }
                }
            }
        }
        res *= iz;
        RA *= S;
        RB *= S;
        bool reva = ra < RA;
        bool revb = rb < RB;
        if (reva) std::swap(ra, RA);
        if (revb) std::swap(rb, RB);
        {
            mint tmp = 0;
            int l = std::max(RA, d - rb + 1), r = std::min(ra, d - RB + 1);
            for (int i = l; i < r; i++) {
                tmp += _A[i] * _B[d - i];
            }
            if (reva && revb) res -= tmp;
            if (!reva && !revb) res += tmp;
        }
        {
            mint tmp = 0;
            int l = std::max(RA, d - RB + 1), r = std::min(ra, d + 1);
            for (int i = l; i < r; i++) {
                tmp += _A[i] * _B[d - i];
            }
            res += reva ? -tmp : tmp;
        }
        {
            mint tmp = 0;
            int l = std::max(RB, d - RA + 1), r = std::min(rb, d + 1);
            for (int i = l; i < r; i++) {
                tmp += _B[i] * _A[d - i];
            }
            res += revb ? -tmp : tmp;
        }
        return res;
    }
};
#endif
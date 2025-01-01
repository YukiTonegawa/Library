#ifndef _RANGE_WILDCARD_PATTERN_MATCHING_H_
#define _RANGE_WILDCARD_PATTERN_MATCHING_H_
#include <vector>
#include <algorithm>
#include <cassert>
#include "../../math/fft/convolution_mod.hpp"

struct range_wildcard_matching {
    int N, M, n, m;
    static constexpr int S = 2048;
    using mint = modint998244353;
    std::vector<int> _A, _B;
    std::vector<std::vector<mint>> A, B, AA, BB, Aflag, Bflag;
    std::vector<std::vector<std::bitset<2 * S>>> C;
    
    range_wildcard_matching(const std::vector<int> &a, const std::vector<int> &b) {
        N = a.size(), M = b.size();
        n = (N + S - 1) / S, m = (M + S - 1) / S;
        _A.resize(N);
        _B.resize(M);
        A.resize(n, std::vector<mint>(2 * S, 0));
        B.resize(m, std::vector<mint>(2 * S, 0));
        C.resize(m, std::vector<std::bitset<2 * S>>(n));
        AA = Aflag = A;
        BB = Bflag = B;
        for (int i = 0; i < N; i++) {
            _A[i] = a[N - 1 - i];
            int j = i / S, k = i % S;
            A[j][k] = _A[i];
            AA[j][k] = A[j][k] * A[j][k];
            Aflag[j][k] = (A[j][k] != 0);
        }
        for (int i = 0; i < M; i++) {
            _B[i] = b[i];
            int j = i / S, k = i % S;
            B[j][k] = _B[i];
            BB[j][k] = B[j][k] * B[j][k];
            Bflag[j][k] = (B[j][k] != 0);
        }
        for (int i = 0; i < n; i++) {
            butterfly(A[i]);
            butterfly(AA[i]);
            butterfly(Aflag[i]);
        }
        for (int i = 0; i < m; i++) {
            butterfly(B[i]);
            butterfly(BB[i]);
            butterfly(Bflag[i]);
        }

        std::vector<mint> tmp(2 * S, 0);
        for (int j = 0; j < m; j++) {
            for (int i = 0; i < n; i++) {
                for (int k = 0; k < 2 * S; k++) {
                    tmp[k] = AA[i][k] * Bflag[j][k] - 2 * A[i][k] * B[j][k] + Aflag[i][k] * BB[j][k];
                }
                butterfly_inv(tmp);
                for (int k = 0; k < 2 * S; k++) C[j][i][k] = tmp[k] != 0;
            }
        }
    }

    int lcp(int la, int lb) {
        assert(0 <= la && la < N);
        assert(0 <= lb && lb < M);
        int maxlen = std::min(N - la, M - lb);
        int ra = N - la, RA = ra / S;
        int d = N - 1 - la + lb;
        int LB = (lb + S - 1) / S;

        int r = std::min(std::max(LB * S, lb + ra - RA * S), lb + maxlen);
        for (int i = lb; i < r; i++) {
            if (_B[i] != 0 && _A[d - i] != 0 && _B[i] != _A[d - i]) {
                return i - lb;
            }
        }
        if (r == lb + maxlen) return maxlen;

        for (int i = LB; i < m; i++) {
            int shiftb = i * S;
            for (int j = (d - shiftb) / S, k = 0; k < 2; j--, k++) {
                if (0 <= j && j < RA) {
                    int shifta = j * S;
                    int pos = d - shiftb - shifta;
                    if (pos < 0 || pos >= 2 * S || C[i][j][pos] == 0) continue;
                    int l = std::max(pos - S + 1, 0);
                    int r = std::min(S, pos + 1);
                    for (int k = l; k < r; k++) {
                        if (_B[k + shiftb] != 0 && _A[pos - k + shifta] != 0 && _B[k + shiftb] != _A[pos - k + shifta]) {
                            return i * S - lb + k;
                        }
                    }
                }
            }
        }
        return maxlen;
    }

    bool match(int la, int lb) {
        int maxlen = std::min(N - la, M - lb);
        return lcp(la, lb) == maxlen;
    }
};

#endif
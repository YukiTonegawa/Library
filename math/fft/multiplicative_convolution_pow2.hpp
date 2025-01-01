#ifndef _MULTI_CONVOLUTION_POW2_H_
#define _MULTI_CONVOLUTION_POW2_H_
#include "butterfly.hpp"

// C_{ij % 2^K} = ∑∑(A[i] * B[j]) mod P
template<typename mint>
std::vector<mint> multiplicative_convolution_pow2(int K, const std::vector<mint> &a, const std::vector<mint> &b) {
    assert((1 << K) == a.size() && (1 << K) == b.size());
    std::vector<std::vector<int>> ord(K); // ord[i][j] := 5^j mod 2^{K-i} 
    std::vector<std::vector<mint>> A(K), B(K), Am(K), Bm(K);
    std::vector<mint> Asum(K, 0), Bsum(K, 0);
    std::vector<mint> ans(1 << K, 0);
    ans[0] = a[0] * b[0];
    for (int i = 0; i < K; i++) {
        for (int j = (1 << i); j < (1 << K); j += (2 << i)) {
            Asum[i] += a[j];
            Bsum[i] += b[j];
        }
        ans[0] += a[0] * Bsum[i] + b[0] * Asum[i];
        int mod = 1 << (K - i);
        ord[i].resize(mod / 4);
        A[i].resize(mod / 4);
        Am[i].resize(mod / 4);
        B[i].resize(mod / 4);
        Bm[i].resize(mod / 4);
        for (int j = 0, k = 1; j < mod / 4; j++, k = (k * 5) % mod) {
            ord[i][j] = k;
            int idx1 = k << i;
            int idx2 = (1 << K) - idx1;
            A[i][j] = a[idx1];
            Am[i][j] = a[idx2];
            B[i][j] = b[idx1];
            Bm[i][j] = b[idx2];
        }
    }
    // Aの2^iとBの2^j
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            if (i + j >= K) {
                ans[0] += Asum[i] * Bsum[j];
                continue;
            }
            int mod = 1 << (K - i - j - 1);
            if (mod <= 8) {
                std::vector<mint> _A(mod, 0), _B(mod, 0);
                for (int k = (1 << i), t = 0; k < (1 << K); k += (2 << i), t = (t + 1) % mod) {
                    _A[t] += a[k];
                }
                for (int k = (1 << j), t = 0; k < (1 << K); k += (2 << j), t = (t + 1) % mod) {
                    _B[t] += b[k];
                }
                for (int k = 0; k < mod; k++) {
                    for (int t = 0; t < mod; t++) {
                        int idx = (k * 2 + 1) * (t * 2 + 1);
                        idx = (idx % (2 * mod)) << (i + j);
                        ans[idx] += _A[k] * _B[t];
                    }
                }
                continue;
            }
            std::vector<mint> _A(mod, 0), _Am(mod, 0), _B(mod, 0), _Bm(mod, 0), tmp(mod, 0);
            int N = A[i].size(), M = B[j].size();
            for (int k = 0; k < N; k++) {
                int idx = k % (mod / 2);
                _A[idx] += A[i][k];
                _Am[idx] += Am[i][k];
            }
            for (int k = 0; k < M; k++) {
                int idx = k % (mod / 2);
                _B[idx] += B[j][k];
                _Bm[idx] += Bm[j][k];
            }
            butterfly(_A);
            butterfly(_Am);
            butterfly(_B);
            butterfly(_Bm);
            for (int k = 0; k < mod; k++) {
                tmp[k] = _A[k] * _B[k];
                _B[k] *= _Am[k];
                _Am[k] *= _Bm[k];
                _Bm[k] *= _A[k];
            }
            butterfly_inv(tmp);
            butterfly_inv(_B);
            butterfly_inv(_Am);
            butterfly_inv(_Bm);
            mint iz = mint(mod).inv();
            for (int k = 0, t = mod / 2; k < mod / 2; k++, t++) {
                int idx = ord[i + j][k] << (i + j);
                ans[idx] += (tmp[k] + tmp[t] + _Am[k] + _Am[t]) * iz;
                ans[(1 << K) - idx] += (_B[k] + _B[t] + _Bm[k] + _Bm[t]) * iz;
            }
        }
    }
    return ans;
}
#endif
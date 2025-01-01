#ifndef _NUMBER_OF_INCREASING_SEQUENCE_H_
#define _NUMBER_OF_INCREASING_SEQUENCE_H_
#include <vector>
#include "../modint/combination.hpp"
#include "../fft/convolution_mod.hpp"
#include "counting_path_on_grid.hpp"

// 0 <= Ai <= xi < Biを満たす広義単調増加列の数
// res[i] := A_j <= i < B_jである最右のjに対して末尾がiである長さj+1の列の数
// 必要 : A, Bが広義単調増加列
template<typename mint>
std::vector<mint> number_of_increasing_sequence(const std::vector<int> &A, const std::vector<int> &B) {
    int N = A.size();
    assert(B.size() == N);
    if (N == 0) return {};

    if (A[0] >= B[0]) return std::vector<mint>(B.back(), 0);
    for (int i = 1; i < N; i++) {
        assert(A[i - 1] <= A[i]);
        assert(B[i - 1] <= B[i]);
        if (A[i] >= B[i]) return std::vector<mint>(B.back(), 0);
    }

    std::vector<mint> D(N + 1, 0), L(B.back(), 0);
    L[A[0]] = 1;
    std::vector<int> C(N);

    auto dfs_inc = [&](auto &&dfs_inc, int l, int r, int d) -> void {
        if (d == C[r - 1]) return;
        if (r - l == 1) {
            L[d] += D[l];
            for (int i = d + 1; i < C[l]; i++) L[i] += L[i - 1];
            D[l] = L[C[l] - 1];
            return;
        }
        int m = (l + r) / 2;
        if (l < m) dfs_inc(dfs_inc, l, m, d);
        int y = C[m];
        int Hi = y - d, Wi = r - m;
        if (Hi && Wi) {
            std::vector<mint> w(Hi + Wi - 1, 0);
            for (int i = 0; i < Hi; i++) w[i] = L[y - 1 - i];
            for (int i = 0; i < Wi; i++) w[Hi - 1 + i] += D[m + i];
            w = counting_path_on_grid<mint>::solve(Hi, Wi, w);
            for (int i = 0; i < Wi; i++) D[m + i] = w[i];
            for (int i = 0; i < Hi; i++) L[y - 1 - i] = w[Wi - 1 + i];
        }
        if (m < r) dfs_inc(dfs_inc, m, r, y);
    };

    auto dfs_dec = [&](auto &&dfs_dec, int l, int r, int d) -> void {
        if (d == C[l]) return;
        if (r - l == 1) {
            L[C[l] - 1] += D[l];
            for (int i = C[l] - 2; i >= d; i--) L[i] += L[i + 1];
            D[l] = L[d];
            for (int i = d; i < C[l]; i++) D[l + 1] += L[i];
            return;
        }
        int m = (l + r) / 2;
        int y = C[m - 1];
        if (l < m) dfs_dec(dfs_dec, l, m, y);
        int Hi = y - d, Wi = m - l;
        if (Hi && Wi) {
            std::vector<mint> w(Hi + Wi - 1, 0);
            for (int i = 0; i < Hi; i++) w[i] = L[d + i];
            for (int i = 0; i < Wi; i++) w[Hi - 1 + i] += D[l + i];
            w = counting_path_on_grid<mint>::solve(Hi, Wi, w);
            for (int i = 0; i < Hi; i++) L[d + i] = w[Wi - 1 + i];
            for (int i = 0; i < Wi; i++) D[l + i] = w[i];
        }
        for (int i = C[m]; i < y; i++) D[m] += L[i];
        if (m < r) dfs_dec(dfs_dec, m, r, d);
    };

    mint low_sum = 0;
    int l = 0;
    while (l < N) {
        int r = l;
        do { r++; } while (r < N && A[r] < B[l]);
        D[l] = low_sum;
        low_sum = 0;
        for (int i = l; i < r; i++) C[i] = B[l] - A[i] + A[l];
        std::reverse(L.begin() + A[l], L.begin() + B[l]);
        dfs_dec(dfs_dec, l, r, A[l]);
        for (int i = A[l]; i < C[r - 1]; i++) low_sum += L[i];
        std::reverse(L.begin() + A[l], L.begin() + B[l]);
        for (int i = l; i < r; i++) C[i] = B[i];
        dfs_inc(dfs_inc, l, r, B[l]);
        for (int i = B[l]; i < (r < N ? A[r] : 0); i++) low_sum += L[i];
        l = r;
    }
    return L;
}
#endif
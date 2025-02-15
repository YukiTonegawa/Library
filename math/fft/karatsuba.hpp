#ifndef _KARATSUBA_H_
#define _KARATSUBA_H_
#include <vector>

// a0b0 + ({a0 + a1}{b0 + b1} - a0b0 - a1b1)X + a1b1X^2
std::vector<long long> karatsuba_64(std::vector<int> A, std::vector<int> B) {
    int N = A.size(), M = B.size(), K = N + M - 1;
    N = std::max(N, M);
    A.resize(N, 0);
    B.resize(N, 0);
    auto dfs = [&](auto &&dfs, int l, int r) -> std::vector<long long> {
        int len = r - l;
        std::vector<long long> C(2 * len - 1, 0);
        if (r - l <= 50) {
            for (int i = 0; i < len; i++) {
                for (int j = 0; j < len; j++) {
                    C[i + j] += (long long)A[l + i] * B[l + j];
                }
            }
            return C;
        }
        int m = (l + r + 1) / 2;
        auto c0 = dfs(dfs, l, m);
        for (int i = 0; i < r - m; i++) {
            A[l + i] += A[m + i];
            B[l + i] += B[m + i];
        }
        auto c1 = dfs(dfs, l, m);
        for (int i = 0; i < r - m; i++) {
            A[l + i] -= A[m + i];
            B[l + i] -= B[m + i];
        }
        auto c2 = dfs(dfs, m, r);
        m -= l;
        for (int i = 0; i < c0.size(); i++) {
            C[i] += c0[i];
            C[m + i] -= c0[i];
        }
        for (int i = 0; i < c1.size(); i++) {
            C[m + i] += c1[i];
        }
        for (int i = 0; i < c2.size(); i++) {
            C[m + i] -= c2[i];
            C[2 * m + i] += c2[i];
        }
        return C;
    };
    auto res = dfs(dfs, 0, N);
    res.resize(K);
    return res;
}

template<typename mint>
std::vector<mint> karatsuba_mod(std::vector<mint> A, std::vector<mint> B) {
    int N = A.size(), M = B.size(), K = N + M - 1;
    N = std::max(N, M);
    A.resize(N, 0);
    B.resize(N, 0);
    auto dfs = [&](auto &&dfs, int l, int r) -> std::vector<mint> {
        int len = r - l;
        std::vector<mint> C(2 * len - 1, 0);
        if (r - l <= 50) {
            for (int i = 0; i < len; i++) {
                for (int j = 0; j < len; j++) {
                    C[i + j] += A[l + i] * B[l + j];
                }
            }
            return C;
        }
        int m = (l + r + 1) / 2;
        auto c0 = dfs(dfs, l, m);
        for (int i = 0; i < r - m; i++) {
            A[l + i] += A[m + i];
            B[l + i] += B[m + i];
        }
        auto c1 = dfs(dfs, l, m);
        for (int i = 0; i < r - m; i++) {
            A[l + i] -= A[m + i];
            B[l + i] -= B[m + i];
        }
        auto c2 = dfs(dfs, m, r);
        m -= l;
        for (int i = 0; i < c0.size(); i++) {
            C[i] += c0[i];
            C[m + i] -= c0[i];
        }
        for (int i = 0; i < c1.size(); i++) {
            C[m + i] += c1[i];
        }
        for (int i = 0; i < c2.size(); i++) {
            C[m + i] -= c2[i];
            C[2 * m + i] += c2[i];
        }
        return C;
    };
    auto res = dfs(dfs, 0, N);
    res.resize(K);
    return res;
}
#endif
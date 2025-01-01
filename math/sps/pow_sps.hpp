#ifndef _POW_SPS_H_
#define _POW_SPS_H_
#include <vector>
#include <algorithm>
#include <array>
#include <cassert>
#include <type_traits>
#include "../base/pow_mod.hpp"
#include "subset_convolution.hpp"

template<int mod, int n, int maxn, std::enable_if_t<(n == 0)>* = nullptr>
std::array<std::array<int, 1 << n>, maxn - n + 1> pow_sps(const std::vector<int> &A, long long K) {
    static constexpr int m = 1 << n;
    std::array<std::array<int, m>, maxn - n + 1> res;
    long long d = K;
    int a = 1;
    for (int i = 0; i <= maxn; i++) {
        res[i][0] = a * pow_mod_constexpr(A[0], d, mod) % mod;
        if (d == 0) {
            a = 0;
        } else {
            a = (long long)a * (d % mod) % mod;
            d--;
        }
    }
    return res;
}

template<int mod, int n, int maxn, std::enable_if_t<(n > 0)>* = nullptr>
std::array<std::array<int, 1 << n>, maxn - n + 1> pow_sps(const std::vector<int> &A, long long K) {
    static constexpr int m = 1 << n;
    static constexpr int mh = 1 << (n - 1);
    auto F = pow_sps<mod, n - 1, maxn>(A, K);
    std::array<std::array<int, m>, maxn - n + 1> res;
    for (int i = maxn - n; i >= 0; i--) {
        std::vector<int> a(mh, 0), b(mh, 0);
        for (int j = mh; j < m; j++) a[j - mh] = A[j];
        for (int j = 0; j < mh; j++) b[j] = F[i + 1][j];
        a = subset_convolution<mod, n - 1>(a, b);
        for (int j = mh; j < m; j++) res[i][j] = a[j - mh];
        for (int j = 0; j < mh; j++) res[i][j] = F[i][j];
    }
    return res;
}

// f(A(x1, x2, ... xn))
template<int mod, int n>
std::vector<int> pow_sps(const std::vector<int> &A, long long K) {
    static_assert(n >= 0);
    assert(A.size() == 1 << n);
    auto ans = pow_sps<mod, n, n>(A, K)[0];
    return std::vector<int>(ans.begin(), ans.end());
}
#endif
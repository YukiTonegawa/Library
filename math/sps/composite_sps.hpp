#ifndef _COMPOSITE_SPS_H_
#define _COMPOSITE_SPS_H_
#include <vector>
#include <algorithm>
#include <array>
#include <cassert>
#include <type_traits>
#include "subset_convolution.hpp"

template<int mod, int n, int maxn, std::enable_if_t<(n == 0)>* = nullptr>
std::array<std::array<int, 1 << n>, maxn - n + 1> composite_sps(const std::vector<int> &A, const std::vector<int> &f) {
    static constexpr int m = 1 << n;
    std::array<std::array<int, m>, maxn - n + 1> res;
    auto _f = f;
    for (int i = 0; i <= maxn; i++) {
        int x = 1, y = 0;
        for (int k = 0; k < _f.size(); k++) {
            y = (y + (long long)x * _f[k]) % mod;
            x = ((long long)x * A[0]) % mod;
        }
        res[i][0] = {y};
        for (int k = 1; k < _f.size(); k++) {
            _f[k - 1] = ((long long)_f[k] * k) % mod;
        }
        if (!_f.empty()) _f.pop_back();
    }
    return res;
}

template<int mod, int n, int maxn, std::enable_if_t<(n > 0)>* = nullptr>
std::array<std::array<int, 1 << n>, maxn - n + 1> composite_sps(const std::vector<int> &A, const std::vector<int> &f) {
    static constexpr int m = 1 << n;
    static constexpr int mh = 1 << (n - 1);
    auto F = composite_sps<mod, n - 1, maxn>(A, f);
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
std::vector<int> composite_sps(const std::vector<int> &A, const std::vector<int> &f) {
    static_assert(n >= 0);
    assert(A.size() == 1 << n);
    auto ans = composite_sps<mod, n, n>(A, f)[0];
    return std::vector<int>(ans.begin(), ans.end());
}
#endif
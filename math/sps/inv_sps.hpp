#ifndef _INV_SPS_H_
#define _INV_SPS_H_
#include <vector>
#include <algorithm>
#include <type_traits>
#include <array>
#include <cassert>
#include "../base/gcd.hpp"
#include "subset_convolution.hpp"

template<int mod, int n, std::enable_if_t<(n == 0)>* = nullptr>
std::vector<int> inv_sps(const std::vector<int> &A) {
    auto [g, x] = inv_gcd(A[0], mod);
    assert(g == 1);
    return {(int)x};
}

template<int mod, int n, std::enable_if_t<(n > 0)>* = nullptr>
std::vector<int> inv_sps(const std::vector<int> &A) {
    static constexpr int m = 1 << n;
    static constexpr int mh = 1 << (n - 1);
    assert(A.size() == m);
    auto F = inv_sps<mod, n - 1>(std::vector<int>(A.begin(), A.begin() + mh));
    auto F2 = subset_convolution_square<mod, n - 1>(F);
    auto tmp = std::vector<int>(A.begin() + mh, A.begin() + m);
    for (int i = 0; i < mh; i++) tmp[i] = (tmp[i] == 0 ? 0 : mod - tmp[i]);
    F2 = subset_convolution<mod, n - 1>(F2, tmp);
    F2.resize(m);
    for (int i = 0; i < mh; i++) {
        F2[i + mh] = F2[i];
        F2[i] = F[i];
    }
    return F2;
}
#endif
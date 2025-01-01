#ifndef _EXP_SPS_H_
#define _EXP_SPS_H_
#include <vector>
#include <algorithm>
#include <type_traits>
#include <array>
#include <cassert>
#include "subset_convolution.hpp"

template<int mod, int n, std::enable_if_t<(n == 0)>* = nullptr>
std::vector<int> exp_sps(const std::vector<int> &A) {
    assert(A[0] == 0);
    return {1};
}

template<int mod, int n, std::enable_if_t<(n > 0)>* = nullptr>
std::vector<int> exp_sps(const std::vector<int> &A) {
    static constexpr int m = 1 << n;
    static constexpr int mh = 1 << (n - 1);
    assert(A.size() == m);
    auto B = std::vector<int>(A.begin() + mh, A.begin() + m);
    auto F = exp_sps<mod, n - 1>(std::vector<int>(A.begin(), A.begin() + mh));
    B = subset_convolution<mod, n - 1>(B, F);
    B.resize(m);
    for (int i = 0; i < mh; i++) {
        B[i + mh] = B[i];
        B[i] = F[i];
    }
    return B;
}
#endif
#ifndef _LOG_SPS_H_
#define _LOG_SPS_H_
#include <vector>
#include <algorithm>
#include <type_traits>
#include <array>
#include <cassert>
#include "inv_sps.hpp"
#include "subset_convolution.hpp"

template<int mod, int n, std::enable_if_t<(n == 0)>* = nullptr>
std::vector<int> log_sps(const std::vector<int> &A) {
    assert(A[0] == 1);
    return {0};
}

template<int mod, int n, std::enable_if_t<(n > 0)>* = nullptr>
std::vector<int> log_sps(const std::vector<int> &A) {
    static constexpr int m = 1 << n;
    static constexpr int mh = 1 << (n - 1);
    assert(A.size() == m);
    auto B = std::vector<int>(A.begin() + mh, A.begin() + m);
    auto C = std::vector<int>(A.begin(), A.begin() + mh);
    auto F = log_sps<mod, n - 1>(C);
    C = inv_sps<mod, n - 1>(C); // todo: 毎回全部計算しない
    B = subset_convolution<mod, n - 1>(B, C);
    B.resize(m);
    for (int i = 0; i < mh; i++) {
        B[i + mh] = B[i];
        B[i] = F[i];
    }
    return B;
}
#endif
#ifndef _LCM_CONVOLUTION_H_
#define _LCM_CONVOLUTION_H_

#include <vector>
#include <functional>
#include "zeta.hpp"
// 1-indexed, z[k] = ∑{i, j, lcm(i, j)==k} A[i]*B[j]
// lcm(i, j)は最大で|A|*|B|になる     O(NMlog(NM))
// https://atcoder.jp/contests/agc038/submissions/18538058　
// のようにgcd畳み込みを変形させるパターンの方が多いかも
template<typename T>
std::vector<T> lcm_convolution(std::vector<T> A, std::vector<T> B, int limit_dim) {
    int N = std::min((long long)A.size() * (long long)B.size(), (long long)limit_dim);
    A.resize(N, 0);
    B.resize(N, 0);
    auto Az = zeta_divisor<T, _add_func<T>>(std::move(A));
    auto Bz = zeta_divisor<T, _add_func<T>>(std::move(B));
    std::vector<T> Cz(N);
    for (int i = 0; i < N; i++) Cz[i] = Az[i] * Bz[i];
    return mobius_divisor<T, _sub_func<T>>(Cz);
}
#endif
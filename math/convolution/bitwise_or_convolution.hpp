#ifndef _BITWISE_OR_CONVOLUTION_H_
#define _BITWISE_OR_CONVOLUTION_H_

#include <vector>
#include "zeta.hpp"
// z[k] = ∑{i, j, (i|j)==k} A[i]*B[j] O(N * 2^N)
template<typename T>
std::vector<T> bitwise_or_convolution(std::vector<T> A, std::vector<T> B) {
    int M = bit_ceil(std::max(A.size(), B.size()));
    A.resize(M, 0);
    B.resize(M, 0);
    auto Az = zeta_subset_sum(std::move(A));
    auto Bz = zeta_subset_sum(std::move(B));
    for (int i = 0; i < M; i++) Az[i] *= Bz[i];
    //return Az;  z[k] = ∑{i, j, (i|j)⊂k} A[i]*B[j] が欲しい場合
    return mobius_subset_sum(std::move(Az));
}

#endif
#ifndef _BITWISE_XOR_CONVOLUTION_H_
#define _BITWISE_XOR_CONVOLUTION_H_

#include <vector>
#include "../base/bit_ceil.hpp"

// z[k] = ∑{i, j, (i^j)==k} A[i]*B[j] O(N * 2^N)
template<typename T>
std::vector<T> bitwise_xor_convolution(std::vector<T> A, std::vector<T> B) {
    int M = bit_ceil(std::max(A.size(), B.size()));
    A.resize(M, 0);
    B.resize(M, 0);

    for (int i = 1; i < M; i <<= 1) {
        for (int j = 0; j < M; j += i << 1) {
            for (int k = j; k < (j | i); k++) {
                T l = A[k], r = A[k | i];
                A[k] = l + r, A[k | i] = l - r;
                T L = B[k], R = B[k | i];
                B[k] = L + R, B[k | i] = L - R;
            }
        }
    }
    T i2 = T(2).inv();
    for (int i = 0; i < M; i++) A[i] *= B[i];

    for (int i = 1; i < M; i <<= 1) {
        for (int j = 0; j < M; j += i << 1) {
            for (int k = j; k < (j | i); k++) {
                T l = A[k], r = A[k | i];
                A[k] = (l + r) * i2, A[k | i] = (l - r) * i2;
            }
        }
    }
    return A;
}
#endif
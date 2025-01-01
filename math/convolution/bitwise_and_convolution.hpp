#ifndef _BITWISE_AND_CONVOLUTION_H_
#define _BITWISE_AND_CONVOLUTION_H_
#include <vector>
#include "zeta.hpp"
// z[k] = ∑{i, j, (i&j)==k} A[i]*B[j] O(N * 2^N)
template<typename T>
std::vector<T> bitwise_and_convolution(std::vector<T> A, std::vector<T> B) {
    int M = bit_ceil(std::max(A.size(), B.size()));
    A.resize(M, 0);
    B.resize(M, 0);
    auto Az = zeta_superset_sum(std::move(A));
    auto Bz = zeta_superset_sum(std::move(B));
    for (int i = 0; i < M; i++) Az[i] *= Bz[i];
    //return Az;  z[k] = ∑{i, j, (i&j)⊂k} A[i]*B[j] が欲しい場合
    return mobius_superset_sum(std::move(Az));
}

template<int mod>
std::vector<long long> bitwise_and_convolution_fast(std::vector<long long> A, std::vector<long long> B) {
    int M = bit_ceil(std::max(A.size(), B.size()));
    A.resize(M, 0);
    B.resize(M, 0);
    auto Az = zeta_superset_sum_fast<mod>(std::move(A));
    auto Bz = zeta_superset_sum_fast<mod>(std::move(B));
    for (int i = 0; i < M; i++) Az[i] = Az[i] * Bz[i] % mod;
    //return Az;  z[k] = ∑{i, j, (i&j)⊂k} A[i]*B[j] が欲しい場合
    return mobius_superset_sum_fast<mod>(std::move(Az));
}

template<int mod>
std::vector<long long> zeta_superset_sum_fast(std::vector<long long> v) {
    if (v.empty()) return {};
    int m = bit_ceil(v.size());
    v.resize(m, 0);
    
    if (1 < m) {
        for (int i = 0; i < m; i += 2) {
            v[i] += v[i + 1];
        }
    }
    if (2 < m) {
        for (int i = 0; i < m; i += 4) {
            v[i] += v[i + 2];
            v[i + 1] += v[i + 3];
        }
    }
    for (int i = 4; i < m; i <<= 1) {
        for (int j = 0; j < m; j += i << 1) {
            for (int k = j; k < (j | i); k += 4) {
                int t = k | i;
                v[k    ] += v[t];
                v[k + 1] += v[t + 1];
                v[k + 2] += v[t + 2];
                v[k + 3] += v[t + 3];
            }
        }
    }
    for (int i = 0; i < m; i++) v[i] %= mod;
    return v;
}

template<int mod>
std::vector<long long> mobius_superset_sum_fast(std::vector<long long> v) {
    if (v.empty()) return {};
    int m = bit_ceil(v.size());
    v.resize(m, 0);
    
    if (1 < m) {
        for (int i = 0; i < m; i += 2) {
            v[i] -= v[i + 1];
        }
    }
    if (2 < m) {
        for (int i = 0; i < m; i += 4) {
            v[i] -= v[i + 2];
            v[i + 1] -= v[i + 3];
        }
    }
    for (int i = 4; i < m; i <<= 1) {
        for (int j = 0; j < m; j += i << 1) {
            for (int k = j; k < (j | i); k += 4) {
                int t = k | i;
                v[k    ] -= v[t];
                v[k + 1] -= v[t + 1];
                v[k + 2] -= v[t + 2];
                v[k + 3] -= v[t + 3];
            }
        }
    }
    for (int i = 0; i < m; i++) {
        v[i] %= mod;
        if (v[i] < 0) v[i] += mod;
    }
    return v;
}
#endif
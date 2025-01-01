#ifndef _MAX_CONVOLUTION_H_
#define _MAX_CONVOLUTION_H_

#include <vector>
// z[k] = ∑{i, j, max(i, j) == k} A[i]*B[j] O(N)
template<typename T>
std::vector<T> max_convolution(const std::vector<T> &A, const std::vector<T> &B) {
    int N = max(A.size(), B.size());
    std::vector<T> Az(N, 0), Bz(N, 0);
    for (int i = 0; i < N; i++) {
        if (i) Az[i] += Az[i - 1];
        if (i < A.size()) Az[i] += A[i];
    }
    for (int i = 0; i < N; i++) {
        if (i) Bz[i] += Bz[i - 1];
        if (i < B.size()) Bz[i] += B[i];
    }
    std::vector<T> Cz(N);
    for (int i = 0; i < N; i++) Cz[i] = Az[i] * Bz[i];
    for (int i = N - 1; i >= 1; i--) Cz[i] -= Cz[i - 1];
    return Cz;
}

#endif
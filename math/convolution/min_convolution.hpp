#ifndef _MIN_CONVOLUTION_H_
#define _MIN_CONVOLUTION_H_
#include <vector>
// z[k] = ∑{i, j, min(i, j) == k} A[i]*B[j] O(N)
template<typename T>
std::vector<T> min_convolution(const std::vector<T> &A, const std::vector<T> &B) {
    int N = std::min(A.size(), B.size());
    std::vector<T> Az(N, 0), Bz(N, 0);
    for (int i = N - 1; i >= 0; i--) {
        if (i != N - 1) Az[i] += Az[i + 1];
        Az[i] += A[i];
    }
    for (int i = N - 1; i >= 0; i--) {
        if (i != N - 1) Bz[i] += Bz[i + 1];
        Bz[i] += B[i];
    }
    std::vector<T> Cz(N);
    for (int i = 0; i < N; i++) Cz[i] = Az[i] * Bz[i];
    for (int i = 0; i < N - 1; i++) Cz[i] -= Cz[i + 1];
    return Cz;
}
#endif
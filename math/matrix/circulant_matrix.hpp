#ifndef _CURCULANT_MATRIX_H_
#define _CURCULANT_MATRIX_H_
#include <vector>
template<typename mint>
struct circulant_matrix {
    int N;
    std::vector<mint> V;

    // V0   V1   V2 ...  VN-1
    // VN-1 V0   V1 ...  VN-2
    // VN-2 VN-1 V0 ...  VN-3
    circulant_matrix(int N = 0) : N(N), V(N, 0) { V[0] = 1; }
    circulant_matrix(const std::vector<mint> &_V) : N(_V.size()), V(_V) {}

    circulant_matrix operator += (const circulant_matrix &r) {
        assert(N == r.N);
        for (int i = 0; i < N; i++) V[i] += r.V[i];
        return *this;
    }

    circulant_matrix operator *= (const circulant_matrix &r) {
        assert(N == r.N);
        // use FFT
        std::vector<mint> tmp(2 * N, 0);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                tmp[i + j] += V[i] * r.V[j];
            }
        }
        for (int i = 0; i < N; i++) V[i] = tmp[i] + tmp[i + N];
        return *this;
    }

    circulant_matrix operator + (const circulant_matrix &r) const {
        circulant_matrix res(*this);
        return res += r;
    }

    circulant_matrix operator * (const circulant_matrix &r) const {
        circulant_matrix res(*this);
        return res *= r;
    }

    circulant_matrix operator ^ (long long k) const {
        circulant_matrix res(N);
        circulant_matrix A(*this);
        while (k) {
            if (k & 1) res *= A;
            k >>= 1;
            A *= A;
        }
        return res;
    }
};
#endif
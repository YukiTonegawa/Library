#ifndef _MULTI_CONVOLUTION_PRIME_H_
#define _MULTI_CONVOLUTION_PRIME_H_
#include "convolution_mod.hpp"
#include "../number_theory/primitive_root32.hpp"

template<typename mint>
std::vector<mint> multiplicative_convolution_prime(int P, const std::vector<mint> &a, const std::vector<mint> &b) {
    assert(a.size() == P && b.size() == P);
    mint asum = 0, bsum = 0;
    for (int i = 0; i < P; i++) {
        asum += a[i];
        bsum += b[i];
    }
    int g = primitive_root32_constexpr(P);
    std::vector<int> f(P - 1), fi(P); // g^i = f[i]
    for (int i = 0, x = 1; i < P - 1; i++, x = ((long long)x * g) % P) {
        f[i] = x;
        fi[x] = i;
    }
    std::vector<mint> A(P - 1), B(P - 1);
    for (int i = 0; i < P - 1; i++) {
        A[i] = a[f[i]];
        B[i] = b[f[i]];
    }
    auto C = convolution_mod<mint>(A, B);
    std::vector<mint> ans(P);
    ans[0] = asum * b[0] + bsum * a[0] - a[0] * b[0];
    for (int i = 1; i < P; i++) {
        ans[i] = C[fi[i]] + (C.size() > fi[i] + P - 1 ? C[fi[i] + P - 1] : 0);
    }
    return ans;
}
#endif
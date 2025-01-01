#ifndef _STIRLING_NUMBER_H_
#define _STIRLING_NUMBER_H_
#include <vector>
#include "../fps/falling_factorial.hpp"
#include "../fps/fps.hpp"
#include "../fps/taylor_shift.hpp"

// s(N, 0), s(N, 1), ... s(N, N)
template<typename mint>
std::vector<mint> stirling_number_first_fixed_n(int N) {
    fps<mint> f{0, 1}, g{1};
    int M = N, s = 1;
    while (M) {
        if (M & 1) g = f * taylor_shift<fps<mint>>(g, -s);
        f *= taylor_shift<fps<mint>>(f, -s);
        M >>= 1;
        s <<= 1;
    }
    g.resize(N + 1, 0);
    return g;
}

// s(0, K), s(1, K), ... s(N, K)　{s(K未満, K)は0とする}
template<typename mint>
std::vector<mint> stirling_number_first_fixed_k(int maxN, int K) {
    if (maxN < K) return std::vector<mint>(maxN + 1, 0);
    fps<mint> f{1, -1};
    f = (-f.log(maxN + 1)).pow(K);
    f.resize(maxN + 1, 0);
    combination_mod<mint>::build(maxN);
    for (int i = 0; i < K; i++) f[i] = 0;
    for (int i = K; i <= maxN; i++) {
        f[i] *= combination_mod<mint>::ifac(K) * combination_mod<mint>::fac(i);
        if ((i + K) & 1) f[i] = -f[i];
    }
    return f;
}

// S(N, 0), S(N, 1), ... S(N, N)
template<typename mint>
std::vector<mint> stirling_number_second_fixed_n(int N) {
    std::vector<mint> Y(N + 1);
    for (int i = 0; i <= N; i++) Y[i] = mint(i).pow(N);
    auto f = falling_factorial<mint>::interpolation(Y);
    return f;
}

// S(0, K), S(1, K), ... S(N, K)　{S(K未満, K)は0とする}
template<typename mint>
std::vector<mint> stirling_number_second_fixed_k(int maxN, int K) {
    if (maxN < K) return std::vector<mint>(maxN + 1, 0);
    fps<mint> f(maxN + 1, 0);
    combination_mod<mint>::build(maxN);
    for (int i = 1; i <= maxN; i++) {
        f[i] = combination_mod<mint>::ifac(i);
    }
    f = f.pow(K);
    f.resize(maxN + 1, 0);
    for (int i = 0; i < K; i++) f[i] = 0;
    for (int i = K; i <= maxN; i++) f[i] *= combination_mod<mint>::fac(i) * combination_mod<mint>::ifac(K);
    return f;
}
#endif
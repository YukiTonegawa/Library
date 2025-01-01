#ifndef _SUBSET_CONVOLUTION_H_
#define _SUBSET_CONVOLUTION_H_
#include <vector>
#include <numeric>
#include <cassert>
#include <array>
#include <algorithm>
#include "ranked_zeta.hpp"
#include "ranked_mobius.hpp"

namespace subset_convolution_inner {
    template<int mod, int n>
    std::vector<int> subset_convolution_power2(std::vector<int> &A, std::vector<int> &B) {
        auto a = ranked_zeta<mod, n>(A.begin(), A.end());
        auto b = ranked_zeta<mod, n>(B.begin(), B.end());
        std::array<__int128_t, n + 1> tmp;
        for (int i = 0; i < (1 << n); i++) {
            int max_size = __builtin_popcount(i);
            tmp.fill(0);
            for (int j = 0; j <= max_size; j++) {
                for (int k = max_size - j; k <= std::min(n - j, max_size); k++) {
                    tmp[j + k] += (long long)a[i][j] * b[i][k];
                }
            }
            for (int j = 0; j <= n; j++) a[i][j] = tmp[j] % mod;
        }
        ranked_mobius<998244353, n>(a);
        std::vector<int> res(1 << n);
        for (int i = 0; i < (1 << n); i++) {
            res[i] = a[i][__builtin_popcount(i)];
        }
        return res;
    }

    template<int mod, int n>
    std::vector<int> subset_convolution_square_power2(std::vector<int> &A) {
        auto a = ranked_zeta<mod, n>(A.begin(), A.end());
        std::array<__int128_t, n + 1> tmp;
        for (int i = 0; i < (1 << n); i++) {
            int max_size = __builtin_popcount(i);
            tmp.fill(0);
            for (int j = 0; j <= max_size; j++) {
                for (int k = max_size - j; k <= std::min(n - j, max_size); k++) {
                    tmp[j + k] += (long long)a[i][j] * a[i][k];
                }
            }
            for (int j = 0; j <= n; j++) a[i][j] = tmp[j] % mod;
        }
        ranked_mobius<998244353, n>(a);
        std::vector<int> res(1 << n);
        for (int i = 0; i < (1 << n); i++) {
            res[i] = a[i][__builtin_popcount(i)];
        }
        return res;
    }

    template<int mod, int Nlog, std::enable_if_t<Nlog >= 25>* = nullptr>
    std::vector<int> subset_convolution(std::vector<int> &A, std::vector<int> &B) {
        assert(false);
        return {};
    }

    template<int mod, int Nlog, std::enable_if_t<Nlog < 25>* = nullptr>
    std::vector<int> subset_convolution(std::vector<int> &A, std::vector<int> &B) {
        if (std::max(A.size(), B.size()) <= (1 << Nlog)) {
            A.resize(1 << Nlog, 0);
            B.resize(1 << Nlog, 0);
            return subset_convolution_power2<mod, Nlog>(A, B);
        }
        return subset_convolution<mod, Nlog + 1>(A, B);
    }

    template<int mod, int Nlog, std::enable_if_t<Nlog >= 25>* = nullptr>
    std::vector<int> subset_convolution_square(std::vector<int> &A) {
        assert(false);
        return {};
    }

    template<int mod, int Nlog, std::enable_if_t<Nlog < 25>* = nullptr>
    std::vector<int> subset_convolution_square(std::vector<int> &A) {
        if (A.size() <= (1 << Nlog)) {
            A.resize(1 << Nlog, 0);
            return subset_convolution_square_power2<mod, Nlog>(A);
        }
        return subset_convolution_square<mod, Nlog + 1>(A);
    }
};

template<int mod>
std::vector<int> subset_convolution(std::vector<int> A, std::vector<int> B) {
    return subset_convolution_inner::subset_convolution<mod, 0>(A, B);
}

template<int mod>
std::vector<int> subset_convolution_square(std::vector<int> A) {
    return subset_convolution_inner::subset_convolution_square<mod, 0>(A);
}
#endif
#ifndef _FACTORIAL_LARGE_H_
#define _FACTORIAL_LARGE_H_
#include <vector>
#include "../fps/falling_factorial.hpp"

template<typename mint>
struct factorial_large {
    static std::vector<mint> bfac;
    static constexpr int K = 9;
    
    static void build() {
        bfac = {1, 3};
        for (int k = 1; k < K; k++) {
            auto f = falling_factorial<mint>::interpolation(bfac);
            f = f.shift(1 << k);
            auto tmp = f.multipoint_evaluation(3 << k);
            bfac.insert(bfac.end(), tmp.begin(), tmp.end());
            for (int j = 0; j < (2 << k); j++) {
                bfac[j] = bfac[2 * j] * bfac[2 * j + 1] * mint(2 * j + 1) * mint(1 << k);
            }
            bfac.resize(2 << k);
        }
        auto f = falling_factorial<mint>::interpolation(bfac);
        bfac = f.multipoint_evaluation((mint::mod() >> K) + 1);
        for (int i = 0; i < int(bfac.size()); i++) {
            bfac[i] *= (i + 1) << K;
            if (i) bfac[i] *= bfac[i - 1];
        }
    }
    
    // k!
    static mint fac(int k) {
        int block = k >> K;
        mint res = (block ? bfac[block - 1] : 1);
        for (int i = (block << K) + 1; i <= k; i++) res *= i;
        return res;
    }

    static mint perm(int n, int k) {
        if (k < 0 || n < k) return 0;
        return fac(n) / fac(k);
    }
    
    static mint comb(int n, int k) {
        if (k < 0 || n < k) return 0;
        return fac(n) / (fac(k) * fac(n - k));
    }
};
template<typename mint>
std::vector<mint> factorial_large<mint>::bfac;
#endif
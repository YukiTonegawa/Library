#ifndef _CHROMATIC_NUMBER_H_
#define _CHROMATIC_NUMBER_H_
#include <vector>
#include "../../math/modint/modint32_static.hpp"

template<typename Int, int mod>
int chromatic_number_mod(const std::vector<Int> &g) {
    using mint = modint32_static<mod>;
    int n = g.size(), m = 1 << n;
    if (n == 0) return 0;
    std::vector<mint> x(m, 0); // 1色で塗れるか
    for (int i = 0; i < m; i++) {
        bool ok = true;
        for (int j = 0; j < n; j++) {
            if (((i >> j) & 1) && (i & g[j])) {
                ok = false;
                break;
            }
        }
        x[i] = ok;
    }
    auto subset_zeta = [m](std::vector<mint> &A) -> void {
        for (int i = 1; i < m; i <<= 1){
            for (int j = 0; j < m; j++) {
                if (!(j & i)) {
                    A[j | i] += A[j];
                }
            }
        }
    };
    auto multiply = [m](std::vector<mint> &A, const std::vector<mint> &B) -> void {
        for (int i = 0; i < m; i++) {
            A[i] *= B[i];
        }
    };
    // メビウス変換をした後にm-1が1以上になるか
    auto check = [n, m](const std::vector<mint> &A) -> bool {
        mint s = 0;
        bool g = n % 2;
        for (int i = 0; i < m; i++) {
            if (__builtin_popcount(i) % 2 == g) {
                s += A[i];
            } else {
                s -= A[i];
            }
        }
        return s != 0;
    };
    subset_zeta(x);
    auto y = x;
    for (int i = 1; i <= n; i++) {
        if (i != 1) multiply(y, x);
        if (check(y)) return i;
    }
    return -1;
}

template<typename Int>
int chromatic_number(const std::vector<Int> &g) {
    int res = g.size();
    res = std::min(res, chromatic_number_mod<Int, 998244353>(g));
    res = std::min(res, chromatic_number_mod<Int, 999999893>(g));
    return res;
}
#endif
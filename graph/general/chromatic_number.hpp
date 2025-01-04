#ifndef _CHROMATIC_NUMBER_H_
#define _CHROMATIC_NUMBER_H_
#include <vector>
#include "../../math/modint/modint32_static.hpp"


// O(2^N * N) こっちの方が早い
std::vector<std::vector<int>> chromatic_number_deterministic(const std::vector<std::vector<int>> &G) {
    int n = G.size(), m = 1 << n;
    if (n == 0) return {};
    std::vector<int> z(m, 0); // 1色で塗れるか

    std::vector<int> g(n, 0);
    for (int i = 0; i < n; i++) {
        for (int j : G[i]) {
            assert(i != j);
            g[i] |= 1 << j;
        }
    }
    
    for (int i = 0; i < m; i++) {
        bool ok = true;
        for (int j = 0; j < n; j++) {
            if (((i >> j) & 1) && (i & g[j])) {
                ok = false;
                break;
            }
        }
        z[i] = ok;
    }

    auto zeta = [](std::vector<int> &A) -> void {
        int k = A.size();
        for (int i = 1; i < k; i <<= 1){
            for (int j = 0; j < k; j++) {
                if (!(j & i)) {
                    A[j | i] += A[j];
                }
            }
        }
    };

    auto mobius = [](std::vector<int> &A) -> void {
        int k = A.size();
        for (int i = 1; i < k; i <<= 1){
            for (int j = 0; j < k; j++) {
                if (!(j & i)) {
                    A[j | i] -= A[j];
                }
            }
        }
    };

    std::vector<std::vector<int>> dp;
    dp.push_back(z);

    for (int i = 0;; i++) {
        if (!dp.back()[(1 << (n - i)) - 1]) {
            int k = 1 << (n - i - 1);
            std::vector<int> x(k, 0);
            std::vector<int> y = x;
            for (int j = 0; j < (1 << n); j++) {
                x[j >> (i + 1)] |= z[j];
            }
            for (int j = 1; j < 2 * k; j += 2) {
                y[j >> 1] |= (dp.back()[j] ? 1 : 0);
            }
            zeta(x);
            zeta(y);
            for (int j = 0; j < k; j++) {
                x[j] *= y[j];
            }
            mobius(x);
            dp.push_back(x);
        } else {
            std::vector<std::vector<int>> res(i + 1);
            int unused = (1 << n) - 1;
            for (; i >= 0; i--) {
                int mask = (1 << i) - 1;
                for (int j = 1; j < (1 << n); j++) {
                    if (!z[j] || (j & unused) != j) continue;
                    int f = unused ^ j;
                    if (i) {
                        if ((f & mask) != mask || !dp[i - 1][f >> (i - 1)]) {
                            continue;
                        }
                    } else {
                        if (f) {
                            continue;
                        }
                    }
                    for (int k = 0; k < n; k++) {
                        if ((j >> k) & 1) {
                            res[i].push_back(k);
                        }
                    }
                    unused = f;
                    break;
                }
            }
            return res;
        }
    }
}

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
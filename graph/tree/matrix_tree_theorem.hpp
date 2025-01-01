#ifndef _MATRIX_TREE_THEOREM_H_
#define _MATRIX_TREE_THEOREM_H_

#include "../../math/matrix/matrix_mod.hpp"

// 無向グラフの全域木の個数
template<typename mint, bool is_prime_mod>
mint matrix_tree_theorem_undirected(int N, const std::vector<std::pair<int, int>> &E) {
    if (N <= 1) return 1;
    matrix_mod<mint> L(N - 1, N - 1, 0);
    for (auto [s, t] : E) {
        if (s == t) continue;
        if (s > t) std::swap(s, t);
        if (t != N - 1) {
            L[s][t]--;
            L[t][s]--;
            L[t][t]++;
        }
        L[s][s]++;
    }
    if (is_prime_mod) {
        return L.det(); // O(N^3)
    } else {
        return L.det_arbitrary_mod(); // O(N^3 log(mod))
    }
}

// 有向グラフのRを根とする全域木の個数
template<typename mint, bool is_prime_mod>
mint matrix_tree_theorem_directed(int N, int R, const std::vector<std::pair<int, int>> &E) {
    if (N <= 1) return 1;
    matrix_mod<mint> L(N - 1, N - 1, 0);
    
    for (auto [s, t] : E) {
        if (s == t) continue;
        if (t != R) {
            L[t - (t > R)][t - (t > R)]++;
            if (s != R) {
                L[t - (t > R)][s - (s > R)]--;
            }
        }
    }
    if (is_prime_mod) {
        return L.det(); // O(N^3)
    } else {
        return L.det_arbitrary_mod(); // O(N^3 log(mod))
    }
}
#endif
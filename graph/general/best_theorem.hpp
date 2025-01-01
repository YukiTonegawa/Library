#ifndef _BEST_THEOREM_H_
#define _BEST_THEOREM_H_
#include <vector>
#include "../tree/matrix_tree_theorem.hpp"
#include "../../math/modint/modint32_static.hpp"
#include "eulerian_trail.hpp"

template<typename mint, typename edge = int>
mint best_theorem(int N, const std::vector<std::pair<int, edge>> &E) {
    int M = E.size();
    auto [f, e] = eulerian_trail_directed<int>(csr<int>(N, E));
    if (!f || e[0].first != e.back().second) return 0;
    int s = e[0].first;
    std::vector<int> outdeg(N, 0);
    for (int i = 0; i < M; i++) {
        int a = E[i].first;
        outdeg[a]++;
    }
    auto Etmp = E;
    for (int i = 0; i < N; i++) {
        if (outdeg[i] == 0) {
            Etmp.push_back({s, i});
        }
    }
    auto C = matrix_tree_theorem_directed<mint, true>(N, s, Etmp);
    std::vector<mint> fac(M);
    fac[0] = 1;
    for (int i = 1; i < M; i++) fac[i] = fac[i - 1] * i;
    mint ans = C;
    for (int i = 0; i < N; i++) {
        if (outdeg[i]) {
            ans *= fac[outdeg[i] - 1];
        }
    }
    return ans;
}
#endif
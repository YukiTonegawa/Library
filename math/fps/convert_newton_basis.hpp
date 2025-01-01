#ifndef _CONVERT_NEWTON_BASIS_H_
#define _CONVERT_NEWTON_BASIS_H_
#include <vector>
#include "../base/bit_ceil.hpp"

template<typename FPS>
std::vector<typename FPS::mint_t> convert_newton_basis(const FPS &f, const std::vector<typename FPS::mint_t> &P) {
    using mint_t = typename FPS::mint_t;
    int N = f.size();
    assert(P.size() == N);
    int log = bit_ceil_log(N), size = 1 << log;
    std::vector<FPS> tree(2 * size, {1});
    for (int i = 0; i < N; i++) tree[i + size] = {-P[i], 1};
    for (int i = size - 1; i > 1; i--) tree[i] = tree[i * 2] * tree[i * 2 + 1];
    tree[1] = f;
    for (int i = 1; i < size; i++) {
        auto [Q, R] = tree[i].division_polynomial(tree[i * 2]);
        tree[i * 2] = R;
        tree[i * 2 + 1] = Q;
    }
    std::vector<mint_t> ans(N);
    for (int i = 0; i < N; i++) ans[i] = tree[i + size][0];
    return ans;
}

#endif
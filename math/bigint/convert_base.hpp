#ifndef _CONVERT_BASE_H_
#define _CONVERT_BASE_H_
#include "bigint.hpp"

// 10進数->n進数 O(Nlog^2N)
std::vector<bigint> to_n_ary(bigint x, bigint n) {
    std::vector<bigint> powtable{bigint(1LL), n};
    while (x >= powtable.back()) {
        powtable.push_back(powtable.back().square());
    }
    powtable.pop_back();
    int log = powtable.size() - 1;
    if (log == 0) return {x};
    int size = 1 << log;
    std::vector<bigint> tree(2 * size);
    tree[1] = x;
    for (int i = 1; i < size; i++) {
        int d = log - (31 - __builtin_clz(i));
        std::tie(tree[i * 2], tree[i * 2 + 1]) = tree[i].div(powtable[d]);
        tree[i] = bigint();
    }
    return std::vector<bigint>(tree.begin() + size, tree.begin() + 2 * size);
}
// n進数->10進数 O(Nlog^2N)
bigint to_decimal(const std::vector<bigint> &x, bigint n) {
    if (x.empty()) return 0;
    if (x.size() == 1) return x[0];
    int log = bit_ceil_log(x.size());
    std::vector<bigint> powtable{bigint(1LL), n};
    while (powtable.size() <= log) {
        powtable.push_back(powtable.back().square());
    }
    int size = 1 << log, N = x.size();
    std::vector<bigint> tree(2 * size, bigint(0));
    for (int i = 0; i < N; i++) tree[2 * size - 1 - i] = x[N - 1 - i];
    for (int i = size - 1; i > 0; i--) {
        int d = log - (31 - __builtin_clz(i));
        tree[i] = tree[i * 2] * powtable[d] + tree[i * 2 + 1];
        tree[i * 2] = tree[i * 2 + 1] = bigint();
    }
    return tree[1];
}
#endif
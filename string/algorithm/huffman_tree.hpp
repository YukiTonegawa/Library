#ifndef _HUFFMAN_TREE_H_
#define _HUFFMAN_TREE_H_
#include <vector>
#include <string>
#include <algorithm>
#include "../../data_structure/basic/binary_heap.hpp"

// n - 1個の追加ノードを含むグラフを構築
// 根は2 * n - 2, [0, n)は葉
// ans[i] := {左ノード, 右ノード} 葉の場合{-1, -1}
// ∑{0 <= i < n} v[i] * depth[i]が最小になる

// 全ての要素が1以上でハフマン木を構築したとき, 高さkのノードの値はフィボナッチ数列のk項目以上
// -> 高さがO(log(N * 値の最大値))
template<typename Val>
std::vector<std::pair<int, int>> huffman_tree(const std::vector<Val> &v) {
    int N = v.size();
    assert(N);
    std::vector<std::pair<Val, int>> tmp(N);
    for (int i = 0; i < N; i++) {
        assert(v[i] > 0); // 0を許容することもできるが許容しない場合と比べて結果が変わるので注意
        tmp[i] = {v[i], i};
    }
    binary_heap_map<Val, int> h(tmp);
    std::vector<std::pair<int, int>> ans(2 * N - 1, {-1, -1});
    while (h.size() >= 2) {
        auto [x, xi] = h.pop_min();
        auto [y, yi] = h.pop_min();
        ans[N] = {yi, xi};
        h.push({x + y, N++});
    }
    return ans;
}

// ハフマン符号, ∑長さ×重み　が最小
template<typename Val>
std::vector<std::string> huffman_code(const std::vector<Val> &v) {
    int N = v.size();
    assert(N);
    if (N == 1) return {"0"};
    auto t = huffman_tree<Val>(v);
    std::vector<std::string> ans(2 * N - 1, "");
    for (int i = 2 * N - 2; i >= N; i--) {
        ans[t[i].first] = ans[i] + '0';
        ans[t[i].second] = ans[i] + '1';
    }
    ans.resize(N);
    return ans;
}

#endif
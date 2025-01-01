#ifndef _KRUSKAL_H_
#define _KRUSKAL_H_
#include "../base/csr.hpp"
#include "../../data_structure/union_find/union_find.hpp"
#include <vector>
#include <queue>

// mst
// クラスカル法
template<typename edge, typename W = typename edge::W>
std::vector<std::pair<int, edge>> kruskal(int N, std::vector<std::pair<int, edge>> E) {
    std::sort(E.begin(), E.end(), [](const std::pair<int, edge> &a, const std::pair<int, edge> &b) {
        return a.second.weight() < b.second.weight();
    });
    std::vector<std::pair<int, edge>> res;
    union_find uf(N);
    for (auto [s, e] : E) {
        int to = e;
        if (uf.unite(s, to)) {
            res.push_back({s, e});
        }
    }
    return res;
}
#endif
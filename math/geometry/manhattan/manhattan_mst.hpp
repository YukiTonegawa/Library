#ifndef _MANHATTAN_MST_H_
#define _MANHATTAN_MST_H_

#include <vector>
#include <tuple>
#include "manhattan_closest_point.hpp"
#include "../../../data_structure/union_find/union_find.hpp"

template<typename T>
std::vector<std::tuple<T, int, int>> manhattan_mst(const std::vector<std::pair<T, T>> &_P) {
    auto E = manhattan_closest_point<T>(_P);
    int N = _P.size();
    union_find uf(N);
    std::sort(E.begin(), E.end());
    std::vector<std::tuple<T, int, int>> res;
    for (auto [d, a, b] : E) {
        if (uf.unite(a, b)) {
            res.push_back({d, a, b});
        }
    }
    return res;
}
#endif
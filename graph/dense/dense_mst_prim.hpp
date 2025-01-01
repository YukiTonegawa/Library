#ifndef _DENSE_MST_PRIM_H_
#define _DENSE_MST_PRIM_H_
#include "../base/adjacency_matrix.hpp"
#include <vector>

template<typename Mat>
std::vector<std::pair<int, int>> dense_mst_prim(const Mat &W) {
    int N = W.size();
    if (N <= 1) return {};
    auto W0 = W[0];
    std::vector<int> S(N, 0);
    std::vector<bool> same(N, false);
    std::vector<std::pair<int, int>> ans;
    for (int s = 1; s < N; s++) {
        int i = 1;
        while (same[i]) i++;
        auto minw = W0[i];
        int mini = i;
        while (i < N) {
            if (!same[i] && W0[i] < minw) {
                minw = W0[i];
                mini = i;
            }
            i++;
        }
        ans.push_back({S[mini], mini});
        for (int j = 1; j < N; j++) {
            if (W0[j] > W[mini][j]) {
                W0[j] = W[mini][j];
                S[j] = mini;
            }
        }
    }
    return ans;
};

#endif
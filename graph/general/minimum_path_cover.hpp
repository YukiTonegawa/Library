#ifndef _MINIMUM_PATH_COVER_H_
#define _MINIMUM_PATH_COVER_H_
#include <vector>
#include "../flow/bipartite_matching.hpp"

// 各頂点が丁度1つのパスに含まれる
// パスの個数の最小値
// s->tパスが存在するなら辺(s, t)を追加すると条件を1つ以上のパスに置き換えられる
struct minimum_path_cover_dag {
    int N;
    bipartite_matching g;

    minimum_path_cover_dag(int N) : N(N), g(N, N) {}
    
    void add_edge(int s, int t) {
        if (s != t) g.add_edge(s, t);
    }

    // K個のパス
    std::vector<std::vector<int>> solve() {
        std::vector<std::vector<int>> res;
        auto [m, next] = g.flow();
        std::vector<int> prev(N, -1);
        for (int i = 0; i < N; i++) {
            if (next[i] == -1) continue;
            prev[next[i]] = i;
        }
        for (int i = 0; i < N; i++) {
            if (prev[i] != -1) continue;
            int j = i;
            res.push_back({});
            while (j != -1) {
                res.back().push_back(j);
                j = next[j];
            }
            std::reverse(res.back().begin(), res.back().end());
        }
        return res;
    }
};

#endif
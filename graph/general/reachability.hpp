#ifndef _REACHABILITY_H_
#define _REACHABILITY_H_
#include <bitset>
#include <vector>
#include <unordered_set>
#include "scc.hpp"

// O(NM/w)
template<int maxN>
struct reachability_dag {
    using Bit = std::bitset<maxN>;
    
    std::array<std::unordered_set<int>, maxN> G;
    std::array<Bit, maxN> R;

    reachability_dag() { R.fill(Bit(0)); }
    
    void add_edge(int s, int t) {
        G[s].insert(t);
    }

    void build() {
        auto dfs = [&](auto &&dfs, int v) -> void {
            R[v][v] = 1;
            for (int j : G[v]) {
                if (!R[j][j]) dfs(dfs, j);
                R[v] |= R[j];
            }
        };

        for (int i = 0; i < maxN; i++) {
            if (!R[i][i]) dfs(dfs, i);
        }
    }

    bool reachable(int s, int t) {
        return R[s][t];
    }
};

template<int maxN>
struct reachability {
    using Bit = std::bitset<maxN>;

    scc g;
    std::vector<int> cmp;
    std::vector<std::pair<int, int>> E;
    reachability_dag<maxN> R;

    reachability() : g(maxN) {}
    
    void add_edge(int s, int t) {
        g.add_edge(s, t);
        E.push_back({s, t});
    }

    void build() {
        int M;
        std::tie(M, cmp) = g.scc_ids();
        for (auto [s, t] : E) {
            if (cmp[s] == cmp[t]) continue;
            R.add_edge(cmp[s], cmp[t]);
        }
        R.build();
    }

    bool reachable(int s, int t) {
        return R[cmp[s]][cmp[t]];
    }
};
#endif
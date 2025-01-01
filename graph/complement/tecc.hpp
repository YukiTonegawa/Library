#ifndef _COMPLEMENT_TECC_H_
#define _COMPLEMENT_TECC_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include <unordered_set>
#include "../../data_structure/ordered_set/nazoki.hpp"
#include "../../data_structure/segtree/segtree.hpp"

struct tecc_complement_graph {
  private:
    int _n;
    std::unordered_set<long long> E;
    std::vector<std::vector<int>> G;

    static constexpr int _e() { return std::numeric_limits<int>::max(); }
    static constexpr int _op(int a, int b) { return std::min(a, b); }

    bool find(int a, int b) {
        if (a > b) std::swap(a, b);
        return E.find(((long long)a << 32) + b) != E.end();
    }

  public:
    explicit tecc_complement_graph(int n) : _n(n), G(_n) {}

    int num_vertices() { return _n; }

    // 多重辺は無視される
    void add_edge(int a, int b) { 
        if (a > b) std::swap(a, b);
        E.insert(((long long)a << 32) + b);
        G[a].push_back(b);
        G[b].push_back(a);
    }

    // @return pair of (# of tecc, tecc id)
    std::pair<int, std::vector<int>> tecc_ids() {
        int now_ord = 0, group_num = 0;
        std::vector<int> visited, low(_n), ord(_n, -1), ids(_n);
        visited.reserve(_n);
        nazotree<4> S;
        for (int i = 0; i < _n; i++) S.insert(i);
        for (int i = 0; i < _n; i++) {
            G[i].push_back(i);
            std::sort(G[i].begin(), G[i].end());
        }
        segtree<int, _op, _e> seg(_n);
        auto dfs = [&](auto self, int v) -> void {
            low[v] = ord[v] = now_ord++;
            seg.set(v, ord[v]);
            visited.push_back(v);
            for (int i = 0;; ) {
                int to = S.ge(i);
                if (to == -1) break;
                if (!find(v, to)) {
                    S.erase(to);
                    self(self, to);
                    low[v] = std::min(low[v], low[to]);
                }
                i = to + 1;
            }
            int m = G[v].size();
            for (int i = 0; i <= m; i++) {
                int l = (i == 0 ? 0 : G[v][i - 1] + 1);
                int r = (i == m ? _n : G[v][i]);
                if (l > r) continue;
                low[v] = std::min(low[v], seg.prod(l, r));
            }
            if (low[v] == ord[v]) {
                while (true) {
                    int u = visited.back();
                    visited.pop_back();
                    ord[u] = _n;
                    seg.set(u, _n);
                    ids[u] = group_num;
                    if (u == v) break;
                }
                group_num++;
            }
        };
        for (int i = 0; i < _n; i++) {
            if (ord[i] == -1) {
                S.erase(i);
                dfs(dfs, i);
            }
        }
        for (auto& x : ids) {
            x = group_num - 1 - x;
        }
        return {group_num, ids};
    }

    // グループごと
    std::vector<std::vector<int>> tecc_group() {
        auto ids = tecc_ids();
        int group_num = ids.first;
        std::vector<int> counts(group_num);
        for (auto x : ids.second) counts[x]++;
        std::vector<std::vector<int>> groups(ids.first);
        for (int i = 0; i < group_num; i++) {
            groups[i].reserve(counts[i]);
        }
        for (int i = 0; i < _n; i++) {
            groups[ids.second[i]].push_back(i);
        }
        return groups;
    }
};
#endif
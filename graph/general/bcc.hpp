#ifndef _BCC_H_
#define _BCC_H_
#include "../base/csr.hpp"

struct bcc {
    int _n;
    std::vector<std::pair<int, int>> edges;
    
    explicit bcc(int n = 0) : _n(n) {}

    int num_vertices() { return _n; }

    void add_edge(int from, int to) { 
        edges.push_back({from, to});
        edges.push_back({to, from});
    }

    std::vector<bool> cut;
    std::vector<std::vector<int>> group; // 各成分の要素

    // cut_vertex以外はただ一つの成分に属する
    // 各辺はただ一つの成分に属する
    void build() {
        auto g = csr<int>(_n, edges);
        std::vector<int> child(_n, 0), dep(_n, -1), low(_n), tmp_e;
        cut = std::vector<bool>(_n, false);
        group = std::vector<std::vector<int>>{};
        auto dfs = [&](auto &&dfs, int v, int p) -> void {
            if (p != -1) child[p]++;
            dep[v] = low[v] = (p == -1 ? 0 : dep[p] + 1);
            for (int i = g.begin(v); i < g.end(v); i++) {
                int to = g.elist[i];
                if (to == p) continue;
                if (dep[to] == -1) {
                    tmp_e.push_back(i);
                    dfs(dfs, to, v);
                    if (low[to] >= dep[v]) {
                        cut[v] = true;
                        group.push_back({v});
                        while (!tmp_e.empty()) {
                            int eid = tmp_e.back();
                            tmp_e.pop_back();
                            int t = g.elist[eid];
                            group.back().push_back(t);
                            if (eid == i) break;
                        }
                    }
                    low[v] = std::min(low[v], low[to]);
                }
                low[v] = std::min(low[v], dep[to]);
            }
        };
        for (int i = 0; i < _n; i++) {
            if (dep[i] != -1) continue;
            int vsz_pre = group.size();
            dfs(dfs, i, -1);
            cut[i] = (child[i] > 1);
            if (group.size() == vsz_pre) group.push_back({i}); // 孤立点
        }
    }

    // 頂点vが属するグループ達
    // 辺eが属するグループ
    void build_detail() {
        //
    }
};
#endif
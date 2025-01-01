#ifndef _ST_NUMBERING_H_
#define _ST_NUMBERING_H_
#include "../base/csr.hpp"
#include <vector>
#include <algorithm>
#include <cassert>

// トポソ順の決め方であって、辺を向き付けした後に
// 任意のvに対してs->v->tのパスが存在するもの
struct st_numbering {
    int _n, _s, _t;
    std::vector<std::pair<int, int>> edges;
    std::vector<int> ord;
    
    explicit st_numbering(int n, int s, int t) : _n(n), _s(s), _t(t), ord(n, -1) {
        assert(s != t); // s = tの場合トポソの定義よりtに戻ってこれない
        assert(0 <= s && s < n);
        assert(0 <= t && t < n);
        edges.push_back({s, t});
    }

    int num_vertices() { return _n; }

    // 自己辺, 多重辺OK
    void add_edge(int from, int to) {
        if (from == to) return;
        edges.push_back({from, to});
        edges.push_back({to, from});
    }

    // 必要十分条件 : s-t辺を追加したグラフで全頂点が同じbiconnected component
    bool build() {
        if (_n == 1) {
            ord[_s] = 0;
            return _s == _t;
        }
        // n = 2の場合のみs-t辺を追加する事で非連結なグラフがbiconnectedになる可能性がある
        if (_n == 2) {
            ord[_s] = 0;
            ord[_t] = 1;
            return edges.size() >= 2;
        }
        auto g = csr<int>(_n, edges);
        std::vector<int> par(_n, -1), pre(_n, -1), low(_n), tmp_e;
        std::vector<int> tour;
        std::vector<std::vector<int>> group;

        int o = 0;
        auto dfs = [&](auto &&dfs, int v, int p) -> void {
            tour.push_back(v);
            par[v] = p;
            pre[v] = low[v] = o++;
            for (int i = g.begin(v); i < g.end(v); i++) {
                int to = g.elist[i];
                if (to == p) continue;
                if (pre[to] == -1) {
                    tmp_e.push_back(i);
                    dfs(dfs, to, v);
                    if (low[to] >= pre[v]) {
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
                low[v] = std::min(low[v], pre[to]);
            }
        };
        dfs(dfs, _s, -1);
        int size = (group.empty() ? 0 : (int)group[0].size());
        if (size != _n) return false;
        
        std::vector<int> next(_n, -1), prev(_n, -1), sign(_n, -1);

        next[_s] = _t;
        prev[_t] = _s;
        for (int i = 2; i < _n; i++) {
            int v = tour[i];
            int lowv = tour[low[v]];
            int parv = par[v];
            if (sign[lowv] == -1) {
                int p = prev[parv];
                next[p] = v;
                next[v] = parv;
                prev[v] = p;
                prev[parv] = v;
            } else {
                int n = next[parv];
                prev[n] = v;
                prev[v] = parv;
                next[v] = n;
                next[parv] = v;
            }
            sign[parv] = -sign[lowv];
        }
        ord[_s] = 0;
        for (int i = 1, s = _s; i < _n; i++) {
            s = next[s];
            ord[s] = i;
        }
        return true;
    }
};
#endif
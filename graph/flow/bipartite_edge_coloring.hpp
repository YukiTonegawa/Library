#ifndef _BIPARTITE_EDGE_COLORING_H_
#define _BIPARTITE_EDGE_COLORING_H_
#include <cassert>
#include <algorithm>
#include <vector>
#include <queue>
#include "hopcroft_karp.hpp"
#include "../../data_structure/union_find/union_find.hpp"

struct bipartite_edge_coloring {
    struct edge { int s, t, id; };
    std::vector<bool> eused;

    std::vector<edge> eulerian_trail_forest(int N, const std::vector<edge> &E) {
        std::vector<std::vector<edge>> G(N);
        std::vector<int> deg(N, 0);
        for (auto e : E) {
            G[e.s].push_back(e);
            G[e.t].push_back({e.t, e.s, e.id});
            deg[e.s]++;
            deg[e.t]++;
        }
        std::vector<edge> res;
        std::vector<int> pos = deg;
        auto dfs = [&](auto &&dfs, int v) -> void {
            while (deg[v]) {
                edge e = G[v][--pos[v]];
                int id = e.id;
                if (eused[id]) continue;
                deg[v]--;
                deg[e.t]--;
                eused[id] = 1;
                dfs(dfs, e.t);
                res.push_back(e);
            }
        };
        for (int i = 0; i < N; i++) dfs(dfs, i);
        for (auto e : res) eused[e.id] = 0;
        return res;
    }
    std::vector<edge> _E;
    int N, M, D;
    bipartite_edge_coloring(int _N, int _M) : N(_N), M(_M) {}

    int add_edge(int s, int t) {
        assert(0 <= s && s < N);
        assert(0 <= t && t < M);
        int id = _E.size();
        _E.push_back({s, N + t, id});
        return id;
    }

    void to_regular() {
        int max_deg = 0;
        std::vector<int> deg(N + M, 0);
        for (auto e : _E) {
            deg[e.s]++;
            deg[e.t]++;
            max_deg = std::max(max_deg, deg[e.s]);
            max_deg = std::max(max_deg, deg[e.t]);
        }
        union_find ufL(N), ufR(M);

        using pi = std::pair<int, int>;
        std::priority_queue<pi, std::vector<pi>, std::greater<pi>> pqL, pqR;
        for (int i = 0; i < N; i++) pqL.push({deg[i], i});
        for (int i = 0; i < M; i++) pqR.push({deg[i + N], i});
        int cmpL = N, cmpR = M;
        while (pqL.size() >= 2) {
            auto [sza, a] = pqL.top();
            pqL.pop();
            auto [szb, b] = pqL.top();
            if (sza + szb <= max_deg) {
                cmpL--;
                pqL.pop();
                ufL.unite(a, b);
                pqL.push({sza + szb, ufL.find(a)});
            } else {
                pqL.push({sza, a});
                break;
            }
        }
        while (pqR.size() >= 2) {
            auto [sza, a] = pqR.top();
            pqR.pop();
            auto [szb, b] = pqR.top();
            if (sza + szb <= max_deg) {
                cmpR--;
                pqR.pop();
                ufR.unite(a, b);
                pqR.push({sza + szb, ufR.find(a)});
            } else {
                pqR.push({sza, a});
                break;
            }
        }
        std::vector<int> rootL, rootR;
        for (int i = 0; i < N; i++) {
            if (i == ufL.find(i)) {
                rootL.push_back(i);
            }
        }
        for (int i = 0; i < M; i++) {
            if (i == ufR.find(i)) {
                rootR.push_back(i);
            }
        }
        D = max_deg;
        int M = std::max(cmpL, cmpR);
        deg = std::vector<int>(2 * std::max(cmpL, cmpR), 0);
        for (auto &e : _E) {
            e.s = ufL.find(e.s);
            e.t = ufR.find(e.t - N);
            e.s = std::lower_bound(rootL.begin(), rootL.end(), e.s) - rootL.begin();
            e.t = std::lower_bound(rootR.begin(), rootR.end(), e.t) - rootR.begin() + M;
            deg[e.s]++;
            deg[e.t]++;
        }
        N = M;
        int j = N;
        int eid = _E.size();
        for (int i = 0; i < N; i++) {
            while (deg[i] < max_deg) {
                while (deg[j] == max_deg) j++;
                _E.push_back({i, j, eid++});
                deg[i]++;
                deg[j]++;
            }
        }
    }

    std::pair<int, std::vector<int>> solve() {
        int ecnt = _E.size();
        to_regular();
        std::vector<int> ans(_E.size(), -1);
        eused.resize(_E.size(), 0);
        int C = 0;

        auto dfs = [&](auto &&dfs, std::vector<edge> &E, int d) -> void {
            if (d == 0) return;
            if (d & 1) {
                hopcroft_karp G(N, N);
                for (auto e : E) G.add_edge(e.s, e.t - N);
                G.max_flow();
                auto pairL = G.pairL();
                std::vector<edge> Etmp;
                for (auto e : E) {
                    if (pairL[e.s] == e.t - N) {
                        ans[e.id] = C;
                        pairL[e.s] = -1;
                    } else {
                        Etmp.push_back(e);
                    }
                }
                C++;
                dfs(dfs, Etmp, d - 1);
                return;
            }
            std::vector<edge> L, R;
            {                
                auto P = eulerian_trail_forest(2 * N, E);
                for (int i = 0; i < P.size(); i++) {
                    if (P[i].s >= N) std::swap(P[i].s, P[i].t);
                    if (i % 2 == 0) {
                        L.push_back(P[i]);
                    } else {
                        R.push_back(P[i]);
                    }
                }
            }
            dfs(dfs, L, d / 2);
            dfs(dfs, R, d / 2);
        };
        dfs(dfs, _E, D);
        ans.resize(ecnt);
        return {C, ans};
    }
};

#endif
#ifndef _SCC_INCREMENTAL_H_
#define _SCC_INCREMENTAL_H_
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cassert>
#include "../base/csr.hpp"
#include "scc.hpp"
#include "../../data_structure/persistent/partial_persistent_union_find.hpp"

struct incremental_scc {
    static constexpr int inf_time = std::numeric_limits<int>::max();
    int N;
    std::vector<std::pair<int, int>> edges;
    std::vector<int> T; // T[i] := 辺iが存在し,2端点が同じ成分になる最小時刻 (そうならない場合はinf_time) [1, M] or inf_time
    std::vector<int> sT; // ソート済のT
    partial_persistent_union_find uf;

    incremental_scc(int N) : N(N), uf(N) {}

    void add_edge(int from, int to) { edges.push_back({from, {to}}); }

    void build() {
        int M = edges.size();
        if (!M) return;
        T = std::vector<int>(M, inf_time);
        std::vector<int> idx(N, -1);

        auto f = [&](auto &&f, int l, int r, std::vector<std::tuple<int, int, int>> &E) -> void {
            int mid = (l + r) / 2;
            std::vector<std::tuple<int, int, int>> EL, ER;
            {
                scc g(0);
                int types = 0;
                for (auto [id, u, v] : E) {
                    if (idx[u] == -1) idx[u] = types++;
                    if (idx[v] == -1) idx[v] = types++;
                    if (r - l == 1 || id < mid) {
                        g.add_edge(idx[u], idx[v]);
                    }
                }
                g._n = types;
                auto group = g.scc_ids().second;
                for (auto [id, u, v] : E) {
                    int u2 = idx[u], v2 = idx[v];
                    if (group[u2] == group[v2]) {
                        if (r - l == 1) T[id] = l + 1;
                        else if (id >= mid) T[id] = id + 1;
                        else EL.push_back({id, u, v});
                    } else {
                        ER.push_back({id, group[u2], group[v2]});
                    }
                }
                for (auto [id, u, v] : E) idx[u] = idx[v] = -1;
                E.clear();
                E.shrink_to_fit();
            }
            if (r - l == 1) return;
            if (!EL.empty()) f(f, l, mid, EL);
            if (!ER.empty()) f(f, mid, r, ER);
        };

        std::vector<std::tuple<int, int, int>> Etmp(M);
        for (int i = 0; i < M; i++) {
            Etmp[i] = {i, edges[i].first, edges[i].second};
        }
        f(f, 0, M, Etmp);
        std::vector<std::tuple<int, int, int>> e;
        for (int i = 0; i < edges.size(); i++) {
            e.push_back({T[i], edges[i].first, edges[i].second});
        }
        std::sort(e.begin(), e.end());
        for (auto [t, a, b] : e) {
            uf.unite(a, b);
            sT.push_back(t);
        }
    }

    // 時刻iを辺がi本ある状態とする
    // 頂点a, bが同じ強連結成分になる最小時刻 (ならない場合はinf_time)
    int same_scc_time(int a, int b) {
        int t = uf.connect_time(a, b);
        if (t == -1) return inf_time;
        if (t == 0) return 0;
        return sT[t - 1];
    }

    // 時刻tにa, bが同じ強連結成分にあるか
    bool same_scc(int a, int b, int t) {
        int ct = same_scc_time(a, b);
        return ct != inf_time && ct <= t;
    }

    // 時刻tでのvのcmp
    int id_scc(int v, int t) {
        t = std::upper_bound(sT.begin(), sT.end(), t) - sT.begin();
        return uf.find(v, t).first;
    }

    // 時刻tでのvのsccのサイズ
    int size_scc(int v, int t) {
        t = std::upper_bound(sT.begin(), sT.end(), t) - sT.begin();
        return uf.size(v, t);
    }
};
#endif
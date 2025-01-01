#ifndef _PRIM_H_
#define _PRIM_H_
#include "../base/csr.hpp"
#include <vector>
#include <queue>

// mst
// プリム法, 連結なら始点sは関係ない
template<typename edge, typename W = typename edge::W>
std::vector<std::pair<int, edge>> prim(const csr<edge> &G, int st = 0) {
    int N = G.N();
    std::vector<bool> used(N, 0);
    std::vector<std::pair<int, edge>> E;
    using P = std::tuple<W, int, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    used[st] = true;
    for (int i = G.begin(st); i < G.end(st); i++) {
        pq.push({G.elist[i].weight(), st, i});
    }
    while (!pq.empty()) {
        auto [d, s, eid] = pq.top();
        pq.pop();
        int to = G.elist[eid];
        if (used[to]) continue;
        used[to] = true;
        E.push_back({s, G.elist[eid]});
        for (int i = G.begin(to); i < G.end(to); i++) {
            int toto = G.elist[i];
            if (!used[toto]) {
                pq.push({G.elist[i].weight(), to, i});
            }
        }
    }
    return E;
}
#endif
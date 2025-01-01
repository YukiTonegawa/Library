#ifndef _BIPARTITE_MAXIMUM_CLOSURE_H_
#define _BIPARTITE_MAXIMUM_CLOSURE_H_
#include "dinic.hpp"
#include "hopcroft_karp.hpp"

// N頂点のうちK個がtrueの場合スコア -K
// Q[i] = {a0, a1, a2, ...} の全てがtrueだとスコア +1
// 最大スコアと頂点の情報
std::pair<int, std::vector<bool>> bipartite_maximum_closure(int N, const std::vector<std::vector<int>> &Q) {
    std::vector<bool> res(N, false);
    int M = Q.size();
    hopcroft_karp g(M, N);
    for (int q = 0; q < M; q++) {
        for (int v : Q[q]) {
            assert(0 <= v && v < N);
            g.add_edge(q, v);
        }
    }
    int score = M - g.max_flow();
    //auto f = g.mincut(); 未実装
    //for (int i = 0; i < N; i++) res[i] = f[i + M];
    return {score, res};
}

// N頂点があり頂点iがtrueがと スコア -cost[i]
// Q[i] = {si, {a0, a1, a2, ...}} の全てがtrueだとスコア +si
// 最大スコアと頂点の情報
template<typename T>
std::pair<T, std::vector<bool>> bipartite_weighted_maximum_closure(const std::vector<T> &cost, const std::vector<std::pair<T, std::vector<int>>> &Q) {
    static constexpr T inf = std::numeric_limits<T>::max();
    int N = cost.size(), M = Q.size();
    std::vector<bool> res(N, false);
    dinic<T> g(M + N + 2);
    int s = M + N, t = s + 1;
    T score = 0;
    for (int q = 0; q < M; q++) {
        score += Q[q].first;
        g.add_edge(s, q, Q[q].first);
        for (int v : Q[q].second) {
            assert(0 <= v && v < N);
            g.add_edge(q, M + v, inf);
        }
    }
    for (int v = 0; v < N; v++) {
        g.add_edge(M + v, t, cost[v]);
    }
    score -= g.max_flow(s, t);
    auto f = g.min_cut(s);
    for (int i = 0; i < N; i++) res[i] = f[i + M];
    return {score, res};
}
#endif
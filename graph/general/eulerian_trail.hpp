#ifndef _EULERIAN_TRAIL_H_
#define _EULERIAN_TRAIL_H_
#include "../base/csr.hpp"

// 必要条件 : 入次数と出次数の異なる頂点が0個または(2個かつその頂点の間に辺を1本足すと0個にできる)
// 連結でない場合不可能
template<typename edge>
std::pair<bool, std::vector<std::pair<int, edge>>> eulerian_trail_directed(const csr<edge> &G) {
    int N = G.N();
    std::vector<int> in(N, 0), out(N);
    for(int i = 0; i < N; i++) out[i] = G.deg(i);
    for (int i = G.begin(0); i < G.end(N - 1); i++) {
        int to = G.elist[i];
        in[to]++;
    }
    int s = -1;
    for (int i = 0; i < N; i++) {
        if (s == -1 && out[i]) s = i;
        if (out[i] > in[i]) s = i;
    }
    std::vector<std::pair<int, edge>> res;
    auto dfs = [&](auto &&dfs, int v) -> void {
        while (out[v]) {
            edge e = G.elist[G.begin(v) + (--out[v])];
            int to = e;
            in[to]--;
            dfs(dfs, to);
            res.push_back({v, e});
        }
    };
    dfs(dfs, s);
    std::reverse(res.begin(), res.end());
    for (int i = 0; i < N; i++) if (in[i] || out[i]) return {false, {}};
    for (int i = 1; i < res.size(); i++) if (int(res[i - 1].second) != res[i].first) return {false, {}};
    return {true, res};
}


// 必要十分条件 : 連結かつ奇次数の頂点が2個か0個
// 無向グラフのときは辺にユニークなidを割り振って逆流しないようにする
template<typename edge>
std::pair<bool, std::vector<std::pair<int, edge>>> eulerian_trail_undirected(const csr<edge> &G) {
    int N = G.N(), degsum = 0;
    std::vector<int> deg(N, 0);
    for (int i = 0; i < N; i++) {
        deg[i] = G.deg(i);
        degsum += deg[i];
    }
    int s = -1;
    for (int i = 0; i < N; i++) {
        if (s == -1 && deg[i]) s = i;
        if (deg[i] & 1) s = i;
    }
    std::vector<bool> used_edge(degsum, 0);
    std::vector<std::pair<int, edge>> res;
    std::vector<int> pos(N);
    for (int i = 0; i < N; i++) pos[i] = deg[i];
    auto dfs = [&](auto &&dfs, int v) -> void {
        while (deg[v]) {
            edge e = G.elist[G.begin(v) + (--pos[v])];
            int to = e;
            int id = e.weight();
            if (used_edge[id]) continue;
            deg[v]--;
            deg[to]--;
            used_edge[id] = 1;
            dfs(dfs, to);
            res.push_back({v, e});
        }
    };
    dfs(dfs, s);
    std::reverse(res.begin(), res.end());
    for(int i = 0; i < N; i++) if (deg[i]) return {false, {}};
    for(int i = 1; i < res.size(); i++) if(int(res[i - 1].second) != res[i].first) return {false, {}};
    return {true, res};
}
#endif
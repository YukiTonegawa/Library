#ifndef _CYCLE_DETECTION_MINCOST_H_
#define _CYCLE_DETECTION_MINCOST_H_
#include <queue>
#include <algorithm>
#include <cassert>
#include <vector>
#include <numeric>
#include <random>
#include "../base/edge.hpp"
#include "../base/csr.hpp"

template<typename edge>
struct cycle_detection_mincost {
    using W = typename edge::W;
    static constexpr W inf = std::numeric_limits<W>::max() / 2;
  private:
    csr<edge> g;

    std::pair<std::vector<W>, std::vector<edge>> bfs_tree(int s, W curw = inf) {
        int N = g.N();
        std::vector<W> dist(N, inf);
        std::vector<edge> par(N);
        std::queue<int> q;
        q.push(s);
        dist[s] = 0;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int i = g.begin(v); i < g.end(v); i++) {
                int to = g[i];
                if (dist[v] + 1 < dist[to] && dist[v] + 1 < curw) {
                    dist[to] = dist[v] + 1;
                    par[to] = g[i];
                    q.push(to);
                }
            }
        }
        return {dist, par};
    }
    
    std::pair<std::vector<W>, std::vector<edge>> dijkstra_tree(int s, W curw = inf) {
        int N = g.N();
        std::vector<W> dist(N, inf);
        std::vector<edge> par(N);
        using P = std::pair<W, int>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> q;
        q.push({0, s});
        dist[s] = 0;
        while (!q.empty()) {
            auto [d, v] = q.top();
            q.pop();
            if (dist[v] != d) continue;
            for (int i = g.begin(v); i < g.end(v); i++) {
                int to = g[i];
                W nd = dist[v] + g[i].weight();
                if (nd < dist[to] && nd < curw) {
                    dist[to] = nd;
                    par[to] = g[i];
                    q.push({nd, to});
                }
            }
        }
        return {dist, par};
    }

  public:
    cycle_detection_mincost(const csr<edge> &_g) : g(_g) {}

    // vを含む最小閉路
    // 無向グラフの場合のみ辺のs-tが逆になっている場合があるがidの順序は正しい
    // 重みあり O((V + E)logV)
    // 重みなし O(V + E)
    std::pair<W, std::vector<edge>> find(int v, bool directed, bool weighted, bool accept_self_loop) {
        int N = g.N();
        W w = inf;
        int eid = -1;
        if (accept_self_loop) {
            for (int j = g.begin(v); j < g.end(v); j++) {
                if (g[j].to() == v && w < g[j].weight()) {
                    w = g[j].weight();
                    eid = j;
                }
            }
        }
        std::vector<W> dist;
        std::vector<edge> par;
        std::vector<int> semi_root;

        if (weighted) {
            std::tie(dist, par) = dijkstra_tree(v);
        } else {
            std::tie(dist, par) = bfs_tree(v);
        }
        if (directed) {
            for (int i = 0; i < N; i++) {
                if (i == v || dist[i] >= w) continue;
                for (int j = g.begin(i); j < g.end(i); j++) {
                    if (g[j].to() == v && dist[i] + g[j].weight() < w) {
                        w = dist[i] + g[j].weight();
                        eid = j;
                    }
                }
            }
        } else {
            semi_root.resize(N, -1);
            auto dfs = [&](auto &&dfs, int u, int p, int sr) -> void {
                semi_root[u] = sr;
                for (int i = g.begin(u); i < g.end(u); i++) {
                    int to = g[i];
                    if (to == v || to == p || to == u || dist[to] >= w) continue;
                    if (par[to].id() == g[i].id()) {
                        dfs(dfs, to, u, sr);
                    }
                }
            };
            for (int i = 0; i < N; i++) {
                if (i == v || dist[i] >= w) continue;
                if (par[i].from() == v) dfs(dfs, i, v, i);
            }
            for (int i = 0; i < N; i++) {
                if (i == v || dist[i] >= w) continue;
                for (int j = g.begin(i); j < g.end(i); j++) {
                    int to = g[j];
                    if (g[j].id() == par[i].id()) continue;
                    if (semi_root[i] == semi_root[to]) continue;
                    if (dist[i] + dist[to] + g[j].weight() < w) {
                        w = dist[i] + dist[to] + g[j].weight();
                        eid = j;
                    }
                }
            }
        }
        if (w == inf) return {inf, {}};
        std::vector<edge> res;
        res.push_back(g[eid]);
        int u = g[eid].from();
        while (u != v) {
            res.push_back(par[u]);
            u = par[u].from();
        }
        std::reverse(res.begin(), res.end());
        if (!directed) {
            u = g[eid].to();
            while (u != v) {
                res.push_back(par[u]);
                u = par[u].from();
            }
        }
        return {w, res};
    }

    // 最小閉路
    // 無向グラフの場合のみ辺のs-tが逆になっている場合があるがidの順序は正しい
    // 重みあり O((V + E)logV)
    // 重みなし O(V + E)
    std::pair<W, std::vector<edge>> find(bool directed, bool weighted, bool accept_self_loop) {
        int N = g.N();
        W w = inf;
        int r = -1, eid = -1;
        if (accept_self_loop) {
            for (int v = 0; v < N; v++) {
                for (int j = g.begin(v); j < g.end(v); j++) {
                    if (g[j].to() == v && w < g[j].weight()) {
                        w = g[j].weight();
                        r = v;
                        eid = j;
                    }
                }
            }
        }
        std::vector<W> dist;
        std::vector<edge> par;
        std::vector<int> semi_root(N, -1);
        std::vector<int> ord(N);
        std::iota(ord.begin(), ord.end(), 0);
        std::random_device seed_gen;
        std::mt19937 engine(seed_gen());
        std::shuffle(ord.begin(), ord.end(), engine);

        for (int v : ord) {
            if (weighted) {
                std::tie(dist, par) = dijkstra_tree(v, w);
            } else {
                std::tie(dist, par) = bfs_tree(v, w);
            }
            if (directed) {
                for (int i = 0; i < N; i++) {
                    if (i == v || dist[i] >= w) continue;
                    for (int j = g.begin(i); j < g.end(i); j++) {
                        if (g[j].to() == v && dist[i] + g[j].weight() < w) {
                            w = dist[i] + g[j].weight();
                            eid = j;
                            r = v;
                        }
                    }
                }
            } else {
                std::fill(semi_root.begin(), semi_root.end(), -1);
                auto dfs = [&](auto &&dfs, int u, int p, int sr) -> void {
                    semi_root[u] = sr;
                    for (int i = g.begin(u); i < g.end(u); i++) {
                        int to = g[i];
                        if (to == v || to == p || to == u || dist[to] >= w) continue;
                        if (par[to].id() == g[i].id()) {
                            dfs(dfs, to, u, sr);
                        }
                    }
                };
                for (int i = 0; i < N; i++) {
                    if (i == v || dist[i] >= w) continue;
                    if (par[i].from() == v) dfs(dfs, i, v, i);
                }
                for (int i = 0; i < N; i++) {
                    if (i == v || dist[i] >= w) continue;
                    for (int j = g.begin(i); j < g.end(i); j++) {
                        int to = g[j];
                        if (g[j].id() == par[i].id()) continue;
                        if (semi_root[i] == semi_root[to]) continue;
                        if (dist[i] + dist[to] + g[j].weight() < w) {
                            w = dist[i] + dist[to] + g[j].weight();
                            r = v;
                            eid = j;
                        }
                    }
                }
            }
        }        
        if (w == inf) return {inf, {}};
        if (weighted) {
            std::tie(dist, par) = dijkstra_tree(r);
        } else {
            std::tie(dist, par) = bfs_tree(r);
        }
        std::vector<edge> res;
        res.push_back(g[eid]);
        int u = g[eid].from();
        while (u != r) {
            res.push_back(par[u]);
            u = par[u].from();
        }
        std::reverse(res.begin(), res.end());
        if (!directed) {
            u = g[eid].to();
            while (u != r) {
                res.push_back(par[u]);
                u = par[u].from();
            }
        }
        return {w, res};
    }
};
#endif
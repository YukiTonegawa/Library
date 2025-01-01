#ifndef _MINCOSF_FLOW_H_
#define _MINCOSF_FLOW_H_
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>
#include <queue>
#include "../base/simple_queue.hpp"

template <class F, class C>
struct primal_dual {
  private:
    int N;
    struct _edge {
        int t, rev;
        F cap;
        C cost;
    };
    std::vector<std::pair<int, int>> pos;
    std::vector<std::vector<_edge>> g;

  public:
    primal_dual() {}
    primal_dual(int n) : N(n), g(n) {}

    int add_edge(int s, int t, F cap, C cost) {
        assert(0 <= s && s < N);
        assert(0 <= t && t < N);
        int m = int(pos.size());
        pos.push_back({s, int(g[s].size())});
        g[s].push_back(_edge{t, int(g[t].size()), cap, cost});
        g[t].push_back(_edge{s, int(g[s].size()) - 1, 0, -cost});
        return m;
    }

    struct edge {
        int s, t;
        F cap, flow;
        C cost;
    };

    edge get_edge(int i) {
        int m = int(pos.size());
        assert(0 <= i && i < m);
        auto _e = g[pos[i].first][pos[i].second];
        auto _re = g[_e.t][_e.rev];
        return edge{pos[i].first, _e.t, _e.cap + _re.cap, _re.cap, _e.cost};
    }

    std::vector<edge> edges() {
        int m = int(pos.size());
        std::vector<edge> result(m);
        for (int i = 0; i < m; i++) {
            result[i] = get_edge(i);
        }
        return result;
    }

    std::pair<F, C> flow(int s, int t) {
        return flow(s, t, std::numeric_limits<F>::max());
    }

    std::pair<F, C> flow(int s, int t, F flow_limit) {
        return slope(s, t, flow_limit).back();
    }

    // f : 流量 -> コストの折れ線
    std::vector<std::pair<F, C>> slope(int s, int t) {
        return slope(s, t, std::numeric_limits<F>::max());
    }

    // f : 流量 -> コストの折れ線
    // negative_edge_mode :
    // 0 : 何もしない 全てのコストが非負         O(F(N + M)log(N + M))
    // 1 : bellman_ford 負の閉路があると壊れる  O(F(N + M)log(N + M)) + O(NM)
    // 2 : DAG                              O(F(N + M)log(N + M)) + O(N + M)
    std::vector<std::pair<F, C>> slope(int s, int t, F flow_limit, int negative_edge_mode = 0) {
        assert(0 <= s && s < N);
        assert(0 <= t && t < N);
        assert(s != t);
        std::vector<C> dual(N, 0), dist(N);
        std::vector<int> pv(N), pe(N);
        std::vector<bool> vis(N);
        static constexpr C inf = std::numeric_limits<C>::max() / 2;

        auto init_potential_bf = [&]() {
            std::fill(dist.begin(), dist.end(), inf);
            dist[s] = 0;
            for (int T = 0; T < N - 1; T++) {
                bool updated = false;
                for (int v = 0; v < N; v++) {
                    if (dist[v] == inf) continue;
                    for (int i = 0; i < int(g[v].size()); i++) {
                        auto e = g[v][i];
                        if (!e.cap) continue;
                        C cost = e.cost;
                        if (dist[e.t] - dist[v] > cost) {
                            dist[e.t] = dist[v] + cost;
                            updated = true;
                        }
                    }
                }
                if (!updated) break;
            }
            if (dist[t] == inf) return false;
            for (int v = 0; v < N; v++) {
                if (dist[v] == inf) continue;
                dual[v] -= dist[t] - dist[v];
            }
            return true;
        };

        auto init_potential_dag = [&]() {
            std::fill(dist.begin(), dist.end(), inf);
            std::vector<int> deg(N, 0);
            for (int v = 0; v < N; v++) {
                for (int i = 0; i < int(g[v].size()); i++) {
                    auto e = g[v][i];
                    if (!e.cap) continue;
                    deg[e.t]++;
                }
            }
            int l = 0, r = 0;
            std::vector<int> que(N);
            for (int v = 0; v < N; v++) {
                if (deg[v] == 0) {
                    que[r++] = v;
                }
            }
            dist[s] = 0;
            while (l < r) {
                int v = que[l++];
                for (int i = 0; i < int(g[v].size()); i++) {
                    auto e = g[v][i];
                    if (!e.cap) continue;
                    C cost = e.cost;
                    if (dist[v] != inf && dist[e.t] - dist[v] > cost) {
                        dist[e.t] = dist[v] + cost;
                    }
                    if (--deg[e.t] == 0) {
                        que[r++] = e.t;
                    }
                }
            }
            if (dist[t] == inf) return false;
            for (int v = 0; v < N; v++) {
                if (dist[v] == inf) continue;
                dual[v] -= dist[t] - dist[v];
            }
            return true;
        };

        auto dual_ref = [&]() {
            std::fill(dist.begin(), dist.end(), inf);
            std::fill(pv.begin(), pv.end(), -1);
            std::fill(pe.begin(), pe.end(), -1);
            std::fill(vis.begin(), vis.end(), false);
            struct Q {
                C key;
                int t;
                bool operator<(Q r) const { return key > r.key; }
            };
            std::priority_queue<Q> que;
            dist[s] = 0;
            que.push(Q{0, s});
            while (!que.empty()) {
                int v = que.top().t;
                que.pop();
                if (vis[v]) continue;
                vis[v] = true;
                if (v == t) break;
                for (int i = 0; i < int(g[v].size()); i++) {
                    auto e = g[v][i];
                    if (vis[e.t] || !e.cap) continue;
                    C cost = e.cost - dual[e.t] + dual[v];
                    //std::cout << "!" << cost << '\n';
                    if (dist[e.t] - dist[v] > cost) {
                        dist[e.t] = dist[v] + cost;
                        pv[e.t] = v;
                        pe[e.t] = i;
                        que.push(Q{dist[e.t], e.t});
                    }
                }
            }
            if (!vis[t]) return false;
            for (int v = 0; v < N; v++) {
                if (!vis[v]) continue;
                dual[v] -= dist[t] - dist[v];
            }
            return true;
        };
        F flow = 0;
        C cost = 0, prev_cost = -1;
        std::vector<std::pair<F, C>> result;
        result.push_back({flow, cost});
        if (negative_edge_mode == 1 && !init_potential_bf()) return result;
        if (negative_edge_mode == 2 && !init_potential_dag()) return result;
        while (flow < flow_limit) {
            if (!dual_ref()) break;
            F c = flow_limit - flow;
            for (int v = t; v != s; v = pv[v]) {
                c = std::min(c, g[pv[v]][pe[v]].cap);
            }
            for (int v = t; v != s; v = pv[v]) {
                auto& e = g[pv[v]][pe[v]];
                e.cap -= c;
                g[v][e.rev].cap += c;
            }
            C d = -dual[s];
            flow += c;
            cost += c * d;
            if (prev_cost == d) result.pop_back();
            result.push_back({flow, cost});
            prev_cost = cost;
        }
        return result;
    }
};
#endif
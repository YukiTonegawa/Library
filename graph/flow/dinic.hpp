#ifndef _DINIC_H_
#define _DINIC_H_
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>

template <typename F>
struct dinic {
    private:
    int N;
    struct _edge {
        int t, rev;
        F cap;
    };
    std::vector<std::pair<int, int>> pos;
    std::vector<std::vector<_edge>> G;
    
  public:
    dinic() : N(0) {}
    dinic(int n) : N(n), G(n) {}

    int add_vertex() {
        G.push_back(std::vector<_edge>());
        return N++;
    }

    int add_edge(int s, int t, F cap) {
        assert(0 <= s && s < N);
        assert(0 <= t && t < N);
        assert(0 <= cap);
        int m = int(pos.size());
        pos.push_back({s, int(G[s].size())});
        G[s].push_back(_edge{t, int(G[t].size()), cap});
        G[t].push_back(_edge{s, int(G[s].size()) - 1, 0});
        return m;
    }

    struct edge {
        int s, t;
        F cap, flow;
    };

    edge get_edge(int i) {
        int m = int(pos.size());
        assert(0 <= i && i < m);
        auto _e = G[pos[i].first][pos[i].second];
        auto _re = G[_e.t][_e.rev];
        return edge{pos[i].first, _e.t, _e.cap + _re.cap, _re.cap};
    }

    std::vector<edge> edges() {
        int M = int(pos.size());
        std::vector<edge> result;
        for (int i = 0; i < M; i++) result.push_back(get_edge(i));
        return result;
    }

    void change_edge(int i, F new_cap, F new_flow) {
        int m = int(pos.size());
        assert(0 <= i && i < m);
        assert(0 <= new_flow && new_flow <= new_cap);
        auto& _e = G[pos[i].first][pos[i].second];
        auto& _re = G[_e.t][_e.rev];
        _e.cap = new_cap - new_flow;
        _re.cap = new_flow;
    }

    F max_flow(int s, int t) { return max_flow(s, t, std::numeric_limits<F>::max()); }
    F max_flow(int s, int t, F flow_limit) {
        assert(0 <= s && s < N);
        assert(0 <= t && t < N);
        std::vector<int> level(N), iter(N), que(N);
        auto bfs = [&]() {
            std::fill(level.begin(), level.end(), -1);
            level[s] = 0;
            que[0] = s;
            int l = 0, r = 1;
            while (l < r) {
                int v = que[l++];
                for (auto e : G[v]) {
                    if (e.cap == 0 || level[e.t] >= 0) continue;
                    level[e.t] = level[v] + 1;
                    if (e.t == t) return;
                    que[r++] = e.t;
                }
            }
        };
        auto dfs = [&](auto self, int v, F up) {
            if (v == s) return up;
            F res = 0;
            int level_v = level[v];
            for (int& i = iter[v]; i < int(G[v].size()); i++) {
                _edge& e = G[v][i];
                if (level_v <= level[e.t] || G[e.t][e.rev].cap == 0) continue;
                F d = self(self, e.t, std::min(up - res, G[e.t][e.rev].cap));
                if (d <= 0) continue;
                G[v][i].cap += d;
                G[e.t][e.rev].cap -= d;
                res += d;
                if (res == up) break;
            }
            return res;
        };

        F flow = 0;
        while (flow < flow_limit) {
            bfs();
            if (level[t] == -1) break;
            std::fill(iter.begin(), iter.end(), 0);
            while (flow < flow_limit) {
                F f = dfs(dfs, t, flow_limit - flow);
                if (!f) break;
                flow += f;
            }
        }
        return flow;
    }
    
    // s側の頂点かどうか (max_flowを先に呼ぶ)
    std::vector<bool> min_cut(int s) {
        std::vector<bool> visited(N);
        std::vector<int> que(N);
        que[0] = s;
        int l = 0, r = 1;
        while (l < r) {
            int p = que[l++];
            visited[p] = true;
            for (auto e : G[p]) {
                if (e.cap && !visited[e.t]) {
                    visited[e.t] = true;
                    que[r++] = e.t;
                }
            }
        }
        return visited;
    }
};
#endif
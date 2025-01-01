#ifndef _HOPCROFT_KARP_H_
#define _HOPCROFT_KARP_H_
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>

struct hopcroft_karp {
  private:
    int N, M;
    std::vector<std::pair<int, int>> info;
    std::vector<std::vector<int>> G;

    std::vector<int> matchL, matchR;

  public:
    hopcroft_karp() : N(0), M(0) {}
    hopcroft_karp(int n, int m) : N(n), M(m), G(n), matchL(N, -1), matchR(M, -1) {}

    int add_edge(int s, int t) {
        assert(0 <= s && s < N);
        assert(0 <= t && t < M);
        int m = int(info.size());
        info.push_back({s, t});
        G[s].push_back(t);
        return m;
    }
    struct edge {
        int s, t;
        bool used;
    };

    edge get_edge(int i) {
        int m = int(info.size());
        assert(0 <= i && i < m);
        auto [s, t] = info[i];
        return edge{s, t, matchL[s] == t};
    }

    std::vector<edge> edges() {
        int M = int(info.size());
        std::vector<edge> result;
        for (int i = 0; i < M; i++) result.push_back(get_edge(i));
        return result;
    }

    int max_flow() { return max_flow(std::numeric_limits<int>::max()); }
    int max_flow(int flow_limit) {
        std::vector<int> level(N), iter(N), que(N);
        auto bfs = [&]() {
            std::fill(level.begin(), level.end(), -1);
            int l = 0, r = 0;
            for (int i = 0; i < N; i++) {
                if (matchL[i] == -1) {
                    level[i] = 0;
                    que[r++] = i;
                }
            }
            while (l < r) {
                int v = que[l++];
                for (int t : G[v]) {
                    int tt = matchR[t];
                    if (tt == -1 || level[tt] != -1) continue;
                    level[tt] = level[v] + 1;
                    que[r++] = tt;
                }
            }
        };
        auto dfs = [&](auto self, int v) -> bool {
            int level_v = level[v];
            for (int& i = iter[v]; i < int(G[v].size()); i++) {
                int t = G[v][i];
                int tt = matchR[t];
                if (tt == -1 || (level[tt] > level_v && self(self, tt))) {
                    matchR[t] = v;
                    matchL[v] = t;
                    return true;
                }
            }
            return false;
        };

        int flow = 0;
        while (flow < flow_limit) {
            bfs();
            std::fill(iter.begin(), iter.end(), 0);
            bool ok = false;
            for (int i = 0; i < N && flow < flow_limit; i++) {
                if (matchL[i] != -1) continue;
                bool f = dfs(dfs, i);
                flow += f;
                ok |= f;
            }
            if (!ok) break;
        }
        return flow;
    }

    std::vector<int> pairL() const { return matchL; }
    std::vector<int> pairR() const { return matchR; }
    std::vector<std::pair<int, int>> pairs() const {
        std::vector<std::pair<int, int>> res;
        for (int i = 0; i < N; i++) {
            if (matchL[i] != -1) {
                res.push_back({i, matchL[i]});
            }
        }
        return res;
    }

    // 点カバー (= max_flow) (max_flowを先に呼ぶ)
    // {0, i} : 左のi {1, i} := 右のi
    std::vector<std::pair<bool, int>> min_vertex_cover() {
        std::vector<bool> visitedL(N, false), visitedR(M, false);
        auto dfs = [&](auto self, int v) {
            visitedL[v] = true;
            if (matchL[v] != -1) return; 
            for (int t : G[v]) {
                visitedR[t] = true;
                if (matchR[t] == -1 || visitedL[matchR[t]]) continue;
                self(self, matchR[t]);
            }
        };
        for (int i = 0; i < N; i++) {
            if (matchL[i] == -1 && !visitedL[i]) {
                dfs(dfs, i);
            }
        }
        std::vector<std::pair<bool, int>> res;
        for (int i = 0; i < N; i++) {
            if (!visitedL[i]) res.push_back({0, i});
        }
        for (int i = 0; i < M; i++) {
            if (visitedR[i]) res.push_back({1, i});
        }
        return res;
    }

    // {i, j} : 左のiと右のj
    // 孤立点がない場合 辺カバー = N + M - max_flow (max_flowを先に呼ぶ)
    std::vector<std::pair<int, int>> min_edge_cover() {
        std::vector<std::pair<int, int>> res;
        std::vector<int> revR(M, -1);
        for (int i = 0; i < N; i++) {
            if (matchL[i] != -1) {
                res.push_back({i, matchL[i]});
                for (int t : G[i]) revR[t] = i;
            }
        }
        for (int i = 0; i < N; i++) {
            if (matchL[i] == -1) {
                assert(!G[i].empty()); // Liが孤立点
                res.push_back({i, G[i][0]});
            }
        }
        for (int i = 0; i < M; i++) {
            if (matchR[i] == -1) {
                assert(revR[i] != -1); // Riが孤立点
                res.push_back({revR[i], i});
            }
        }
        return res;
    }

    // 最大独立集合 (= (N + M) - max_flow) (max_flowを先に呼ぶ)
    // {0, i} : 左のi {1, i} := 右のi
    std::vector<std::pair<bool, int>> max_independent_set() {
        std::vector<bool> okL(N, true), okR(M, true);
        for (auto [s, v] : min_vertex_cover()) {
            if (!s) okL[v] = false;
            else okR[v] = false;
        }
        std::vector<std::pair<bool, int>> res;
        for (int i = 0; i < N; i++) {
            if (okL[i]) res.push_back({0, i});
        }
        for (int i = 0; i < M; i++) {
            if (okR[i]) res.push_back({1, i});
        }
        return res;
    }
    /*
    // res[i] := s側か (0 <= i <= N + M)
    std::vector<bool> min_cut() {
        std::vector<bool> visited(N + M, false);
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
    */
};
#endif
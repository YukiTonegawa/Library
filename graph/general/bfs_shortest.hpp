#ifndef _BFS_SHORTEST_H_
#define _BFS_SHORTEST_H_
#include "../base/csr.hpp"
#include <vector>
#include <queue>

// O(V + E)
// 辺の重みが1
template<typename edge>
struct bfs_shortest {
  private:
    int _s;
    csr<edge> &G;
  
  public:
    static constexpr int inf = std::numeric_limits<int>::max();
    bfs_shortest() {}
    bfs_shortest(csr<edge> &G) : G(G) {}
    std::vector<int> dist, par;

    // targetを指定した場合targetに到達した時点で打ち切る
    void build(int s, int target = -1) {
        _s = s;
        using P = std::pair<int, int>;
        int N = G.N();
        if (dist.empty()) {
            dist.resize(N, inf);
            par.resize(N, -1);
        } else {
            std::fill(dist.begin(), dist.end(), inf);
            std::fill(par.begin(), par.end(), -1);
        }
        std::queue<P> que;
        dist[s] = 0;
        que.push({0, s});
        while (!que.empty()) {
            auto [w, v] = que.front();
            if (v == target) break;
            que.pop();
            if (dist[v] < w) continue;
            for (int i = G.begin(v); i < G.end(v); i++) {
                int to = G.elist[i];
                int d = dist[v] + 1;
                if (dist[to] > d) {
                    dist[to] = d;
                    par[to] = v;
                    que.push(P{d, to});
                }
            }
        }
    }
    
    // 到達不可能な場合空
    std::vector<int> path(int t) {
        assert(!dist.empty());
        if (par[t] == -1) return {};
        std::vector<int> res{t};
        while (t != _s) {
            t = par[t];
            res.push_back(t);
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    int operator [](int v) const { return dist[v]; }
};
#endif
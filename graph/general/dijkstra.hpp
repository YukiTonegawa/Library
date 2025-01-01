#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_
#include "../base/csr.hpp"
#include <vector>
#include <queue>

// O((V + E)logV)
// 辺の重みが非負(負の閉路がなければ一応動く)
template<typename edge, typename W = typename edge::W>
struct dijkstra {
  private:
    int _s;
    csr<edge> &G;
  
  public:
    static constexpr W inf = std::numeric_limits<W>::max() / 2;
    dijkstra() {}
    dijkstra(csr<edge> &G) : G(G) {}
    std::vector<W> dist;
    std::vector<int> par;

    // targetを指定した場合targetに到達した時点で打ち切る
    void build(int s, int target = -1) {
        _s = s;
        using P = std::pair<W, int>;
        int N = G.N();
        if (dist.empty()) {
            dist.resize(N, inf);
            par.resize(N, -1);
        } else {
            std::fill(dist.begin(), dist.end(), inf);
            std::fill(par.begin(), par.end(), -1);
        }
        std::priority_queue<P, std::vector<P>, std::greater<P>> que;
        dist[s] = 0;
        que.push({0, s});
        while (!que.empty()) {
            auto [w, v] = que.top();
            if (v == target) break;
            que.pop();
            if (dist[v] < w) continue;
            for (int i = G.begin(v); i < G.end(v); i++) {
                int to = G.elist[i];
                W d = dist[v] + G.elist[i].weight();
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

    W operator [](int v) const { return dist[v]; }
};
#endif
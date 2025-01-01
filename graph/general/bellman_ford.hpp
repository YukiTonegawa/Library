#ifndef _BELLMAN_FORD_H_
#define _BELLMAN_FORD_H_
#include "../base/csr.hpp"

// O(VE)
// inf: 到達不可, minf: 負の閉路
template<typename edge, typename W = typename edge::W>
struct bellman_ford {
  private:
    using P = std::pair<W, int>;
    csr<edge> &G;
    int _s;
  
  public:
    bellman_ford() {}
    bellman_ford(csr<edge> &G) : G(G) {}
    static constexpr W inf = std::numeric_limits<W>::max() / 2;
    static constexpr W minf = std::numeric_limits<W>::min() / 2;

    std::vector<W> dist;
    std::vector<int> par;

    void build(int s) {
        _s = s;
        int N = G.N();
        if (dist.empty()) {
            dist.resize(N, inf);
            par.resize(N, -1);
        } else {
            std::fill(dist.begin(), dist.end(), inf);
            std::fill(par.begin(), par.end(), -1);
        }
        dist[s] = 0;
        for (int lp = 0; ; lp++) {
            bool update = false;
            for (int i = 0; i < N; i++) {
                if (dist[i] == inf) continue;
                for (int j = G.begin(i); j < G.end(j); j++) {
                    int to = G.elist[j];
                    if (dist[i] == minf) {
                        if (dist[to] != minf) update = true;
                        dist[to] = minf;
                    } else if (dist[to] > dist[i] + G.elist[j].weight()) {
                        dist[to] = (lp > N ? minf : dist[i] + G.elist[j].weight());
                        par[to] = i;
                        update = true;
                    }
                }
            }
            if (!update) return;
        }
    }

    // 最短路がない(inf, -inf)場合空
    std::vector<int> path(int t) {
        assert(!dist.empty());
        if (dist[t] == inf || dist[t] == minf) return {};
        std::vector<int> res{t};
        while (t != _s) {
            t = par[t];
            res.push_back(t);
        }
        std::reverse(res.begin(), res.end());
        return res;
    }
    
    W operator [] (int v) const { return dist[v]; }
};
#endif
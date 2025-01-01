#ifndef _WARSHALL_H_
#define _WARSHALL_H_
#include "../base/csr.hpp"
#include <vector>
#include <queue>

// O(N^3)
template<typename edge, typename W = typename edge::W>
struct warshall_floyd {
  private:
    csr<edge> &G;
  
  public:
    static constexpr W inf = std::numeric_limits<W>::max() / 2;
    warshall_floyd() {}
    warshall_floyd(csr<edge> &G) : G(G) {}
    std::vector<std::vector<W>> dist;
    std::vector<std::vector<int>> par;
    
    void build() {
        int N = G.N();
        dist.resize(N, std::vector<W>(N, inf));
        par.resize(N, std::vector<int>(N, -1));
        
        for (int i = 0; i < N; i++) {
            dist[i][i] = 0;
            par[i][i] = i;
            for (int j = G.begin(i); j < G.end(j); j++) {
                int to = G.elist[j];
                W w = G.elist[j].weight();
                if (dist[i][to] > w) {
                    dist[i][to] = w;
                    par[i][to] = i;
                }
            }
        }
        for (int k = 0; k < N; k++) {
            for (int s = 0; s < N; s++) {
                for (int t = 0; t < N; t++) {
                    if (dist[s][t] > dist[s][k] + dist[k][t]) {
                        dist[s][t] = dist[s][k] + dist[k][t];
                        par[s][t] = par[k][t];
                    }
                }
            }
        }
    }
    
    // infの場合空
    std::vector<int> path(int s, int t) {
        if (dist[s][t] == inf) return {};
        std::vector<int> res{t};
        while (t != s) {
            t = par[s][t];
            res.push_back(t);
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    const std::vecor<W>& operator [](int v) { return dist[v]; }
};
#endif
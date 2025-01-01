#ifndef _CENTROID_H_
#define _CENTROID_H_
#include "../base/csr.hpp"

// 重心を再帰的に繋いだ木を作る
template<typename edge = int>
struct centroid {
    int N;
    csr<int> G;
    std::vector<int> sz, dep, par;
    
    centroid() : N(0) {}
    centroid(const csr<edge> &g) : N(g.N()), sz(N, 0), dep(N, N), par(N, -1) {
        std::vector<std::pair<int, int>> E;
        auto add_edge = [&](int p, int c) -> void {
            E.push_back({p, c});
            par[c] = p;
        };
        auto dfs = [&](auto &&dfs, int v, int p, const int M, const int rank) -> std::pair<int, int> {
            int size = 1;
            for (int i = g.begin(v); i < g.end(v); i++) { 
                int to = g.elist[i];
                if (to == p || dep[to] < rank) continue;
                auto [sz_c, cent_c] = dfs(dfs, to, v, M, rank);
                if (sz_c == -1) return {-1, cent_c};
                sz[to] = sz_c;
                size += sz_c;
            }
            if(size * 2 >= M) {
                sz[v] = M;
                dep[v] = rank;
                for (int i = g.begin(v); i < g.end(v); i++) {
                    int to = g.elist[i];
                    if (to == p || dep[to] < rank) continue;
                    auto [sz_c, cent_c] = dfs(dfs, to, -1, sz[to], rank + 1);
                    add_edge(v, cent_c);
                }
                if (p != -1) {
                    auto [sz_c, cent_c] = dfs(dfs, p, -1, M - size, rank + 1);
                    add_edge(v, cent_c);
                }
                return {-1, v};
            }
            return {size, -1};
        };
        dfs(dfs, 0, -1, N, 0);
        G = csr<int>(N, E);
    }

    // 元の木のパスが重心木でa, bを含む場合lca(a, b)も含む
    // ランク最小の頂点は1個
    // s-tパスのランク最小の頂点はlca(s, t)
    int lca(int a, int b) {
        if (dep[a] > dep[b]) std::swap(a, b);
        while (dep[a] < dep[b]) b = par[b];
        while (a != b) {
            a = par[a];
            b = par[b];
        }
        return a;
    }
};
#endif
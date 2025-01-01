#ifndef _EULER_TOUR_LCA_H_
#define _EULER_TOUR_LCA_H_
#include "../base/csr.hpp"
#include "../../data_structure/sparse_table/sparse_table_efficient.hpp"

template<typename edge>
struct euler_tour_lca {
    int N;
    std::vector<int> sz, dep, par;
    std::vector<int> in, out;
    sparse_table_rmq<long long> st;
    euler_tour_lca() {}
    euler_tour_lca(int root, const csr<edge> &G) : N(G.N()), sz(N), dep(N), par(N), in(N), out(N) {
        std::vector<long long> val(2 * N);
        int pos = 0;

        auto dfs = [&](auto &&dfs, int v, int p) -> void {
            sz[v] = 1;
            dep[v] = (p == -1 ? -1 : dep[p]) + 1;
            par[v] = p;
            val[pos] = ((long long)dep[v] << 30) + v;
            in[v] = out[v] = pos++;
            for (int i = G.begin(v); i < G.end(v); i++) {
                int to = G.elist[i];
                if (to == p) continue;
                dfs(dfs, to, v);
                sz[v] += sz[to];
                val[pos] = val[in[v]];
                out[v] = pos++;
            }
        };
        dfs(dfs, root, -1);
        st = sparse_table_rmq<long long>(val);
    }

    int lca(int a, int b) const {
        int l = std::min(in[a], in[b]);
        int r = std::max(out[a], out[b]);
        return st.prod(l, r + 1) & ((1LL << 30) - 1);
    }
    
    int dist(int a, int b) const {
        int l = lca(a, b);
        return dep[a] + dep[b] - 2 * dep[l];
    }
};
#endif
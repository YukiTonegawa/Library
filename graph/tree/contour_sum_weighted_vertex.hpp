#ifndef _CONTOUR_SUM_W_V_H_
#define _CONTOUR_SUM_W_V_H_
#include "hld.hpp"
#include "centroid.hpp"
#include "../../data_structure/fenwick_tree/fenwick_tree_sparse.hpp"

template<typename T, typename W, typename edge = int>
struct contour_sum_weighted_vertex {
    hld<edge> H;
    centroid<edge> C;
    std::vector<int> O;
    std::vector<std::vector<W>> D;
    std::vector<fenwick_tree_sparse<W, T>> FT;
    std::vector<std::vector<fenwick_tree_sparse<W, T>>> FTc;

    contour_sum_weighted_vertex() {}
    contour_sum_weighted_vertex(const hld<edge> &_H, const centroid<edge> &_C, const std::vector<T> &X, const std::vector<W> &_W) : H(_H), C(_C), O(C.N), D(C.N), FT(C.N), FTc(C.N) {
        int N = C.N;
        int root = -1;
        for (int i = 0; i < N; i++) {
            if (C.par[i] == -1) {
                root = i;
                break;
            }
        }
        for (int i = 0; i < N; i++) {
            D[i].push_back(0);
            for (int j = C.G.begin(i); j < C.G.end(i); j++) {
                int k = C.G.elist[j];
                O[k] = j - C.G.begin(i);
            }
        }

        std::vector<W> wsum(N);
        auto dfs_w = [&](auto &&dfs_w, int v, int p) -> void {
            wsum[v] = _W[v] + (p == -1 ? 0 : wsum[p]);
            for (int i = H.G.begin(v); i < H.G.end(v); i++) {
                int to = H.G.elist[i];
                if (to == p) continue;
                dfs_w(dfs_w, to, v);
            }
        };
        dfs_w(dfs_w, H.root, -1);

        auto dfs = [&](auto &&dfs, int v) -> std::vector<int> {
            std::vector<int> res{v};
            std::vector<std::pair<W, T>> tmp{{_W[v], X[v]}};
            FTc[v].resize(C.G.end(v) - C.G.begin(v));
            for (int i = C.G.begin(v); i < C.G.end(v); i++) {
                int c = C.G.elist[i];
                auto V = dfs(dfs, c);
                std::vector<std::pair<W, T>> tmpc;
                for (int c : V) {
                    int L = H.lca(c, v);
                    W dist1 = wsum[c] + wsum[v] - 2 * wsum[L] + _W[L];
                    W dist2 = dist1 - _W[v];
                    tmp.push_back({dist1, X[c]});
                    tmpc.push_back({dist1, X[c]});
                    D[c].push_back(dist2);
                }
                if (res.size() < V.size()) res.swap(V);
                res.insert(res.end(), V.begin(), V.end());
                std::sort(tmpc.begin(), tmpc.end());
                FTc[v][i - C.G.begin(v)] = fenwick_tree_sparse<W, T>(tmpc);
            }
            std::sort(tmp.begin(), tmp.end());
            FT[v] = fenwick_tree_sparse<W, T>(tmp);
            return res;
        };
        dfs(dfs, root);
    }

    void add(int v, T x) {
        int p = v, c = -1;
        for (int i = 0; i < D[v].size(); i++, c = p, p = C.par[p]) {
            W dist = D[v][i];
            FT[p].apply(dist, x);
            if (i) FTc[p][O[c]].apply(dist, x);
        }
    }

    // vとの距離がl以上r未満の頂点の重みの和
    T sum(int v, W l, W r) {
        T res = 0;
        int p = v, c = -1;
        for (int i = 0; i < D[v].size(); i++, c = p, p = C.par[p]) {
            W dist = D[v][i];
            res += FT[p].prod(l - dist, r - dist);
            if (i) {
                int ord = O[c];
                res -= FTc[p][ord].prod(l - dist, r - dist);
            }
        }
        return res;
    }
};
#endif
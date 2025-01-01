#ifndef _CONTOUR_SUM_H_
#define _CONTOUR_SUM_H_
#include "hld.hpp"
#include "centroid.hpp"
#include "../../data_structure/segtree/segtree.hpp"

/*
template<typename T, T (*op)(T, T), T (*e)(), typename edge = int>
struct contour_prod {
    hld<edge> H;
    centroid<edge> C;
    std::vector<int> O;
    std::vector<std::vector<int>> D;

    //std::vector<segtree<T, op, e>> ST;
    //std::vector<std::vector<fenwick_tree<T>>> FTc;

    contour_prod() {}
    contour_prod(const hld<edge> &_H, const centroid<edge> &_C, const std::vector<T> &X) : H(_H), C(_C), O(C.N), D(C.N) {
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
        auto dfs = [&](auto &&dfs, int v) -> std::vector<int> {
            std::vector<int> res{v};
            std::vector<T> tmp{X[v]};
            FTc[v].resize(C.G.end(v) - C.G.begin(v));
            for (int i = C.G.begin(v); i < C.G.end(v); i++) {
                int c = C.G.elist[i];
                auto V = dfs(dfs, c);
                std::vector<T> tmpc;
                for (int c : V) {
                    int dist = H.dist(c, v);
                    D[c].push_back(dist);
                    if (tmp.size() <= dist) tmp.resize(dist + 1, 0);
                    if (tmpc.size() <= dist) tmpc.resize(dist + 1, 0);
                    tmp[dist] += X[c];
                    tmpc[dist] += X[c];
                }
                if (res.size() < V.size()) res.swap(V);
                res.insert(res.end(), V.begin(), V.end());
                //FTc[v][i - C.G.begin(v)] = fenwick_tree<T>(tmpc);
            }
            //FT[v] = fenwick_tree<T>(tmp);
            return res;
        };
        dfs(dfs, root);
    }

    void add(int v, T x) {
        int p = v, c = -1;
        for (int i = 0; i < D[v].size(); i++, c = p, p = C.par[p]) {
            int dist = D[v][i];

            //FT[p].apply(dist, x);
            //if (i) FTc[p][O[c]].apply(dist, x);
        }
    }

    // vとの距離がl以上r未満の頂点の重みの和
    T sum(int v, int l, int r) {
        T res = 0;
        int p = v, c = -1;
        for (int i = 0; i < D[v].size(); i++, c = p, p = C.par[p]) {
            int dist = D[v][i];
            int L = std::max(l - dist, 0);
            {
                int R = std::min(FT[p].size(), r - dist);
                if (L < R) res += FT[p].prod(L, R);
            }
            if (i) {
                int ord = O[c];
                int R = std::min(FTc[p][ord].size(), r - dist);
                if (L < R) res -= FTc[p][ord].prod(L, R);
            }
        }
        return res;
    }
};
*/
#endif
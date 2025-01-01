#ifndef _CONTOUR_ADD_H_
#define _CONTOUR_ADD_H_
#include "hld.hpp"
#include "centroid.hpp"
#include "../../data_structure/fenwick_tree/fenwick_tree.hpp"

template<typename T, typename edge = int>
struct contour_add {
    hld<edge> H;
    centroid<edge> C;
    std::vector<T> X0;
    std::vector<int> O;
    std::vector<std::vector<int>> D;
    std::vector<fenwick_tree<T>> FT;
    std::vector<std::vector<fenwick_tree<T>>> FTc;

    contour_add() {}
    contour_add(const hld<edge> &_H, const centroid<edge> &_C, const std::vector<T> &X) : H(_H), C(_C), X0(X), O(C.N), D(C.N), FT(C.N), FTc(C.N) {
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
            int max_dist = 0;
            for (int i = C.G.begin(v); i < C.G.end(v); i++) {
                int c = C.G.elist[i];
                auto V = dfs(dfs, c);
                int max_distc = 0;
                for (int c : V) {
                    int dist = H.dist(c, v);
                    D[c].push_back(dist);
                    max_dist = std::max(max_dist, dist);
                    max_distc = std::max(max_distc, dist);
                }
                if (res.size() < V.size()) res.swap(V);
                res.insert(res.end(), V.begin(), V.end());
                FTc[v][i - C.G.begin(v)] = fenwick_tree<T>(max_distc + 1);
            }
            FT[v] = fenwick_tree<T>(max_dist + 1);
            return res;
        };
        dfs(dfs, root);
    }

    // vとの距離がl以上r未満の頂点にx加算
    void add(int v, int l, int r, T x) {
        int p = v, c = -1;
        for (int i = 0; i < D[v].size(); i++, c = p, p = C.par[p]) {
            int dist = D[v][i];
            int L = std::max(l - dist, 0);
            {
                int R = std::min(FT[p].size(), r - dist);
                if (L < R) {
                    FT[p].apply(L, x);
                    if (R < FT[p].size()) FT[p].apply(R, -x);
                }
            }
            if (i) {
                int ord = O[c];
                int R = std::min(FTc[p][ord].size(), r - dist);
                if (L < R) {
                    FTc[p][ord].apply(L, x);
                    if (R < FTc[p][ord].size()) FTc[p][ord].apply(R, -x);
                }
            }
        }
    }

    T get(int v) {
        T res = 0;
        int p = v, c = -1;
        for (int i = 0; i < D[v].size(); i++, c = p, p = C.par[p]) {
            int dist = D[v][i];
            res += FT[p].prod(dist + 1);
            if (i) res -= FTc[p][O[c]].prod(dist + 1);
        }
        return X0[v] + res;
    }
};
#endif
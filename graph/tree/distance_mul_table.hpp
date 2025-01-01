#ifndef _DISTANCE_MUL_TABLE_H_
#define _DISTANCE_MUL_TABLE_H_
#include "hld.hpp"
#include "centroid.hpp"
#include "../../math/fft/convolution_mod.hpp"
#include "../../math/fft/square_mod.hpp"

// D[i] := 距離iのペアの{j, k}のXj*Xkの和 (0 <= i < N)
template<typename mint, typename edge>
std::vector<mint> distance_mul_table(const centroid<edge> &C, const hld<edge> &H, const csr<edge> &G, const std::vector<mint> &X) {
    int N = G.N();
    std::vector<mint> ans(N, 0);
    int root = -1;
    for (int i = 0; i < N; i++) {
        if (C.par[i] == -1) {
            root = i;
            break;
        }
    }

    auto dfs = [&](auto &&dfs, int v) -> std::vector<int> {
        int deg = C.G.end(v) - C.G.begin(v);
        if (deg == 2) {
            ans[0] += X[v] * X[v];
            int c1 = C.G.elist[C.G.begin(v)];
            int c2 = C.G.elist[C.G.begin(v) + 1];
            auto A = dfs(dfs, c1);
            auto B = dfs(dfs, c2);
            std::vector<mint> freqA, freqB;
            for (int c : A) {
                int dist = H.dist(c, v);
                ans[dist] += 2 * X[v] * X[c];
                if (freqA.size() <= dist) freqA.resize(dist + 1, 0);
                freqA[dist] += X[c];
            }
            for (int c : B) {
                int dist = H.dist(c, v);
                ans[dist] += 2 * X[v] * X[c];
                if (freqB.size() <= dist) freqB.resize(dist + 1, 0);
                freqB[dist] += X[c];
            }
            freqA = convolution_mod<mint>(freqA, freqB);
            for (int j = 0; j < freqA.size(); j++) {
                ans[j] += 2 * freqA[j];
            }
            if (A.size() < B.size()) A.swap(B);
            A.insert(A.end(), B.begin(), B.end());
            A.push_back(v);
            return A;
        } else {
            std::vector<int> res{v};
            std::vector<mint> tmp{X[v]};
            for (int i = C.G.begin(v); i < C.G.end(v); i++) {
                int c = C.G.elist[i];
                auto V = dfs(dfs, c);
                std::vector<mint> tmpc;
                for (int c : V) {
                    int dist = H.dist(c, v);
                    if (tmp.size() <= dist) tmp.resize(dist + 1, 0);
                    if (tmpc.size() <= dist) tmpc.resize(dist + 1, 0);
                    tmp[dist] += X[c];
                    tmpc[dist] += X[c];
                }
                if (res.size() < V.size()) res.swap(V);
                res.insert(res.end(), V.begin(), V.end());
                tmpc = square_mod<mint>(tmpc);
                for (int j = 0; j < tmpc.size(); j++) {
                    ans[j] -= tmpc[j];
                }
            }
            tmp = square_mod<mint>(tmp);
            for (int j = 0; j < tmp.size(); j++) {
                ans[j] += tmp[j];
            }
            return res;
        }
    };
    dfs(dfs, root);
    mint i2 = mint(2).inv();
    for (int i = 1; i < N; i++) ans[i] *= i2;
    return ans;
}

template<typename mint, typename edge>
std::vector<mint> distance_mul_table(const csr<edge> &G, const std::vector<mint> &X) {
    return distance_mul_table<mint, edge>(centroid<edge>(G), hld<edge>(0, G), G, X);
}
#endif
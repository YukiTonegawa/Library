#ifndef _DISTANCE_TABLE_H_
#define _DISTANCE_TABLE_H_
#include "hld.hpp"
#include "centroid.hpp"
#include "../../math/fft/convolution59.hpp"
#include "../../math/fft/square59.hpp"

// D[i] := 距離iのペアの数 (0 <= i < N)
template<typename edge>
std::vector<long long> distance_table(const centroid<edge> &C, const hld<edge> &H, const csr<edge> &G) {
    int N = G.N();
    std::vector<long long> ans(N, 0);
    int root = -1;
    for (int i = 0; i < N; i++) {
        if (C.par[i] == -1) {
            root = i;
            break;
        }
    }

    auto dfs = [&](auto &&dfs, int v) -> std::vector<int> {
        int deg = C.G.end(v) - C.G.begin(v);
        // 次数2の場合そのまま畳み込んだ方が早い
        if (deg == 2) {
            ans[0]++;
            int c1 = C.G.elist[C.G.begin(v)];
            int c2 = C.G.elist[C.G.begin(v) + 1];

            auto A = dfs(dfs, c1);
            auto B = dfs(dfs, c2);

            std::vector<long long> freqA, freqB;

            for (int c : A) {
                int dist = H.dist(c, v);
                ans[dist] += 2;
                if (freqA.size() <= dist) freqA.resize(dist + 1, 0);
                freqA[dist]++;
            }

            for (int c : B) {
                int dist = H.dist(c, v);
                ans[dist] += 2;
                if (freqB.size() <= dist) freqB.resize(dist + 1, 0);
                freqB[dist]++;
            }
            freqA = convolution59(freqA, freqB);
            for (int j = 0; j < freqA.size(); j++) {
                ans[j] += 2 * freqA[j];
            }
            if (A.size() < B.size()) A.swap(B);
            A.insert(A.end(), B.begin(), B.end());
            A.push_back(v);
            return A;
        } else {
            std::vector<int> res{v};
            std::vector<long long> tmp{1};
            for (int i = C.G.begin(v); i < C.G.end(v); i++) {
                int c = C.G.elist[i];
                auto V = dfs(dfs, c);
                std::vector<long long> tmpc;
                for (int c : V) {
                    int dist = H.dist(c, v);
                    if (tmp.size() <= dist) tmp.resize(dist + 1, 0);
                    if (tmpc.size() <= dist) tmpc.resize(dist + 1, 0);
                    tmp[dist]++;
                    tmpc[dist]++;
                }
                if (res.size() < V.size()) res.swap(V);
                res.insert(res.end(), V.begin(), V.end());
                tmpc = square59(tmpc);
                for (int j = 0; j < tmpc.size(); j++) {
                    ans[j] -= tmpc[j];
                }
            }
            tmp = square59(tmp);
            for (int j = 0; j < tmp.size(); j++) {
                ans[j] += tmp[j];
            }
            return res;
        }
    };
    dfs(dfs, root);

    for (int i = 1; i < N; i++) ans[i] /= 2;
    return ans;
}

template<typename edge>
std::vector<long long> distance_table(const csr<edge> &G) {
    return distance_table(centroid<edge>(G), hld<edge>(0, G), G);
}
#endif
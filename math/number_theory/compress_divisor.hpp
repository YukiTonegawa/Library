#ifndef _COMPRESS_DIVISOR_H_
#define _COMPRESS_DIVISOR_H_

#include "rho.hpp"

struct compress_divisor {
    std::vector<uint64_t> D;
    std::vector<std::vector<int>> G;

    compress_divisor() {}
    // Nの正の約数に小さい順に0からidを割り振る
    compress_divisor(uint64_t N) {
        assert(N > 0);
        auto P = rho::factorize(N);
        std::sort(P.begin(), P.end());
        std::vector<std::pair<uint64_t, int>> tmp{{1, 0}};
        std::vector<std::pair<int, int>> E;
        int r = 1, sz = 1;
        uint64_t x = 1;
        for (int i = 0; i < P.size(); i++) {
            if (i && P[i] != P[i - 1]) {
                r = tmp.size();
                x = P[i];
            } else {
                x *= P[i];
            }
            for (int j = 0; j < r; j++) {
                tmp.push_back({tmp[j].first * x, sz});
                E.push_back({sz++, j});
            }
        }
        int M = tmp.size();
        std::sort(tmp.begin(), tmp.end());
        std::vector<int> id(M);
        D.resize(M);
        G.resize(M);
        for (int i = 0; i < M; i++) {
            id[tmp[i].second] = i;
            D[i] = tmp[i].first;
        }
        for (auto [s, t] : E) G[id[s]].push_back(id[t]);
    }
};

#endif
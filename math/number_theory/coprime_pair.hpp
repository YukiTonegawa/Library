#ifndef _COPRIME_PAIR_H_
#define _COPRIME_PAIR_H_
#include "../../data_structure/hash_table/hash_map.hpp"
#include "quotients.hpp"

// res[i][j] = gcd(x, y) = 1であるペアの数　(0 < x <= i, 0 < y <= j), O(nm)
std::vector<std::vector<int>> coprime_pair_table(int N, int M) {
    std::vector<std::vector<int>> res(N + 1, std::vector<int>(M + 1, 1));
    for (int i = 0; i <= N; i++) res[i][0] = 0;
    for (int i = 0; i <= M; i++) res[0][i] = 0;
    for (int k = 2; k <= std::min(N, M); k++) {
        if (res[k][k] == 0) continue;
        for (int i = k; i <= N; i += k) {
            for (int j = k; j <= M; j += k) {
                res[i][j] = 0;
            }
        }
    }
    for (int i = 1; i <= N; i++) {
        for(int j = 2; j <= M; j++) {
            res[i][j] += res[i][j - 1];
        }
    }
    for(int j = 1; j <= M; j++) {
        for(int i = 2; i <= N; i++) {
            res[i][j] += res[i - 1][j];
        }
    }
    return res;
}

// N, Mが10^9で900msくらい
__int128_t coprime_pair(uint64_t N, uint64_t M) {
    static std::vector<std::vector<int>> low;
    if (low.empty()) {
        low = coprime_pair_table(2000, 2000);
    }
    hash_map<uint64_t, __int128_t> mp;
    uint64_t dmin = std::max(N, M) + 1;
    auto csum = [&](auto &&csum, uint64_t x, uint64_t y) -> __int128_t {
        if (x < low.size()) return low[x][y];
        if (y == 0) return 0;
        uint64_t d = N / x;
        if (d >= dmin) return mp.find(d).second;
        __int128_t res = (__int128_t)x * y;
        for(auto [a, b] : quotients_floor<uint64_t>::enumerate_pair(x, y)) {
            if(a == 1) continue;
            res -= (b - a) * csum(csum, x / a, y / a);
        }
        mp.emplace(d, res);
        dmin = d;
        return res;
    };
    if (N < M) std::swap(N, M);
    return csum(csum, N, M);
}

#endif
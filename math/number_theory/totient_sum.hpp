#ifndef _TOTIENT_SUM_H_
#define _TOTIENT_SUM_H_
#include <vector>
#include <numeric>
#include "../../data_structure/hash_table/hash_map.hpp"
#include "quotients.hpp"

// res[i] := 0以上i以下のtotient関数の和
std::vector<uint64_t> totient_sum_table(uint64_t N) {
    assert(N >= 0);
    N++;
    if (N == 1) return {0};
    std::vector<uint64_t> res(N + 1);
    std::iota(res.begin() + 1, res.end(), 0);
    res[0] = 0;
    res[1] = 1;
    for (int i = 2; i < N; i++) {
        if (res[i] != i - 1) continue;
        for (int j = 2 * i; j < N; j += i) {
            res[j] -= res[j] / i;
        }
    }
    for (int i = 2; i < N; i++) res[i] += res[i - 1];
    return res;
}


__int128_t totient_sum(uint64_t N) {
    static std::vector<uint64_t> low;
    if (low.empty()) {
        low = totient_sum_table(std::min(N, (uint64_t)4000000));
    }
    
    hash_map<uint64_t, __int128_t> mp(1.5 * N / low.size());
    
    uint64_t memo_max = 0;
    auto tsum = [&](auto &&tsum, uint64_t x) -> __int128_t {
        if (x < low.size()) return low[x];
        if (x <= memo_max) return mp.find(x).second;
        __int128_t res = (__int128_t)x * (x + 1) / 2;

        quotients_floor<uint64_t>::apply_range(x, [&](uint64_t q, uint64_t l, uint64_t r) {
            if (q != x) {
                res -= (__int128_t)(r - l) * tsum(tsum, q);
            }
        });

        mp.emplace(x, res);
        memo_max = x;
        return res;
    };

    return tsum(tsum, N);
}
#endif
#ifndef _MAXIMUM_INDEPENDENT_SET_H_
#define _MAXIMUM_INDEPENDENT_SET_H_
#include <vector>

// i-bit目が1 -> 頂点iを使う
long long maximum_independent_set(const std::vector<long long> &G, long long rem = -1) {
    int N = G.size();
    if (rem == -1) rem = (1LL << N) - 1;
    long long res = 0;
    int k = -1, m = -1;
    while (true) {
        bool update = false;
        for (int i = 0; i < N; i++) {
            if (!((rem >> i) & 1)) continue;
            int s = __builtin_popcountll(rem & G[i]);
            if (s > m) k = i, m = s;
            if (s <= 1) {
                rem &= ~(G[i] | (1LL << i));
                res |= (1LL << i), update = true;
            }
        }
        if (!update) break;
        k = -1, m = -1;
    }
    if (rem > 0) {
        rem &= ~(1LL << k);
        long long p = maximum_independent_set(G, rem); //kを使わない
        long long q = maximum_independent_set(G, rem & ~G[k]); //kを使う
        if (__builtin_popcountll(p) > __builtin_popcountll(q)) res |= p;
        else res |= ((1LL << k) | q);
    }
    return res;
}
#endif
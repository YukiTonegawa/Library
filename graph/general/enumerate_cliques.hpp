#ifndef _ENUMERATE_CLIQUES_H_
#define _ENUMERATE_CLIQUES_H_
#include <vector>

// 全てのクリーク(頂点数0はクリークでない, 頂点数1はクリーク)に対してf(u128)を実行
template<typename Int, typename F>
void enumerate_cliques(const std::vector<Int> &_g, F f) {
    using u128 = __uint128_t;
    auto popcount128 = [](u128 x) -> int {
        return __builtin_popcountll(uint64_t(x)) + __builtin_popcountll(x >> 64);
    };
    std::vector<u128> g;
    for (Int x : _g) g.push_back(x);
    int N = g.size(), M = 0;
    for (u128 x : g) M += popcount128(x);
    M /= 2;

    u128 flag = ((u128)1 << N) - 1;

    auto pop = [&](u128 x, int k) -> bool { return (x >> k) & 1; };

    auto dfs = [&](auto &&dfs, int i, u128 s1, u128 s2, const std::vector<int> &V, const int must) -> void {
        if ((s1 & s2) != s2) return;
        if (i == V.size()) {
            f(s2);
            return;
        }
        int cur = V[i];
        dfs(dfs, i + 1, s1 & (g[cur] | ((u128)1 << cur)), s2 | ((u128)1 << cur), V, must);
        if (cur != must) dfs(dfs, i + 1, s1, s2, V, must);
    };

    for (int i = 0; i < N; i++) {
        int upper = 1;
        while (upper * upper <= 2 * M) upper++;
        int low = -1;
        for (int j = 0; j < N; j++) {
            if(pop(flag, j) && popcount128(g[j]) < upper){
                low = j;
                break;
            }
        }
        if (low == -1) {
            std::vector<int> v;
            for(int j = 0; j < N; j++) if (pop(flag, j)) v.push_back(j);
            dfs(dfs, 0, ((u128)1 << N) - 1, 0, v, -1);
            break;
        }else{
            std::vector<int> v{low};
            for (int j = 0; j < N; j++) {
                if (j != low && pop(g[low], j) && pop(flag, j)) {
                    v.push_back(j);
                }
            }
            dfs(dfs, 0, ((u128)1 << N) - 1, 0, v, low);
            M -= popcount128(g[low]);
            flag ^= (u128)1 << low;
            for(int j = 0; j < N; j++) {
                if (pop(g[j], low)) {
                    g[j] ^= (u128)1 << low;
                }
            }
        }
    }
}
#endif
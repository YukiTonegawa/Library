#ifndef _COUNTING_H_
#define _COUNTING_H_
#include "../../math/modint/combination.hpp"

// AGC53C
// http://enjapma.com/2021/04/11/random-card-game/
// 以下の条件を満たす長さ2Nの順列Pを数える
// A:=P[0,N), B:=P[N,2N)として任意のAiよりも大きい要素がB[0,i+d]にある
// 前処理 O(Nの最大値), クエリ O(1)
template<typename mint>
mint A_is_less_in_d(int N, int d) {
    assert(N > 0 && d >= 0);
    combination_mod<mint>::build(2 * N);
    static std::vector<mint> f, g;
    if (f.empty()) f = g = {1, 1};
    if (f.size() < 2 * N) {
        int l = f.size();
        f.resize(2 * N);
        g.resize(2 * N);
        for (int i = l; i < 2 * N; i++) {
            f[i] = f[i - 2] * i;
            g[i] = g[i - 2] * combination_mod<mint>::inv(i);
        }
    }
    d = std::min(d, N - 1);
    mint res = combination_mod<mint>::fac(2 * N - 1);
    res *= f[2 * N - 1 - d] * g[d + 1] * g[2 * N - 2 - d] * f[d];
    return res * 2 * (d + 1);
}

#endif
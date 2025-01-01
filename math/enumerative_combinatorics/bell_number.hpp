#ifndef _BELL_NUMBER_H_
#define _BELL_NUMBER_H_
#include "../modint/combination.hpp"
#include "../fps/fps.hpp"

// B[0, n]
// 異なるN個のものをいくつかの箱に入れる方法(箱同士は順序を区別しない)
template<typename mint>
std::vector<mint> bell_number(int n) {
    combination_mod<mint>::build(n);
    fps<mint> F(n + 1, 0);
    for (int i = 1; i <= n; i++) F[i] = combination_mod<mint>::ifac(i);
    F = F.exp(n + 1);
    for (int i = 1; i <= n; i++) F[i] *= combination_mod<mint>::fac(i);
    return F;
}
#endif
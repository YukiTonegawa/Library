#ifndef _CATALAN_NUMBER_H_
#define _CATALAN_NUMBER_H_
#include "../modint/combination.hpp"

// 1, 2, 5, 14, 42, 132, 429, 1430
// open, closeがn個ずつある正しい括弧列の数
template<typename mint>
mint catalan_number(int n) {
    combination_mod<mint>::build(2 * n);
    return combination_mod<mint>::comb(2 * n, n) * combination_mod<mint>::inv(n + 1);
}

// open, closeがn個ずつある正しい括弧列のうち"()"がk個ある列の数
template<typename mint>
mint narayana_number(int n, int k) {
    combination_mod<mint>::build(n);
    return combination_mod<mint>::comb(n, k) * combination_mod<mint>::comb(n, k - 1) * combination_mod<mint>::inv(n);
}
#endif
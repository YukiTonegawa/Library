#ifndef _P_RECURSIVE_H_
#define _P_RECURSIVE_H_
#include <vector>
#include "../matrix/matrix_mod.hpp"

template<typename mint>
std::vector<std::vector<mint>> find_p_recursive(const std::vector<mint>& A, int r, int d) {
    assert(r >= 0 && d >= 0);
    int N = A.size();
    int m = (r + 1) * (d + 1);
    if (N < m + r) return {};
    using mat = matrix_mod<mint>;
    mat M(N - r, m, 0);
    for (int k = 0; k < N - r; k++) {
        // P0(k)ak + P1(k)ak+1 ... Pr(k)ak+r = 0
        for (int i = 0; i <= r; i++) {
            mint x = A[k + i];
            for (int j = 0; j <= d; j++, x *= k) {
                int val_id = i * (d + 1) + j;
                M[k][val_id] = x;
            }
        }
    }
    auto [rnk, C, sol] = mat::system_of_linear_equations(M, std::vector<mint>(N - r, mint(0)));
    if (rnk == -1) return {}; // 解なし
    std::vector<std::vector<mint>> res(r + 1, std::vector<mint>(d + 1));
    int none_zero = -2;
    {
        for (int i = 0; i <= d; i++) {
            if (sol[r * (d + 1) + i] != 0) {
                none_zero = -1;
                break;
            }
        }
        if (none_zero == -2) {
            for (int k = 0; k < rnk && none_zero == -2; k++) {
                for (int i = 0; i <= d; i++) {
                    if (C[k][r * (d + 1) + i] != 0) {
                        none_zero = k;
                        break;
                    }
                }
            }
        }
    }
    if (none_zero == -2) return {};
    
    for (int i = 0; i <= r; i++) {
        for (int j = 0; j <= d; j++) {
            res[i][j] = sol[i * (d + 1) + j];
            if (none_zero != -1) res[i][j] += C[none_zero][i * (d + 1) + j];
        }
    }
    return res;
}

#endif
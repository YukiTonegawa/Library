#ifndef _BINOMIAL_SUM_H_
#define _BINOMIAL_SUM_H_
#include "../modint/combination.hpp"
#include "../../data_structure/mo/mo_algorithm.hpp"

// F(i, j) := iC0 + iC1 + ... iCj
template<typename mint>
std::vector<mint> pascal_sum_row(std::vector<std::pair<int, int>> Q) {
    struct pascal_sum_row_struct {
        mint i2 = mint(2).inv();
        mint x = i2;
        pascal_sum_row_struct() {}
        void add_left(int l, int r) {
            x -= combination_mod<mint>::comb(r - 1, l + 1);
        }
        void del_left(int l, int r) {
            x += combination_mod<mint>::comb(r - 1, l + 1);
        }
        void add_right(int l, int r) {
            r--;
            x = 2 * x - combination_mod<mint>::comb(r, l);
        }
        void del_right(int l, int r) {
            r--;
            x = (x + combination_mod<mint>::comb(r, l)) * i2;
        }
    };
    int max_n = 0;
    for (auto [i, j] : Q) max_n = std::max(max_n, i); // nCrの最大のn
    combination_mod<mint>::build(max_n + 2);
    pascal_sum_row_struct pas;
    mo_algorithm mo(max_n + 1, pas);
    int q = Q.size();
    std::vector<mint> ans(q);
    for (int i = 0; i < q; i++) {
        if (Q[i].first < Q[i].second) Q[i].second = Q[i].first;
        mo.add_query(Q[i].second, Q[i].first + 1);
    }
    for (int i = 0; i < q; i++) {
        int idx = mo.process2();
        ans[idx] = pas.x;
    }
    return ans;
}

// F(i, j) := (0a + b)iC0 + (1a+b)iC1 + ... (ja+b)iCj
template<typename mint>
std::vector<mint> pascal_sum_row_arithmetic_weighted(mint a, mint b, std::vector<std::pair<int, int>> Q) {
    struct pascal_sum_row_arithmetic_struct {
        mint i2 = mint(2).inv();
        mint a, b, g, f = i2;

        pascal_sum_row_arithmetic_struct(mint _a, mint _b) : a(_a), b(_b), g(b * i2 - a * i2 * i2) {}

        void add_left(int l, int r) {
            l++;
            g -= (a * l + b) * combination_mod<mint>::comb(r - 1, l);
            f -= combination_mod<mint>::comb(r - 1, l);
        }
        void del_left(int l, int r) {
            l++;
            g += (a * l + b) * combination_mod<mint>::comb(r - 1, l);
            f += combination_mod<mint>::comb(r - 1, l);
        }
        void add_right(int l, int r) {
            r--;
            g = 2 * g + a * f - (a * l + a + b) * combination_mod<mint>::comb(r, l);
            f = 2 * f - combination_mod<mint>::comb(r, l);
        }
        void del_right(int l, int r) {
            r--;
            f = (f + combination_mod<mint>::comb(r, l)) * i2;
            g = (g - a * f + (a * l + a + b) * combination_mod<mint>::comb(r - 1, l)) * i2;
        }
    };
    int max_n = 0;
    for (auto [i, j] : Q) max_n = std::max(max_n, i); // nCrの最大のn
    pascal_sum_row_arithmetic_struct pas(a, b);
    combination_mod<mint>::build(max_n + 2);
    mo_algorithm mo(max_n + 1, pas);
    int q = Q.size();
    std::vector<mint> ans(q);
    for (int i = 0; i < q; i++) {
        if (Q[i].first < Q[i].second) Q[i].second = Q[i].first;
        mo.add_query(Q[i].second, Q[i].first + 1);
    }
    for (int i = 0; i < q; i++) {
        int idx = mo.process2();
        ans[idx] = pas.g;
    }
    return ans;
}

// F(i, j) := (ab^0)iC0 + (ab^1)iC1 + ... (ab^j)iCj
template<typename mint>
std::vector<mint> pascal_sum_row_geometric_weighted(mint a, mint b, std::vector<std::pair<int, int>> Q) {
    struct pascal_sum_row_geometric_struct {
        const mint a, b, binv, b_plus_oneinv;
        mint a_b_pow_l, f;

        pascal_sum_row_geometric_struct(mint _a, mint _b): a(_a), b(_b), binv(b.inv()), b_plus_oneinv((b + 1).inv()), a_b_pow_l(a), f(a * b_plus_oneinv) {}

        void add_left(int l, int r) {
            l++;
            f -= a_b_pow_l * combination_mod<mint>::comb(r - 1, l);
            a_b_pow_l *= binv;
        }
        void del_left(int l, int r) {
            l++;
            a_b_pow_l *= b;
            f += a_b_pow_l * combination_mod<mint>::comb(r - 1, l);
        }
        void add_right(int l, int r) {
            r--;
            f = (1 + b) * f - b * a_b_pow_l * combination_mod<mint>::comb(r, l);
        }
        void del_right(int l, int r) {
            r--;
            f = (f + b * a_b_pow_l * combination_mod<mint>::comb(r - 1, l)) * b_plus_oneinv;
        }
    };
    int max_n = 0;
    for (auto [i, j] : Q) max_n = std::max(max_n, i); // nCrの最大のn
    pascal_sum_row_geometric_struct pas(a, b);
    combination_mod<mint>::build(max_n + 2);
    mo_algorithm mo(max_n + 1, pas);
    int q = Q.size();
    std::vector<mint> ans(q);
    for (int i = 0; i < q; i++) {
        if (Q[i].first < Q[i].second) Q[i].second = Q[i].first;
        mo.add_query(Q[i].second, Q[i].first + 1);
    }
    for (int i = 0; i < q; i++) {
        int idx = mo.process2();
        ans[idx] = pas.f;
    }
    return ans;
}

// F(i, j) = iCj + iC(j - 2) + iC(j - 4) ...
template<typename mint>
std::vector<mint> pascal_sum_row_stripe(std::vector<std::pair<int, int>> Q) {
    struct pascal_sum_row_stripe_struct {
        mint i2;
        mint x, y, tmp;
        pascal_sum_row_stripe_struct() : i2(mint(2).inv()), x(1), y(0) {}
        void add_left(int l, int r) {
            tmp = x - combination_mod<mint>::comb(r - 1, l + 1);
            x = y;
            y = tmp;
        }
        void del_left(int l, int r) {
            tmp = y + combination_mod<mint>::comb(r - 1, l + 1);
            y = x;
            x = tmp;
        }
        void add_right(int l, int r) {
            tmp = x + y;  // ∑{0 <= k <= j}iCk
            x = tmp;
            y = tmp - combination_mod<mint>::comb(r, l);
        }
        void del_right(int l, int r) {
            r--;
            tmp = x + y; // ∑{0 <= k <= j}iCk
            tmp = (tmp + combination_mod<mint>::comb(r, l)) * i2; // ∑{0 <= k <= j}(i - 1)Ck
            tmp = (tmp + combination_mod<mint>::comb(r - 1, l)) * i2; // ∑{0 <= k <= j}(i - 2)Ck
            x = tmp;
            y = tmp - combination_mod<mint>::comb(r, l);
        }
    };
    int max_n = 0;
    for (auto [i, j] : Q) max_n = std::max(max_n, i); // nCrの最大のn
    combination_mod<mint>::build(max_n + 2);
    pascal_sum_row_stripe_struct st;
    mo_algorithm mo(max_n + 1, st);
    int q = Q.size();
    std::vector<mint> ans(q);
    for (int i = 0; i < q; i++) {
        if (Q[i].first < Q[i].second) Q[i].second = Q[i].first;
        mo.add_query(Q[i].second, Q[i].first + 1);
    }
    for (int i = 0; i < q; i++) {
        int idx = mo.process2();
        ans[idx] = st.x;
    }
    return ans;
}

// F(i, j) := 0Cj + 1Cj + ... iCj
template<typename mint>
mint pascal_sum_column(int i, int j) {
    combination_mod<mint>::build(std::max(i, j) + 1);
    return combination_mod<mint>::comb(i + 1, j + 1);
}

// ガリガリ計算するとこの式になる
// F(i, j) := (0a+b)0Cj + (1a+b)1Cj + ... (ia+b)iCj
template<typename mint>
mint pascal_sum_column_arithmetic_weighted(mint a, mint b, int i, int j) {
    combination_mod<mint>::build(std::max(i, j) + 2);
    return (a * i + b) * combination_mod<mint>::comb(i + 1, j + 1) - a * combination_mod<mint>::comb(i + 1, j + 2);
}

// F(i, j) := 0C0 + 0C1 ... 0Cj + 1C0 + 1C1 ... iC0 + iC1... iCj
template<typename mint>
std::vector<mint> pascal_sum(std::vector<std::pair<int, int>> Q) {
    int q = Q.size();
    for (int i = 0; i < q; i++) {
        Q[i].first++;
        Q[i].second++;
    }
    auto res = pascal_sum_row<mint>(Q);
    for (int i = 0; i < q; i++) res[i] -= 1;
    return res;
}

/*
// G(i, j) := comb(i, j) * comb(n - i, m - j), {0 <= i <= n, 0 <= j <= m}
// G(i, j) = (0, 0) -> (i, j) -> (n, m)の経路数
// F(i, j) := G(i, 0) + G(i - 1, 1) + ... G(i - j, j)
template<typename mint>
std::vector<mint> diagonal_path_sum(int n, int m, std::vector<std::pair<int, int>> Q) {
    struct diagonal_path_sum_struct {
        int n, m;
        mint x;
        diagonal_path_sum_struct(int n, int m): n(n), m(m), x(combination_mod<mint>::comb(n, m)) {}
        void add_left(int l, int r) {
            x -= combination_mod<mint>::comb(r - 1, l + 1) * combination_mod<mint>::comb(n - (r - 1), m - (l + 1));
        }
        void add_right(int l, int r) {
            r--;
            x -= combination_mod<mint>::comb(r, l) * combination_mod<mint>::comb(n - r - 1, m - l - 1);
        }
        void del_left(int l, int r) {
            x += combination_mod<mint>::comb(r - 1, l + 1) * combination_mod<mint>::comb(n - (r - 1), m - (l + 1));
        }
        void del_right(int l, int r) {
            r--;
            x += combination_mod<mint>::comb(r, l) * combination_mod<mint>::comb(n - r - 1, m - l - 1);
        }
    };
    combination_mod<mint>::build(std::max(n, m) + 2);
    diagonal_path_sum_struct dp(n, m);
    mo_algorithm mo(n + 1, dp);
    int q = Q.size();
    std::vector<mint> ans(q);
    for (int i = 0; i < q; i++) mo.add_query(Q[i].first, Q[i].second + 1);
    for (int i = 0; i < q; i++) ans[mo.process2()] = dp.x;
    return ans;
}
*/

// F(i, j) := iCj + (i+1)C(j-1) + (i+2)C(j-2) ... i+jC0
template<typename mint>
std::vector<mint> pascal_sum_diagonal_ur(std::vector<std::pair<int, int>> Q) {
    struct pascal_sum_diagonal_ur_struct {
        mint i2 = mint(2).inv();
        mint x = 1, y = 0;
        pascal_sum_diagonal_ur_struct() {}
        void add_left(int l, int r) {
            // (l + 1, r) -> (l, r)
            x = x - y + combination_mod<mint>::comb(r - 1, l);
            std::swap(x, y);
            x -= combination_mod<mint>::comb(r - 1, l + 1);
            y -= combination_mod<mint>::comb(r - 2, l + 1);
        }
        void del_left(int l, int r) {
            // (l, r) -> (l + 1, r)
            y = x + y - combination_mod<mint>::comb(r - 1, l);
            std::swap(x, y);
            x += combination_mod<mint>::comb(r, l + 1);
            y += combination_mod<mint>::comb(r - 1, l + 1);
        }
        void add_right(int l, int r) {
            // (l, r) -> (l, r + 1)
            y = x + y - combination_mod<mint>::comb(r - 1, l);
            std::swap(x, y);
        }
        void del_right(int l, int r) {
            // (l, r + 1) -> (l, r)
            x = x - y + combination_mod<mint>::comb(r - 1, l);
            std::swap(x, y);
        }
    };
    int max_n = 0;
     for (int i = 0; i < Q.size(); i++) {
        if (Q[i].first < Q[i].second) {
            int diff = (Q[i].second - Q[i].first + 1) / 2;
            Q[i].first += diff;
            Q[i].second -= diff;
        }
        max_n = std::max(max_n, Q[i].first);
    }
    combination_mod<mint>::build(max_n + 2);
    pascal_sum_diagonal_ur_struct pas;
    mo_algorithm mo(max_n + 1, pas);
    int q = Q.size();
    std::vector<mint> ans(q);
    for (int i = 0; i < q; i++) {
        mo.add_query(Q[i].second, Q[i].first);
    }
    for (int i = 0; i < q; i++) {
        int idx = mo.process2();
        ans[idx] = pas.x;
    }
    return ans;
}
#endif
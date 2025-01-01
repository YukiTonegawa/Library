#ifndef _TWO_SQUARE_SUM_H_
#define _TWO_SQUARE_SUM_H_
#include "rho.hpp"
#include "sqrt_mod.hpp"
#include "../modint/modint64_dynamic_prime.hpp"
#include "../base/gaussian_integer.hpp"
#include <vector>
#include <cassert>

// 0 <= a, bかつN = a^2 + b^2を満たすペア
// 0 <= N <= 10^18
std::vector<gaussian_integer<int>> two_square_sum(long long N) {
    using mint = modint64_dynamic_prime<-1>;
    using gint = gaussian_integer<int>;
    if (N < 0) return {};
    if (N == 0) return {{0, 0}};
    int lsb = __builtin_ctzll(N);
    N >>= lsb;
    if (N % 4 == 3) return {};
    long long allmul = 1LL << (lsb / 2);
    if (lsb % 2 == 1) N *= 2;
    int L = 0;
    std::vector<gint> res{{0, 1}, {1, 0}};
    for (auto [p, q] : rho::factorize2(N)) {
        if (p % 4 == 3) {
            if (q % 2) return {};
            for (int i = 0; i < q / 2; i++) allmul *= p;
        } else {
            __int128_t A = -1, B = -1;
            if (p == 2) {
                A = 1;
                B = 1;
            } else {
                mint::set_mod(p);
                auto [f, x] = sqrt_mod<mint>(p - 1);
                assert(f);
                A = x.val();
                B = 1;
                while (true) {
                    __int128_t Q = (A * A + B * B) / p;
                    if (Q == 1) break;
                    __int128_t C = A % Q, D = B % Q;
                    if (C * 2 >= Q) C -= Q;
                    if (D * 2 >= Q) D -= Q;
                    __int128_t E = (C * A + D * B) / Q;
                    __int128_t F = (D * A - C * B) / Q;
                    A = (E >= 0 ? E : -E);
                    B = (F >= 0 ? F : -F);
                }
                assert(A * A + B * B == p);
            }
            // A + Biとその共役
            // B + Aiとその共役
            // 最大8個が条件を満たす
            std::vector<gint> tmp;
            gint a{(int)A, (int)B}, b{(int)B, (int)A};

            gint e{0, 1};
            for (int i = 0; i <= q; i++) {
                gint x = e;
                for (int j = 0; j < i; j++) x = x * a;
                for (int j = i; j < q; j++) x = x * b;
                tmp.push_back(x);
            }
            int R = res.size();
            for (int i = L; i < R; i++) {
                for (int j = 0; j < tmp.size(); j++) {
                    res.push_back(res[i] * tmp[j]);
                }
            }
            L = R;
        }
    }
    for (int i = L; i < res.size(); i++) {
        res[i].a *= allmul;
        res[i].b *= allmul;
    }

    for (int i = L; i < res.size(); i++) {
        res[i].a = abs(res[i].a);
        res[i].b = abs(res[i].b);
    }
    std::sort(res.begin() + L, res.end(), [](gint a, gint b) { return a.a < b.a || (a.a == b.a && a.b < b.b); });
    std::vector<gint> ans;
    for (int i = L; i < res.size(); i++) {
        gint x = res[i];
        if (ans.empty() || x.a != ans.back().a || x.b != ans.back().b) {
            ans.push_back(x);
        }
    }   
    return ans;
}
#endif
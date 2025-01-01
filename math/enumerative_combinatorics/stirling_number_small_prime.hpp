#ifndef _STIRLING_NUMBER_SMALL_P_H_
#define _STIRLING_NUMBER_SMALL_P_H_
#include <vector>
#include "../number_theory/lucas.hpp"

template<int id>
struct stirling_number_small_prime {
    using mint = modint32_dynamic<id>;
    using cmb = combination_mod<mint>;
    using luc = lucas_prime<id>;
    
    static void set_mod(int mod) {
        assert(0 < mod && mod < 1e7);
        mint::set_mod(mod);
        cmb::build(mod - 1);
        luc::set_mod(mod);
    }

    static mint first(long long n, long long k) {
        static std::vector<std::vector<mint>> table;
        if (table.empty()) {
            table.resize(mint::mod(), std::vector<mint>(mint::mod(), 0));
            table[0][0] = 1;
            for (int i = 1; i < mint::mod(); i++) {
                table[i][0] = 0;
                for (int j = 1; j <= i; j++) {
                    table[i][j] = table[i - 1][j - 1] + table[i - 1][j] * (-i + 1);
                }
            }
        }
        if (n < k) return 0;
        if (n < mint::mod()) return table[n][k];
        if (k == 0) return n == 0;
        long long i = n / mint::mod();
        long long j = n - i * mint::mod();
        if (i > k) return 0;
        long long t = k - i;
        long long a = t / (mint::mod() - 1);
        long long b = t - a * (mint::mod() - 1);
        mint res = 0;
        if (a <= i && b <= j) {
            res += ((i - a) % 2 != 0 ? -1 : 1) * luc::comb(i, a) * first(j, b);
        }
        if (a > 0 && a - 1 <= i && b == 0 && j == mint::mod() - 1) {
            a--;
            b = mint::mod() - 1;
            res += ((i - a) % 2 != 0 ? -1 : 1) * luc::comb(i, a) * first(j, b);
        }
        return res;
    }
    
    // よくわかってない
    static mint second(long long n, long long k) {
        static std::vector<std::vector<mint>> table;
        if (table.empty()) {
            table.resize(mint::mod(), std::vector<mint>(mint::mod(), 0));
            table[0][0] = 1;
            for (int i = 1; i < mint::mod(); i++) {
                table[i][0] = 0;
                for (int j = 1; j <= i; j++) {
                    table[i][j] = table[i - 1][j - 1] + table[i - 1][j] * j;
                }
            }
        }
        if (n < k) return 0;
        if (n < mint::mod()) return table[n][k];
        if (k == 0) return n == 0;
        long long i = k / mint::mod();
        long long j = k - i * mint::mod();
        long long a = (n - i) / (mint::mod() - 1);
        long long b = (n - i) - a * (mint::mod() - 1);
        if (b == 0) {
            assert(a);
            b += mint::mod() - 1;
            a--;
        }
        if (b == mint::mod() - 1) {
            return luc::comb(a, i) * second(mint::mod() - 1, j) + luc::comb(a, i - 1) * second(0, j);
        } else {
            return luc::comb(a, i) * second(b, j);
        }
    }
};

#endif
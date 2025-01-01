#ifndef _RHO_H_
#define _RHO_H_
#include "../base/fast_mul_mod/montgomery64.hpp"
#include "../base/gcd.hpp"
#include "miller_rabin64.hpp"
#include <vector>
#include <cstdint>
#include <array>

namespace rho {
    uint64_t find_divisor(uint64_t n) {
        static std::vector<int> small_p{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
        for (int sp : small_p) if(n % sp == 0) return sp; // n < 50

        montgomery64 mr(n);
        if (miller_rabin_mr(n, mr)) return n;

        auto try_factorize = [n, mr](uint64_t c) {
            c = mr.generate(c);
            auto f = [mr, c](uint64_t mx) {
                return mr.add(mr.mul(mx, mx), c);
            };
            uint64_t m = 1LL << ((64 - __builtin_clzll(n)) / 8);
            uint64_t y = n, r = 1, q = 1;
            uint64_t x, g, k, ys;
            do {
                x = y;
                y = mr.generate(y);
                for (int i = 0; i < r; i++) y = f(y);
                y = mr.reduce(y);

                k = 0;
                while (k < r && g == 1) {
                    q = mr.generate(q);
                    y = mr.generate(y);
                    ys = y;
                    for (int i = 0; i < std::min(m, r - k); i++) {
                        y = f(y);
                        uint64_t z = mr.reduce(y);
                        q = mr.mul(q, mr.generate(x > z ? x - z : z - x));
                    }
                    y = mr.reduce(y);
                    g = gcd(mr.reduce(q), n);
                    k += m;
                }
                r <<= 1;
            } while (g == 1);
            
            if (g == n) {
                do {
                    ys = f(ys);
                    uint64_t z = mr.reduce(ys);
                    g = gcd(x > z ? x - z : z - x, n);
                } while (g == 1);
            }
            return g; // g == n when failure
        };
    
        uint64_t c = 1, res = n;
        
        do {
            res = try_factorize(c);
            //c = random_prime(2, n - 1);
            c = (c + 1) % n;
        } while (res == n);
        
        return res;
    }
  
    std::vector<uint64_t> factorize(uint64_t n) {
        assert(n < (1ULL << 63));
        if (n <= 1) return {};
        uint64_t x = rho::find_divisor(n);
        if (x == n) return {x};
        auto l = rho::factorize(x);
        auto r = rho::factorize(n / x);
        l.insert(l.end(), r.begin(), r.end());
        return l;
    }

    // {素数, 個数}の形で返す
    std::vector<std::pair<uint64_t, int>> factorize2(uint64_t n) {
        auto p = rho::factorize(n);
        std::sort(p.begin(), p.end());
        std::vector<std::pair<uint64_t, int>> res;
        for (uint64_t i : p) {
            if (res.empty() || res.back().first != i) {
                res.push_back({i, 1});
            } else {
                res.back().second++;
            }
        }
        return res;
    }

    // 素因数の集合(重複なし, ソート済)を返す
    std::vector<uint64_t> prime_factor(uint64_t n){
        auto p = rho::factorize(n);
        std::sort(p.begin(), p.end());
        p.erase(std::unique(p.begin(), p.end()), p.end());
        return p;
    }

    // 10^18以下の高度合成数 897612484786617600の約数が103680個なので全列挙して良さそう
    std::vector<uint64_t> divisor(uint64_t n) {
        assert(n >= 1);
        auto P = rho::factorize(n);
        std::sort(P.begin(), P.end());
        
        std::vector<uint64_t> res{1};
        int r = 1;
        
        uint64_t x = 1;
        for (int i = 0; i < P.size(); i++) {
            if (i && P[i] != P[i - 1]) {
                r = res.size();
                x = P[i];
            } else {
                x *= P[i];
            }
            for (int j = 0; j < r; j++) {
                res.push_back(res[j] * x);
            }
        }

        return res;
    }

    // (nの約数a, a未満のaの約数b)の全ての組に対してf(a, b)を呼ぶ
    // a -> bに辺を張ったトポロジカル順の逆を満たす
    template<typename F>
    void aggregate_divisor(uint64_t n, F f) {
        auto P = factorize2(n);
        int N = P.size();
        auto dfs = [&](auto &&dfs, uint64_t a, uint64_t b, int k) -> void {
            if (k == N) {
                if (a != b) f(a, b);
                return;
            }
            auto [p, q] = P[k];
            for (int i = 0; i <= q; i++, a *= p) {
                uint64_t B = b;
                for (int j = 0; j <= i; j++, B *= p) {
                    dfs(dfs, a, B, k + 1);
                }
            }
        };
        dfs(dfs, 1, 1, 0);
    }

    // (nの約数a, aより大きいaの倍数かつnの約数b)の全ての組に対してf(a, b)を呼ぶ
    // a <- bに辺を張ったトポロジカル順を満たす
    template<typename F>
    void aggregate_multiple(uint64_t n, F f) {
        auto P = factorize2(n);
        int N = P.size();
        
        static std::array<std::array<uint64_t, 64>, 64> powtable;

        for (int i = 0; i < N; i++) {
            auto [p, q] = P[i];
            uint64_t x = 1;
            for (int j = 0; j <= q; j++, x *= p) {
                powtable[i][j] = x;
            }
        }
        
        auto dfs = [&](auto &&dfs, uint64_t a, uint64_t b, int k) -> void {
            if (k == N) {
                if (a != b) f(b, a);
                return;
            }
            auto [p, q] = P[k];
            for (int i = q; i >= 0; i--) {
                uint64_t x = a * powtable[k][i];
                for (int j = i; j >= 0; j--) {
                    dfs(dfs, x, b * powtable[k][j], k + 1);
                }
            }
        };
        dfs(dfs, 1, 1, 0);
    }

    uint64_t count_divisor(uint64_t n) {
        uint64_t res = 1;
        for (auto [p, cnt] : rho::factorize2(n)) {
            res *= cnt + 1;
        }
        return res;
    }

    int mobius_function(long long x) {
        auto P = rho::factorize(x);
        for (long long p : P) if((x / p) % p == 0) return 0;
        return P.size() % 2 == 0 ? 1 : -1;
    }
    
    uint64_t totient_function(uint64_t n) {
        uint64_t res = n;
        auto prims = rho::prime_factor(n);
        for (auto p : prims) res -= res / p;
        return res;
    }

    // [1, n]でaと素な数の個数
    // 2^{aの素因数の種類} <= aの約数の数 なので O(sqrt(a))
    // 1 <= a <= Mについて呼び出すと O(MlogM)
    uint64_t count_coprime(uint64_t a, uint64_t n) {
        assert(a <= 1e18);
        static std::vector<int64_t> table;
        // (a <= 10^18) -> d(a) < 110000;
        if (table.empty()) table.resize(110000);
        table[0] = 1;
        int r = 1;
        for (uint64_t p : factorize(a)) {
            while (a % p == 0) a /= p;
            int l = r;
            for (int i = 0; i < r; i++) {
                table[l++] = -table[i] * p;
            }
            r = l;
        }
        uint64_t res = 0;
        for (int i = 0; i < r; i++) {
            int64_t x = table[i];
            if (x < 0) res -= n / -x;
            else res += n / x;
        }
        return res;
    }
};
#endif
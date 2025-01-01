#ifndef _PRIME_SIEVE_H_
#define _PRIME_SIEVE_H_
#include <vector>
#include <numeric>

namespace prime_sieve {
    std::vector<int> primes, min_factor, mobius_table, totient_table;
    
    // [0, N]
    void init(int N) {
        min_factor.resize(N + 1, -1);
        for (int i = 2; i <= N; i++) {
            if (min_factor[i] == -1) {
                primes.push_back(i);
                min_factor[i] = i;
            }
            for (int p : primes) {
                if ((long long)p * i > N || p > min_factor[i]) {
                    break;
                }
                min_factor[p * i] = p;
            }
        }
    }

    // min_factorのサイズ分のテーブルを作る
    void init_totient_table() {
        int N = min_factor.size();
        totient_table.resize(N);
        if (N <= 1) {
            if (N == 1) totient_table[0] = 0;
            return;
        }
        std::iota(totient_table.begin() + 1, totient_table.end(), 0);
        totient_table[1] = 1;
        for (int i = 2; i < N; i++) {
            if (min_factor[i] != -1) continue;
            for (int j = 2 * i; j < N; j += i) {
                totient_table[j] -= totient_table[j] / i;
            }
        }
    }

    // min_factorのサイズ分のテーブルを作る
    void init_mobius_table() {
        int N = min_factor.size();
        mobius_table.resize(N, 1);
        for (int i = 2; i < N; i++) {
            if (min_factor[i] != -1) continue;
            mobius_table[i] = -1;
            for (int j = 2 * i; j < N; j += i) {
                mobius_table[j] *= -1;
            }
            for (long long k = (long long)i * i, j = k; j < N; j += k) {
                mobius_table[j] = 0;
            }
        }
    }

    bool is_prime(int n) {
        assert(n < min_factor.size());
        return n == min_factor[n];
    }

    // {{素因数, 数}}, O(log n)
    std::vector<std::pair<int, int>> factorize(int n) {
        assert(n < min_factor.size());
        std::vector<std::pair<int, int>> res;
        while (n > 1) {
            int cnt = 0, f = min_factor[n];
            while (n % f == 0) {
                n /= f;
                cnt++;
            }
            res.push_back({f, cnt});
        }
        return res;
    }

    // 約数列挙, O(√n)
    // ソート済とは限らない
    std::vector<int> divisor(int n) {
        std::vector<int> res{1};
        int r = 1;
        for (auto [p, cnt] : factorize(n)) {
            int x = p;
            for (int j = 0; j < cnt; j++, x *= p) {
                for (int k = 0; k < r; k++) {
                    res.push_back(res[k] * x);
                }
            }
            r = res.size();
        }
        return res;
    }
    
    // (nの約数a, a未満のaの約数b)の全ての組に対してf(a, b)を呼ぶ
    // a -> bに辺を張ったトポロジカル順の逆を満たす
    template<typename F>
    void aggregate_divisor(int n, F f) {
        auto P = factorize(n);
        int N = P.size();
        auto dfs = [&](auto &&dfs, int a, int b, int k) -> void {
            if (k == N) {
                if (a != b) f(a, b);
                return;
            }
            auto [p, q] = P[k];
            for (int i = 0; i <= q; i++, a *= p) {
                int B = b;
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
    void aggregate_multiple(int n, F f) {
        auto P = factorize(n);
        int N = P.size();
        
        static std::array<std::array<int, 30>, 30> powtable;

        for (int i = 0; i < N; i++) {
            auto [p, q] = P[i];
            int x = 1;
            for (int j = 0; j <= q; j++, x *= p) {
                powtable[i][j] = x;
            }
        }
        
        auto dfs = [&](auto &&dfs, int a, int b, int k) -> void {
            if (k == N) {
                if (a != b) f(b, a);
                return;
            }
            auto [p, q] = P[k];
            for (int i = q; i >= 0; i--) {
                int x = a * powtable[k][i];
                for (int j = i; j >= 0; j--) {
                    dfs(dfs, x, b * powtable[k][j], k + 1);
                }
            }
        };
        dfs(dfs, 1, 1, 0);
    }

    int count_divisor(int n) {
        int res = 1;
        for (auto [p, cnt] : factorize(n)) {
            res *= cnt + 1;
        }
        return res;
    }

    // O(logN)
    int totient_function(int n) {
        if (n < totient_table.size()) return totient_table[n];
        int res = n;
        int prev = -1;
        while (n > 1) {
            if (min_factor[n] > prev) {
                res -= res / min_factor[n];
                prev = min_factor[n];
            }
            n /= min_factor[n];
        }
        return res;
    }

    // O(logN)
    int mobius_function(int x) {
        if (x < mobius_table.size()) return mobius_table[x];
        int pcnt = 0;
        while (x > 1) {
            int y = x / min_factor[x];
            if (min_factor[x] == min_factor[y]) return 0;
            x = y;
            pcnt++;
        }
        return pcnt % 2 == 0 ? 1 : -1;
    }

    // [1, n]でaと素な数の個数
    // 2^{aの素因数の種類} <= aの約数の数 なので O(sqrt(a))
    // 1 <= a <= Mについて呼び出すと O(MlogM)
    uint64_t count_coprime(int a, uint64_t n) {
        assert(a < min_factor.size());
        // (a < 10^8) -> d(a) < 1000
        static std::array<int, 1000> table;
        table[0] = 1;
        int r = 1;
        while (a > 1) {
            int p = min_factor[a];
            while (a % p == 0) a /= p;
            int l = r;
            for (int i = 0; i < r; i++) {
                table[l++] = -table[i] * p;
            }
            r = l;
        }
        uint64_t res = 0;
        for (int i = 0; i < r; i++) {
            int x = table[i];
            if (x < 0) res -= n / -x;
            else res += n / x;
        }
        return res;
    }
};
#endif
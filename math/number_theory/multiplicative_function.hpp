#ifndef _MULTIPLICATIVE_FUNCTION_H_
#define _MULTIPLICATIVE_FUNCTION_H_
#include <vector>
#include <cmath>
#include <cassert>
#include <numeric>

template<typename T>
struct multiplicative_function {
  private:
    long long n;
    int sq;
    std::vector<bool> is_prime;
    std::vector<int> cnt_prime, primes;
    std::vector<long long> Q;

    static std::vector<long long> enumerate_quotients(long long x) {
        assert(x >= 0);
        if (x == 0) return {};
        long long sq = sqrtl(x);
        std::vector<long long> ans(sq);
        std::iota(ans.begin(), ans.end(), 1);
        if (x / sq == sq) sq--;
        for (long long i = sq; i >= 1; i--) ans.push_back(x / i);
        return ans;
    }
  
  public:
    multiplicative_function(long long _n) : n(_n), sq(sqrt(n)), is_prime(sq + 1, true), cnt_prime(sq + 1) {
        is_prime[0] = is_prime[1] = false;
        cnt_prime[0] = cnt_prime[1] = 0;
        for (int p = 2; p <= sq; p++) {
            cnt_prime[p] = cnt_prime[p - 1] + is_prime[p];
            if (!is_prime[p]) continue;
            primes.push_back(p);
            for (int i = 2 * p; i <= sq; i += p) is_prime[i] = false;
        }
        Q = enumerate_quotients(n);
    }

    std::vector<long long> quotients() const {
        return Q;
    }

    // res[i] := [0, Q[i]の素数の数
    std::vector<long long> counting_primes_quotients() const {
        assert(n >= 0);
        if (n == 0) return {};
        std::vector<int> Fprime = cnt_prime;
        int W = Q.size();
        std::vector<long long> f(W);
        for (int i = 0; i < W; i++) f[i] = Q[i] - 1;
        for (int p = 2; p <= sq; p++) {
            if (!is_prime[p]) continue;
            long long p2 = (long long)p * p;
            if (p2 <= sq) {
                for (int i = W - 1, j = W - 1; i >= 0 && Q[i] >= p2; i--) {
                    while (j >= 0 && Q[j] * p > Q[i]) j--;
                    f[i] -= f[j] - Fprime[p - 1];
                }
            } else {
                for (int i = W - 1; i >= 0 && Q[i] >= p2; i--) {
                    long long x = Q[i] / p;
                    int j = (x <= sq ? x - 1 : W - n /x);
                    f[i] -= f[j] - Fprime[p - 1];
                }
            }
        }
        return f;
    }

    // res[i] := [0, Q[i]の素数の和
    std::vector<T> summation_primes_quotients() const {
        assert(n >= 0);
        if (n == 0) return {};
        std::vector<T> Fprime(sq + 1, 0);
        for (int p = 2; p <= sq; p++) {
            Fprime[p] = Fprime[p - 1] + (is_prime[p] ? p : 0);
        }
        int W = Q.size();
        std::vector<T> f(W);
        for (int i = 0; i < W; i++) f[i] = (T(Q[i]) * (Q[i] + 1)) / 2 - 1;
        for (int p = 2; p <= sq; p++) {
            if (!is_prime[p]) continue;
            long long p2 = (long long)p * p;
            if (p2 <= sq) {
                for (int i = W - 1, j = W - 1; i >= 0 && Q[i] >= p2; i--) {
                    while (j >= 0 && Q[j] * p > Q[i]) j--;
                    f[i] -= p * (f[j] - Fprime[p - 1]);
                }
            } else {
                for (int i = W - 1; i >= 0 && Q[i] >= p2; i--) {
                    long long x = Q[i] / p;
                    int j = (x <= sq ? x - 1 : W - n / x);
                    f[i] -= p * (f[j] - Fprime[p - 1]);
                }
            }
        }
        return f;
    }

    // res[i] := [0, Q[i]の素数pのa + bpの和
    std::vector<T> a_bp_quotients(T a, T b) const {
        assert(n >= 0);
        if (n == 0) return {};
        std::vector<T> Fprime(sq + 1, 0), Gprime(sq + 1, 0);

        for (int p = 2; p <= sq; p++) {
            Fprime[p] = Fprime[p - 1] + (is_prime[p] ? p : 0);
            Gprime[p] = Gprime[p - 1] + (is_prime[p] ? 1 : 0);
        }
        int W = Q.size();
        std::vector<T> f(W), g(W);
        for (int i = 0; i < W; i++) {
            f[i] = (T(Q[i]) * (Q[i] + 1)) / 2 - 1;
            g[i] = Q[i] - 1;
        }
        for (int p = 2; p <= sq; p++) {
            if (!is_prime[p]) continue;
            long long p2 = (long long)p * p;
            if (p2 <= sq) {
                for (int i = W - 1, j = W - 1; i >= 0 && Q[i] >= p2; i--) {
                    while (j >= 0 && Q[j] * p > Q[i]) j--;
                    f[i] -= p * (f[j] - Fprime[p - 1]);
                    g[i] -= (g[j] - Gprime[p - 1]);
                }
            } else {
                for (int i = W - 1; i >= 0 && Q[i] >= p2; i--) {
                    long long x = Q[i] / p;
                    int j = (x <= sq ? x - 1 : W - n / x);
                    f[i] -= p * (f[j] - Fprime[p - 1]);
                    g[i] -= (g[j] - Gprime[p - 1]);
                }
            }
        }
        for (int i = 0; i < W; i++) f[i] = f[i] * b + g[i] * a;
        return f;
    }

    long long counting_primes() const {
        if (n == 0) return 0;
        return counting_primes_quotients().back();
    }

    T summation_primes() const {
        if (n == 0) return 0;
        return summation_primes_quotients().back();
    }

    T totient_sum_primes() const {
        if (n == 0) return 0;
        return a_bp_quotients(-1, 1).back();
    }

    T totient_sum() const {
        auto Fprime = a_bp_quotients(-1, 1);
        int W = Q.size();

        auto dfs = [&](auto &&dfs, int L, long long x, T f) -> T {
            T res = 0;
            int last_p = (L == 0 ? 0 : primes[L - 1]);
            long long y = n / x;
            int idx = (y <= sq ? y - 1 : W - x);
            if (last_p == 0) {
                res += Fprime[idx];
            } else {
                res += f * last_p;
                if ((long long)last_p * last_p <= y) res += dfs(dfs, L, x * last_p, f * last_p);
                if (idx >= last_p) res += f * (Fprime[idx] - Fprime[last_p - 1]);
            }
            for (int i = L; i < int(primes.size()); i++) {
                int p = primes[i];
                if ((long long)p * p > y) break;
                res += f * dfs(dfs, i + 1, x * p, p - 1);
            }
            return res;
        };
        return dfs(dfs, 0, 1, 1) + 1;
    }

    // f(p^e) = ae + bpのsum
    T ae_bp(T a, T b) const {
        auto Fprime = a_bp_quotients(a, b);
        int W = Q.size();

        auto dfs = [&](auto &&dfs, int L, long long x, T f) -> T {
            T res = 0;
            int last_p = (L == 0 ? 0 : primes[L - 1]);
            long long y = n / x;
            int idx = (y <= sq ? y - 1 : W - x);
            if (last_p == 0) {
                res += Fprime[idx];
            } else {
                res += f + a;
                if ((long long)last_p * last_p <= y) res += dfs(dfs, L, x * last_p, f + a);
                if (idx >= last_p) res += f * (Fprime[idx] - Fprime[last_p - 1]);
            }
            for (int i = L; i < int(primes.size()); i++) {
                int p = primes[i];
                if ((long long)p * p > y) break;
                res += f * dfs(dfs, i + 1, x * p, a + b * p);
            }
            return res;
        };
        return dfs(dfs, 0, 1, 1) + 1;
    }

    long long mobius_sum() {
        std::vector<long long> Fprime;
        for (auto x : counting_primes_quotients()) {
            Fprime.push_back(-(long long)x);
        }
        int W = Q.size();

        auto dfs = [&](auto &&dfs, int L, long long x, T f) -> T {
            T res = 0;
            int last_p = (L == 0 ? 0 : primes[L - 1]);
            long long y = n / x;
            int idx = (y <= sq ? y - 1 : W - x);
            if (last_p == 0) {
                res += Fprime[idx];
            } else {
                if (idx >= last_p) res += f * (Fprime[idx] - Fprime[last_p - 1]);
            }
            for (int i = L; i < int(primes.size()); i++) {
                int p = primes[i];
                if ((long long)p * p > y) break;
                res += f * dfs(dfs, i + 1, x * p, -1);
            }
            return res;
        };
        return dfs(dfs, 0, 1, 1) + 1;
    }
};
#endif
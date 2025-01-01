#ifndef _LUCAS_H_
#define _LUCAS_H_
#include "../modint/modint32_dynamic.hpp"
#include "../modint/combination.hpp"

// mod == 2: 定数時間
// modが素数: 初期化 O(mod), クエリ O(log(n))
template<int id>
struct lucas_prime {
    using mint = modint32_dynamic<id>;
    using cmb = combination_mod<mint>;
    
    static void set_mod(int mod) {
        assert(0 < mod && mod < 1e7);
        mint::set_mod(mod);
        cmb::build(mint::mod() - 1);
    }

    static int comb(long long n, long long r) {
        if (mint::mod() == 1 || n < 0 || r < 0 || n < r) return 0;
        if (mint::mod() == 2) return (n & r) == r;
        mint res = 1;
        while (n) {
            int x = n % mint::mod(), y = r % mint::mod();
            n /= mint::mod(), r /= mint::mod();
            res *= cmb::comb(x, y);
        }
        return res.val();
    }

    // sum(r) = n
    static int comb_multi(long long n, const std::vector<long long> &r) {
        if (mint::mod() == 1 || n < 0) return 0;

        {
            long long S = 0, O = 0;
            for (long long x : r) {
                if (n < x || x < 0) return 0;
                S += x;
                O |= x;
            }
            if (S != n) return 0;
            if (mint::mod() == 2) {
                return n == O;
            }
        }

        mint res = 1;
        std::vector<int> tmp(r.size());
        while (n) {
            int x = n % mint::mod();
            n /= mint::mod();
            for (int i = 0; i < r.size(); i++) {
                tmp[i] = r[i] % mint::mod();
                r[i] /= mint::mod();
            }
            res *= cmb::comb_multi(x, tmp);
        }
        return res.val();
    }
};

template<int id>
struct lucas_prime_power {
  private:
    static int p, q, P;
    static std::vector<int> F, FI;
    static barrett br;
    static int _kth_comb(int a, int b, int c) {
        return br.mul(F[a], br.mul(FI[b], FI[c]));
    }

    static int _comb_p2q1(long long n, long long r) {
        assert(p == 2 && q == 1);
        return (n & r) == r;
    }

    static int _comb_q1(long long n, long long r) {
        assert(q == 1);
        int res = 1;
        while (n) {
            int x = n % p, y = r % p;
            if (x < y) return 0;
            n /= p, r /= p;
            res = br.mul(res, br.mul(F[x], br.mul(FI[y], FI[x - y])));
        }
        return res;
    }
 public:

    // mod p^q
    static void set_mod(int _p, int _q) {
        p = _p;
        q = _q;
        P = 1;
        for (int i = 0; i < q; i++) P *= p;
        F.resize(P);
        FI.resize(P);
        br = barrett(P);

        F[0] = 1;
        for (int i = 1, j = p; i < P; i++) {
            if (i == j) {
                F[i] = F[i - 1];
                j += p;
            } else {
                F[i] = br.mul(F[i - 1], i);
            }
        }
        FI[P - 1] = inv_gcd(F.back(), P).second;
        for (int i = P - 1, j = P - p; i > 0; i--) {
            if (i == j) {
                FI[i - 1] = FI[i];
                j -= p;
            } else {
                FI[i - 1] = br.mul(FI[i], i);
            }
        }
    }

    static int comb(long long n, long long r) {
        if (n < r || r < 0) return 0;
        if (n == 0) return 1;
        
        if (p == 2 && q == 1) return _comb_p2q1(n, r);
        if (q == 1) return _comb_q1(n, r);

        int x = 1;
        long long m = n - r;
        std::vector<int> k;

        while (n) {
            x = br.mul(x, _kth_comb(n % P, m % P, r % P));
            if (x == 0) return 0;
            bool eps = k.empty() ? 0 : k.back();
            k.push_back(m % p + r % p + eps >= p);
            n /= p, m /= p, r /= p;
        }
        
        for (int i = (int)k.size() - 2; i >= 0; i--) k[i] += k[i + 1];
        
        int y = 1;
        for (int i = 0; i < k[0]; i++) y = br.mul(y, p);
        if ((p != 2 || q <= 2) && k.size() > q - 1 && k[q - 1] % 2 == 1) {
            x = (x == 0 ? 0 : P - x);
        }
        return br.mul(x, y);
    }
};

template<int id>
int lucas_prime_power<id>::p = 0;
template<int id>
int lucas_prime_power<id>::q = 0;
template<int id>
int lucas_prime_power<id>::P = 0;
template<int id>
std::vector<int> lucas_prime_power<id>::F;
template<int id>
std::vector<int> lucas_prime_power<id>::FI;
template<int id>
barrett lucas_prime_power<id>::br(998244353);

#endif
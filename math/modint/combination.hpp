#ifndef _COMBINATION_H_
#define _COMBINATION_H_

#include <vector>
template<typename mint>
struct combination_mod {
  private:
    static int N;
    static std::vector<mint> F, FI, I;

  public:
    static bool built() { return !F.empty(); }

    static void clear() { N = 0; }

    // [0, N]を扱えるようにする
    // dynamic modint 等でmodを変えて再びbuildするときはclearを呼んでおく
    // O(logMOD + 増えた分)
    static void build(int _N) {
        _N++;
        assert(0 < _N && _N <= mint::mod());
        if (N >= _N) return;
        
        int preN = N;
        N = _N;
        F.resize(N);
        FI.resize(N);
        I.resize(N);
    
        F[0] = 1;
        for (int i = std::max(1, preN); i < N; i++) {
            F[i] = F[i - 1] * i;
        }
        FI[N - 1] = mint(F[N - 1]).inv();
        
        for (int i = N - 1; i >= std::max(1, preN); i--) {
            FI[i - 1] = FI[i] * i;
            I[i] = FI[i] * F[i - 1];
        }
    }

    static mint inv(int k) {
        return I[k];
    }

    using TypeMod = typename std::invoke_result<decltype(&mint::mod)>::type; // modintの内部的な整数型
    
    static mint inv_large(TypeMod k) {
        if constexpr (std::is_same<TypeMod, int>::value) {
            long long res = 1;
            while (k >= N) {
                int q = -(mint::mod() / k);
                res *= q;
                res %= mint::mod();
                k = mint::mod() + q * k;
            }
            return mint(res) * I[k];
        } else {
            mint res = 1;
            while (k >= N) {
                TypeMod q = -(mint::mod() / k);
                res *= q;
                k = mint::mod() + q * k;
            }
            return res * I[k];
        }
    }

    static mint fac(int k) {
        return F[k];
    }

    static mint ifac(int k) {
        return FI[k];
    }

    static mint comb(int a, int b) {
        if (a < b || b < 0) return 0;
        return F[a] * FI[a - b] * FI[b];
    }

    static mint icomb(int a, int b) {
        assert(a >= b && b >= 0);
        return FI[a] * F[a - b] * F[b];
    }
    
    // O(b)
    static mint comb_small(int a, int b) {
        assert(b < mint::mod());
        if (a < b) return 0;
        mint res = 1;
        for (int i = 0; i < b; i++) res *= a - i;
        return res * FI[b];
    }

    // O(|b|) sum(b) = a
    static mint comb_multi(int a, const std::vector<int> &b) {
        mint res = 1;
        for (int r : b) {
            res *= comb(a, r);
            a -= r;
        }
        if (a == 0) return res;
        return 0;
    }

    static mint perm(int a, int b) {
        if (a < b || b < 0) return 0;
        return F[a] * FI[a - b];
    }

    static mint iperm(int a, int b) {
        assert(a >= b && b >= 0);
        return FI[a] * F[a - b];
    }

    // O(b)
    static mint perm_small(int a, int b) {
        assert(b < mint::mod());
        if (a < b) return 0;
        mint res = 1;
        for (int i = 0; i < b; i++) res *= a - i;
        return res;
    }
};

template<typename mint>
int combination_mod<mint>::N = 0;
template<typename mint>
std::vector<mint> combination_mod<mint>::F;
template<typename mint>
std::vector<mint> combination_mod<mint>::FI;
template<typename mint>
std::vector<mint> combination_mod<mint>::I;
#endif
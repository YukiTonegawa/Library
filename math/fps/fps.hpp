#ifndef _FPS_H_
#define _FPS_H_
#include <vector>
#include "../modint/modint32_static.hpp"
#include "../number_theory/sqrt_mod.hpp"
#include "../fft/convolution_mod.hpp"

template<typename mint>
struct fps_base {
    // inv(2^k)
    static mint ipow2(int k) {
        static constexpr int max_size = 30;
        assert(k < max_size);
        static std::vector<mint> table;
        if (table.empty()) {
            mint i2 = mint(2).inv();
            table.resize(max_size, 1);
            for (int i = 1; i < max_size; i++) {
                table[i] = table[i - 1] * i2;
            }
        }
        return table[k];
    }

    static mint inv_low(int i) {
        static std::vector<mint> table{0};
        if (table.size() <= i) {
            int N = table.size();
            table.resize(i + 1);
            for (int j = N; j < int(table.size()); j++) {
                table[j] = mint(j).inv();
            }
        }
        return table[i];
    }
};

// ntt_friendly専用 
template<typename mint>
struct fps : std::vector<mint> {
    using std::vector<mint>::vector;
    using self_t = fps<mint>;
    using mint_t = mint;

    fps(const std::vector<mint> &v) : std::vector<mint>::vector(v) {}

    int size() const {
        return std::vector<mint>::size();
    }

    bool empty() const {
        return size() == 0;
    }

    self_t inv(int K = -1) const {
        int N = size();
        assert(N != 0);
        if (K == -1) K = N;
        self_t g{(*this)[0].inv()}, tmp, tmp2;
        int step = 0;
        for (int k = 1; k < K; k *= 2, step++) {
            int sz = 4 * k;
            self_t tmp = g;
            self_t tmp2(this->begin(), this->begin() + std::min(N, 2 * k));
            tmp.resize(sz, 0);
            tmp2.resize(sz, 0);
            butterfly(tmp);
            butterfly(tmp2);
            for (int j = 0; j < sz; j++) tmp[j] = tmp[j] * tmp[j] * tmp2[j];
            butterfly_inv(tmp);
            sz = std::min(K, 2 * k);
            g.resize(sz, 0);
            for (int j = k; j < sz; j++) {
                g[j] = -tmp[j] * fps_base<mint>::ipow2(step + 2);
            }
        }
        return g;
    }

    self_t prefix(int K = -1) const {
        if (K == -1) K = size();
        K = std::min(K, size());
        return self_t(std::vector<mint>(this->begin(), this->begin() + K));
    }

    self_t suffix(int K = -1) const {
        int s = size();
        if (K == -1) K = s;
        K = std::min(K, s);
        return self_t(std::vector<mint>(this->begin() + s - K, this->end()));
    }

    self_t reverse(int K = -1) const {
        if (K == -1) K = size();
        K = std::min(K, size());
        self_t res(std::vector<mint>(this->begin(), this->begin() + K));
        std::reverse(res.begin(), res.end());
        return res;
    }

    self_t diff() const {
        int N = size();
        self_t res(std::max(0, N - 1));
        for (int i = 1; i < N; i++) {
            res[i - 1] = (*this)[i] * i;
        }
        return res;
    }

    self_t integral() const {
        int N = size();
        self_t res(N + 1);
        res[0] = 0;
        for (int i = 0; i < N; i++) {
            res[i + 1] = (*this)[i] * fps_base<mint>::inv_low(i + 1); 
        }
        return res;
    }

    self_t log(int K = -1) const {
        assert(!empty() && (*this)[0] == 1);
        if (K == -1) K = size();
        return (diff() * inv(K)).prefix(K - 1).integral();
    }

    self_t exp(int K = -1) const {
        assert(!empty() && (*this)[0] == 0);
        if (K == -1) K = size();
        self_t f{1}, g{1};
        int step = 0;
        self_t d = diff();
        for (int k = 1; k < K; k *= 2, step++) {
            mint i2 = fps_base<mint>::ipow2(step + 1);
            self_t fd = f.diff();
            self_t fpre = f;
            f.resize(2 * k, 0), g.resize(2 * k, 0);
            self_t tmp = g;
            butterfly(tmp);
            butterfly(f);
            for (int j = 0; j < 2 * k; j++) tmp[j] = tmp[j] * tmp[j] * f[j];
            butterfly_inv(tmp);
            for (int j = 0; j < k; j++) {
                g[j] = 2 * g[j] - tmp[j] * i2;
            }
            self_t q(d.begin(), d.begin() + std::min(d.size(), k - 1));
            self_t w = q;
            q.resize(2 * k, 0);
            butterfly(q);
            fd.resize(2 * k, 0);
            butterfly(fd);
            for (int j = 0; j < 2 * k; j++) q[j] = fd[j] - q[j] * f[j];
            w.resize(2 * k - 1, 0);
            self_t G = g;
            butterfly(G);
            for (int j = 0; j < 2 * k; j++) G[j] *= q[j];
            butterfly_inv(G);
            for (int j = k - 1; j < 2 * k - 1; j++) w[j] = G[j] * i2;
            w = w.integral();
            for (int j = 0; j < 2 * k; j++) {
                mint x = (j < size() ? (*this)[j] : 0);
                w[j] = x - w[j];
            }
            w[0]++;
            butterfly(w);
            for (int j = 0; j < 2 * k; j++) w[j] *= f[j];
            butterfly_inv(w);
            for (int j = 0; j < k; j++) f[j] = fpre[j];
            for (int j = k; j < 2 * k; j++) f[j] = w[j] * i2;
        }
        return f.prefix(K);
    }

    // f^b(x)
    self_t pow(long long b, int K = -1) const {
        assert(b >= 0);
        int N = size();
        if (K == -1) K = N;
        if (!b) return {1};
        int i = 0;
        while (i < N && (*this)[i] == 0) i++;
        long long min_deg = i;
        if (__builtin_mul_overflow(min_deg, b, &min_deg) || min_deg >= K || i >= N) {
            return self_t(K, 0);
        }
        mint iinv = (*this)[i].inv();
        self_t res(K - min_deg, 0);
        for (int j = i; j < std::min(N, (int)res.size() + i); j++) {
            res[j - i] = (*this)[j] * iinv;
        }
        res = res.log(-1);
        for (auto &x : res) x *= b;        
        res = res.exp(-1);
        mint ipow = (*this)[i].pow(b);
        for (auto &x : res) x *= ipow;
        self_t tmp(min_deg, 0);
        res.insert(res.begin(), tmp.begin(), tmp.end());
        return res;
    }

    std::pair<bool, self_t> sqrt(int K = -1) const {
        int N = size();
        if (K == -1) K = N;
        int d = 0;
        while (d < N && (*this)[d] == 0) d++;
        if (d == N || d / 2 >= K) return {true, self_t(K, 0)};
        if (d & 1) return {false, {}};
        auto [f, b0] = sqrt_mod<mint>((*this)[d]);
        if (!f) return {false, {}};
        self_t res{b0};
        mint i2 = fps_base<mint>::ipow2(1);
        K -= d / 2;
        for(int i = 1; i < K; i *= 2) {
            int M = std::min(K, 2 * i);
            res.resize(M, 0);
            self_t resi = res.inv(M) * self_t(this->begin() + d, this->begin() + std::min(N, d + M));
            for (int j = 0; j < M; j++) {
                res[j] = (res[j] + resi[j]) * i2;
            }
        }
        res.resize(K);
        self_t tmp(d / 2, 0);
        res.insert(res.begin(), tmp.begin(), tmp.end());
        return {true, res};
    }

    // q(x)g(x)+r(x) == thisを満たす{q(x), r(x)} 多項式の除算
    std::pair<self_t, self_t> division_polynomial(const self_t &g) const {
        int N = size(), M = g.size(), K = N - M + 1;
        assert(M != 0);
        if (N < M) return {{}, *this};
        self_t A(K), B(std::min(M, K), 0);
        for (int i = 0; i < K; i++) A[i] = (*this)[N - 1 - i];
        for (int i = 0; i < std::min(K, M); i++) B[i] = g[M - 1 - i];
        if (A.size() > K) A.resize(K);
        A *= B.inv(K);
        B.resize(K);
        for (int i = 0; i < K; i++) B[K - 1 - i] = A[i];
        A = B * g;
        A.resize(M - 1);
        for (int i = 0; i < M - 1; i++) A[i] = (*this)[i] - A[i];
        while (!A.empty() && A.back() == 0) A.pop_back();
        while (!B.empty() && B.back() == 0) B.pop_back();
        return {B, A};
    }

    // operator
    self_t operator - () const { self_t res(*this); for (int i = 0; i < size(); i++) res[i] = -res[i]; return res; }
    self_t operator += (const self_t &B) {
        if (size() < B.size()) this->resize(B.size(), 0);
        for (int i = 0; i < int(B.size()); i++) (*this)[i] += B[i];
        return *this;
    }
    self_t operator + (const self_t &B) const { self_t res(*this); return res += B; }
    self_t operator -= (const self_t &B) {
        if (size() < B.size()) this->resize(B.size(), 0);
        for (int i = 0; i < int(B.size()); i++) (*this)[i] -= B[i];
        return *this;
    }
    self_t operator - (const self_t &B) const { self_t res(*this); return res -= B; }
    self_t operator * (const self_t &B) const { return self_t(convolution_fft<mint>(*this, B)); }
    self_t operator *= (const self_t &B) { return *this = *this * B; }
    self_t operator /= (const self_t &B) { return *this *= B.inv(); }
    self_t operator / (const self_t &B) const { self_t res(*this); return res /= B; }
    self_t operator %= (const self_t &B) { return *this = division_polynomial(B).second; }
    self_t operator % (const self_t &B) const { self_t res(*this); return res %= B; }

    self_t operator += (const mint &B) {
        if (size() == 0) this->push_back(B);
        else (*this)[0] += B;
        return *this;
    }
    self_t operator + (const mint &B) const { self_t res(*this); return res += B; }
    self_t operator -= (const mint &B) {
        if (size() == 0) this->push_back(-B);
        else (*this)[0] -= B;
        return *this;
    }
    self_t operator - (const mint &B) const { self_t res(*this); return res -= B; }
    self_t operator * (const mint &B) const { self_t res(*this); return res *= B; }
    self_t operator *= (const mint &B) { for (int i = 0; i < size(); i++) (*this)[i] *= B; return *this; }
    self_t operator /= (const mint &B) { mint Bi = B.inv(); for (int i = 0; i < size(); i++) (*this)[i] *= Bi; return *this;  }
    self_t operator / (const mint &B) const { self_t res(*this); return res /= B; }
};
#endif
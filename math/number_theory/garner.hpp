#ifndef _GARNER_H_
#define _GARNER_H_
#include <vector>
#include <cassert>
#include <cstdint>
#include <unordered_map>
#include "../base/gcd.hpp"
#include "rho.hpp"

// m_iで割るとr_i余る情報から(全ての条件を満たすlcm(m_i)以下の数)%modを返す
// mが互いに素でないと壊れる
// O(n^2)
long long garner_coprime(const std::vector<long long> &r, std::vector<long long> m, long long mod) {
    int n = r.size();
    assert(n == m.size());
    
    std::vector<long long> accum(n + 1, 0), M(n + 1, 1);
    m.push_back(mod);
    
    for (int i = 0; i < n; i++) {
        auto [g, inv] = inv_gcd(M[i], m[i]);
        assert(g == 1);
        long long x = (((__int128_t)r[i] - accum[i]) * inv) % m[i];
        if (x < 0) x += m[i];
        for (int j = i + 1; j <= n; j++) {
            accum[j] = (accum[j] + (__int128_t)M[j] * x) % m[j];
            M[j] = ((__int128_t)M[j] * m[i]) % m[j];
        }
    }
    return accum[n];
}

// m_iで割るとr_i余る情報から(全ての条件を満たすlcm(m_i)以下の数)%modを返す
// 解が無い場合は-1
// O(n^2 + ∑m_i^(1/4)log(m_i))
long long garner(std::vector<long long> r, std::vector<long long> m, long long mod) {
    int n = r.size();
    assert(n == m.size());
    
    std::unordered_map<long long, std::pair<long long, int>> P;
    for (int i = 0; i < n; i++) {
        auto f = rho::factorize(m[i]);
        std::sort(f.begin(), f.end());
        f.push_back(0);
        long long p = -1, p_pow = 1;
        for (int j = 0; j < f.size(); j++) {
            if (f[j] == p) {
                p_pow *= p;
            } else {
                if (p != -1) {
                    auto itr = P.find(p);
                    if (itr == P.end()) {
                        P.emplace(p, std::make_pair(p_pow, i));
                    } else {
                        auto [p_pow_max, idx_max] = itr->second;
                        long long p_pow_gcd = std::min(p_pow_max, p_pow);
                        if (r[idx_max] % p_pow_gcd != r[i] % p_pow_gcd) return -1; // 解なし, 例: 2で割って1余りつつ0余る数
                        if (p_pow_max < p_pow) {
                            m[idx_max] /= p_pow_max;
                            r[idx_max] %= m[idx_max];
                            itr->second = std::make_pair(p_pow, i);
                        } else {
                            m[i] /= p_pow;
                            r[i] %= m[i];
                        }
                    }
                }
                p = f[j];
                p_pow = p;
            }
        }
    }
    // rが全て0の場合答えは0になるが, 正の数が欲しい場合はlcm(m)が答え
    /*
    bool is_all_zero = true;
    long long _lcm = 1 % mod;
    for(int i = 0; i < n; i++){
        if(r[i]){
        is_all_zero = false;
        break;
        }
        _lcm = (__int128_t)_lcm * m[i] % mod;
    }
    if(is_all_zero) return _lcm;
    */
    return garner_coprime(r, m, mod);
}


// mが固定の場合, 定数の前計算やmontgomery reductionによる高速化ができる
// 解が無いと壊れる
struct fixed_mod_garner{
    std::vector<unsigned long long> m, iM, accum;
    std::vector<std::vector<unsigned long long>> M;
    std::vector<montgomery64> mr;

    void set_mod(std::vector<unsigned long long> _m) {
        int n = _m.size();
        std::unordered_map<long long, std::pair<long long, int>> P;
        for (int i = 0; i < n; i++) {
            auto f = rho::factorize(_m[i]);
            std::sort(f.begin(), f.end());
            f.push_back(0);
            long long p = -1, p_pow = 1;
            for (int j = 0; j < f.size(); j++) {
                if (f[j] == p) {
                    p_pow *= p;
                } else {
                    if (p != -1) {
                        auto itr = P.find(p);
                        if (itr == P.end()) {
                            P.emplace(p, std::make_pair(p_pow, i));
                        } else {
                            auto [p_pow_max, idx_max] = itr->second;
                            if (p_pow_max < p_pow) {
                                _m[idx_max] /= p_pow_max;
                                itr->second = std::make_pair(p_pow, i);
                            } else {
                                _m[i] /= p_pow;
                            }
                        }
                    }
                    p = f[j];
                    p_pow = p;
                }   
            }
        }
        m = _m;
        mr.resize(n + 1);
        for (int i = 0; i < n; i++) mr[i].set_mod(m[i]);
        M.resize(n + 1, std::vector<unsigned long long>(n + 1));
        for (int i = 0; i < n; i++) {
            M[i][0] = mr[i].generate(1);
            for (int j = 1; j <= i; j++) {
                M[i][j] = mr[i].mul(M[i][j - 1], mr[i].generate(m[j - 1]));
            }
        }
        iM.resize(n);
        for (int i = 0; i < n; i++) iM[i] = mr[i].generate(inv_gcd(mr[i].reduce(M[i][i]), m[i]).second);
        accum.resize(n + 1, 0);
    }

    unsigned long long query(std::vector<unsigned long long> r, unsigned long long mod) {
        int n = r.size();
        assert(n == m.size());
        std::fill(accum.begin(), accum.end(), 0);
        mr[n].set_mod(mod);
        M[n][0] = mr[n].generate(1);
        for (int i = 1; i <= n; i++) M[n][i] = mr[n].mul(M[n][i - 1], mr[n].generate(m[i - 1]));
        for (int i = 0; i < n; i++) {
            r[i] = mr[i].generate(r[i] % m[i]);
            unsigned long long x = mr[i].reduce(mr[i].mul(mr[i].sub(r[i], accum[i]), iM[i]));
            for (int j = i + 1; j <= n; j++) accum[j] = mr[j].add(accum[j], mr[j].mul(M[j][i], mr[j].generate(x)));
        }
        return mr[n].reduce(accum[n]);
    }
};

#endif
#ifndef _BASE_BIGBINARY_H_
#define _BASE_BIGBINARY_H_
#include "../fft/convolution_mod.hpp"
#include "../fft/square_mod.hpp"
#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>

struct base_bigbinary {
    using i64 = long long;
    using i128 = __int128_t;
    static constexpr std::array<i64, 62> pow2 = [](){
        std::array<i64, 62> res;
        res[0] = 1;
        for (int i = 1; i < 62; i++) res[i] = res[i - 1] * 2;
        return res;
    }();
    static constexpr int base_log = 60;
    static constexpr i64 base = i64(1) << base_log;
    static constexpr i128 base_sq = (i128)base * base;

    // leading zeroがない
    // 0文字目の+/-は無視する
    static std::vector<i64> to_vector(const std::string &S) {
        if (S.empty()) return {0};
        int n = S.size();
        int l = (S[0] == '+' || S[0] == '-');
        std::vector<i64> res;
        std::string tmp = "";
        for (int i = n - 1; i >= l; i--) {
            tmp += S[i];
            if (tmp.size() == base_log || i == l) {
                std::reverse(tmp.begin(), tmp.end());
                i64 x = 0;
                for (int j = 0; j < int(tmp.size()); j++) {
                    x = (x << 1) + (tmp[j] == '1');
                }
                res.push_back(x);
                tmp = "";
            }
        }
        return res;
    }

    static std::string to_string(const std::vector<i64> &a) {
        std::string res = "";
        int n = int(a.size());
        bool nonzero = false;
        for (int i = n - 1; i >= 0; i--) {
            if (a[i] && !nonzero) {
                nonzero = true;
                int msb = 63 - __builtin_clzll(a[i]);
                for (int j = msb; j >= 0; j--) {
                    res += ((a[i] >> j) & 1) == 1 ? '1' : '0';
                }
            } else if (nonzero) {
                for (int j = base_log - 1; j >= 0; j--) {
                    res += ((a[i] >> j) & 1) == 1 ? '1' : '0';
                }
            }
        }
        if (!nonzero) res = '0';
        return res;
    }

    // 各桁がbase未満になるように繰り上げる
    template<typename T>
    static std::vector<i64> modify(const std::vector<T> &a) {
        i64 up = 0;
        std::vector<i64> res(a.size(), 0);
        for (int i = 0; i < a.size(); i++) {
            i128 tmp = a[i] + up;
            up = tmp / base;
            res[i] = tmp - (i128)up * base;
        }
        if (up) res.push_back(up);
        return res;
    }

    static int deg(const std::vector<i64> &a) {
        int n = a.size();
        for (int i = n - 1; i >= 0; i--) {
            if (a[i]) {
                for (int j = base_log - 1; j >= 0; j--) {
                    if (a[i] >= pow2[j]) return i * base_log + j;
                }
            }
        }
        return -1; // 0
    }

    static std::vector<i64> lshift(const std::vector<i64> &a, int k) {
        assert(k >= 0);
        if (a.empty()) return {};
        if (k == 0) return a;
        int s = k / base_log, r = k % base_log;
        std::vector<i64> res(a.size() + (k + base_log - 1) / base_log, 0);
        if (r == 0) {
            for (int i = s; i < res.size(); i++) res[i] = a[i - s];
        } else {
            int n = int(res.size()), m = base_log - r;
            for (int i = n - 1; i > s; i--) {
                res[i] = (i - s >= a.size() ? 0 : a[i - s]) % pow2[m] * pow2[r] + a[i - s - 1] / pow2[m];
            }
            res[s] = (a[0] % pow2[m]) * pow2[r];
        }
        return res;
    }

    static std::vector<i64> rshift(const std::vector<i64> &a, int k) {
        assert(k >= 0);
        int s = k / base_log, r = k % base_log;
        if (a.size() <= s) return {};
        if (k == 0) return a;
        std::vector<i64> res(int(a.size()) - s, 0);
        if (r == 0) {
            for (int i = 0; i < res.size(); i++) res[i] = a[i + s];
        } else {
            int n = int(res.size()), m = base_log - r;
            for (int i = 0; i < n - 1; i++) {
                res[i] = a[i + s] / pow2[r] + a[i + s + 1] % pow2[r] * pow2[m];
            }
            res[n - 1] = a[n - 1 + s] / pow2[r];
        }
        return res;
    }

    static std::vector<i64> add(const std::vector<i64> &a, const std::vector<i64> &b) {
        int n = int(a.size()), m = int(b.size());
        if (n > m) return add(b, a);
        std::vector<i64> res = a;
        res.resize(m, 0);
        bool up = 0;
        for (int i = 0; i < m; i++) {
            res[i] += b[i] + up;
            if ((up = (res[i] >= base))) res[i] -= base;
        }
        if (up) res.push_back(1);
        return res;
    }

    // 最大の項以外は0以上base未満
    // 最大の項は a >= bなら0以上, a < bなら-1
    static std::vector<i64> sub(const std::vector<i64> &a, const std::vector<i64> &b) {
        int n = int(a.size()), m = int(b.size());
        std::vector<i64> res = a;
        if (n < m) res.resize(m, 0);
        bool down = 0;
        for (int i = 0; i < std::max(n, m); i++) {
            res[i] -= (i >= m ? 0 : b[i]) + down;
            if ((down = (res[i] < 0))) res[i] += base;
        }
        if (down) res.push_back(-1);
        return res;
    }

    struct mul_bigbinary {
        static constexpr i64 MOD1 = 754974721;
        static constexpr i64 MOD2 = 998244353;
        static constexpr i64 MOD3 = 167772161;
        static constexpr i64 MOD4 = 469762049;
        static constexpr i64 MOD5 = 1224736769;
        static constexpr i64 M1M2 = MOD1 * MOD2;
        static constexpr i128 M1M2M3 = (i128)M1M2 * MOD3;
        static constexpr i128 M1M2M3M4 = M1M2M3 * MOD4;
        static constexpr i128 Q = M1M2M3M4 / base;
        static constexpr i128 R = M1M2M3M4 % base;
        static constexpr i64 IM1_M2 = inv_gcd(MOD1, MOD2).second;
        static constexpr i64 IM1M2_M3 = inv_gcd(MOD1 * MOD2, MOD3).second;
        static constexpr i64 IM1M2M3_M4 = inv_gcd((i64)(M1M2M3 % MOD4), MOD4).second;
        static constexpr i64 IM1M2M3M4_M5 = inv_gcd((i64)(M1M2M3M4 % MOD5), MOD5).second;

        static std::vector<i64> crt_restore(const std::vector<i64> &c1, const std::vector<i64> &c2, const std::vector<i64> &c3, const std::vector<i64> &c4, const std::vector<i64> &c5) {
            int n = c1.size();
            std::vector<i64> c(n);
            i128 up = 0;
            for (int i = 0; i < n; i++) {
                i128 x = c1[i], y, t;
                t = ((c2[i] < x ? MOD2 + c2[i] - x : c2[i] - x) * IM1_M2) % MOD2;
                x += t * MOD1; // x < 2^60
                y = x % MOD3;
                t = ((c3[i] < y ? MOD3 + c3[i] - y : c3[i] - y) * IM1M2_M3) % MOD3;
                x += t * M1M2; // x < 2^90
                y = x % MOD4;
                t = ((c4[i] < y ? MOD4 + c4[i] - y : c4[i] - y) * IM1M2M3_M4) % MOD4;
                x += t * M1M2M3; // x < 2^120
                y = x % MOD5;
                t = ((c5[i] < y ? MOD5 + c5[i] - y : c5[i] - y) * IM1M2M3M4_M5) % MOD5;
                x += up + t * R;
                up = Q * t;
                i128 z = x / base;
                x -= base * z;
                up += z;
                c[i] = x;
            }
            if (up) c.push_back(up);
            return c;
        }

        static std::vector<i64> mul_naive(const std::vector<i64> &a, const std::vector<i64> &b) {
            int n = int(a.size()), m = int(b.size());
            std::vector<i128> c(n + m, 0);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    c[i + j] += (i128)a[i] * b[j];
                    if (c[i + j] >= base_sq) {
                        c[i + j] -= base_sq;
                        c[i + j + 1] += base;
                    }
                }
            }
            return base_bigbinary::modify<i128>(c);
        }

        static std::vector<i64> square_naive(const std::vector<i64> &a) {
            int n = int(a.size());
            std::vector<i128> c(2 * n, 0);
            for (int i = 0; i < n; i++) c[2 * i] = i128(a[i]) * a[i];
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    c[i + j] += 2 * i128(a[i]) * a[j];
                    while (c[i + j] >= base_sq) {
                        c[i + j] -= base_sq;
                        c[i + j + 1] += base;
                    }
                }
            }
            return base_bigbinary::modify<i128>(c);
        }

        static std::vector<i64> mul(const std::vector<i64> &a, const std::vector<i64> &b) {
            int n = int(a.size()), m = int(b.size());
            if (!n || !m) return {};
            if (std::min(n, m) <= 80) return mul_naive(a, b);
            auto c1 = convolution_mod<MOD1>(a, b);
            auto c2 = convolution_mod<MOD2>(a, b);
            auto c3 = convolution_mod<MOD3>(a, b);
            auto c4 = convolution_mod<MOD4>(a, b);
            auto c5 = convolution_mod<MOD5>(a, b);
            return crt_restore(c1, c2, c3, c4, c5);
        }

        static std::vector<i64> square(const std::vector<i64> &a) {
            int n = int(a.size());
            if (!n) return {};    
            if (n <= 80) return square_naive(a);
            auto c1 = square_mod<MOD1>(a);
            auto c2 = square_mod<MOD2>(a);
            auto c3 = square_mod<MOD3>(a);
            auto c4 = square_mod<MOD4>(a);
            auto c5 = square_mod<MOD5>(a);
            return crt_restore(c1, c2, c3, c4, c5);
        }
    };
};
#endif
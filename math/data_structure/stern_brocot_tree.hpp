#ifndef _STERN_BROCOT_TREE_H_
#define _STERN_BROCOT_TREE_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include <type_traits>
#include <iostream>
#include "../base/fraction.hpp"

// 分母の0を許容
// 0 <= a, b <= inf
// inf^2がオーバーフローしない
template<typename T, T inf>
struct stern_brocot_tree {
    using frac = fraction<T, inf>;
    
    // {{左右, 移動回数}}
    static std::vector<std::pair<bool, T>> encode_path(const frac &x) {
        assert(x.a > 0 && x.b > 0);
        frac L(0, 1), R(1, 0);
        std::vector<std::pair<bool, T>> res;
        while(true) {
            T a = (x.a * L.b - x.b * L.a);
            T b = (x.b * R.a - x.a * R.b);
            frac M(L.a + R.a, L.b + R.b);
            if (M <= x) {
                T k = a / b + 1;
                L = frac(L.a + (k - 1) * R.a, L.b + (k - 1) * R.b);
                res.push_back({1, k - 1});
                if (L == x) {
                    if (res.back().second > 1) res.back().second--;
                    else res.pop_back();
                    return res;
                }
            } else {
                std::swap(a, b);
                T k = a / b + 1;
                R = frac((k - 1) * L.a + R.a, (k - 1) * L.b + R.b);
                res.push_back({0, k - 1});
                if (R == x) {
                    if (res.back().second > 1) res.back().second--;
                    else res.pop_back();
                    return res;
                }
            }
        }
    }
    
    static frac decode_path(const std::vector<std::pair<bool, T>> &P) {
        frac L(0, 1), R(1, 0);
        for(auto [dir, k] : P) {
            if (dir) {
                L = frac(L.a + k * R.a, L.b + k * R.b);
            } else {
                R = frac(k * L.a + R.a, k * L.b + R.b);
            }
        }
        return frac(L.a + R.a, L.b + R.b);
    }

    // 分母がmax_d以下であるxの近似値(下側, 上側)
    // x <= 1なら分子も自動的にmax_d以下になる
    // x > 1で分子にも条件を課したいならxの分母と分子を入れ替えて計算する
    static std::pair<frac, frac> best_approximation(const frac &x, T max_d) {
        assert(x.b > 0 && max_d > 0);
        frac L(0, 1), R(1, 0);
        while (true) {
            T a = (x.a * L.b - x.b * L.a);
            T b = (x.b * R.a - x.a * R.b);
            if (a == 0) return {L, L};
            if (b == 0) return {R, R};
            if (L.b + R.b > max_d) return {L, R};
            frac M(L.a + R.a, L.b + R.b);
            if (M <= x) {
                T k = a / b + 1;
                if (R.b) k = std::min(k, (max_d - L.b) / R.b + 1);
                L = frac(L.a + (k - 1) * R.a, L.b + (k - 1) * R.b);
            } else {
                std::swap(a, b);
                T k = a / b + 1;
                if (L.b) k = std::min(k, (max_d - R.b) / L.b + 1);
                R = frac((k - 1) * L.a + R.a, (k - 1) * L.b + R.b);
            }
        }
    }

    // 値が(L, R)の部分木で分母がmax_d以下の最左ノード
    static frac leftmost(frac L, frac R, T max_d) {
        assert(L.b);
        T k = (max_d - R.b) / L.b;
        return frac(k * L.a + R.a, k * L.b + R.b);
    }

    // 値が(L, R)の部分木で分母がmax_d以下の最右ノード
    // R = ♾️だと壊れる
    static frac rightmost(frac L, frac R, T max_d) {
        assert(R.b);
        T k = (max_d - L.b) / R.b;
        return frac(L.a + k * R.a, L.b + k * R.b);
    }
    
    // 分母がmax_d以下の頂点のうちxより大きい最小のもの
    // x > 1だと壊れる可能性がある(分母 < infでも分子 > infになる可能性がある)
    static frac next_frac(const frac &x, T max_d){
        assert(0 < x.b && 0 < max_d);
        if (max_d < x.b) return best_approximation(x, max_d).second;
        auto [L, R] = range_subtree(x);
        // xの右部分木に答えがある
        if (L.b + 2 * R.b <= max_d) return leftmost(x, R, max_d);
        return R;
    }
    // 分母がmax_d以下の頂点のうちxより小さい最大のもの
    // x > 1だと壊れる可能性がある(分母 < infでも分子 > infになる可能性がある)
    static frac prev_frac(const frac &x, T max_d) {
        assert(0 < x.b && 0 < max_d);
        if (max_d < x.b) return best_approximation(x, max_d).first;
        auto [L, R] = range_subtree(x);
        // xの左部分木に答えがある
        if (2 * L.b + R.b <= max_d) return rightmost(L, x, max_d);
        return L;
    }

    // xの部分木が表す値の範囲
    static std::pair<frac, frac> range_subtree(const frac &x) {
        if (x.b == 1) return {frac(x.a - 1, 1), frac(1, 0)};
        return best_approximation(x, x.b - 1);
    }

    // xの深さ
    static T depth(const frac &x){
        T res = 0;
        for (auto [dir, t] : encode_path(x)) res += t;
        return res;
    }

    // xの祖先のうち深さkのもの(depth(x) < kの場合 {false, ...})
    static std::pair<bool, frac> ancestor(const frac &x, T k) {
        auto P = encode_path(x);
        T d = 0;
        for (auto [lr, t] : P) d += t;
        if (d < k) return {false, frac(0, 0)};
        while(d > k) {
            auto [lr, t] = P.back();
            if (d - t >= k){
                P.pop_back();
                d -= t;
            } else {
                P.back().second -= d - k;
                d = k;
            }
            if (d == k) break;
        }
        return {true, decode_path(P)};
    }

    // lca(x, y)
    static frac lca(const frac &x, const frac &y) {
        auto P1 = encode_path(x);
        auto P2 = encode_path(y);
        int m = std::min(P1.size(), P2.size());
        std::vector<std::pair<bool, T>> P3;
        for (int i = 0; i < m; i++) {
            if (P1[i] == P2[i]) {
                P3.push_back(P1[i]);
            } else {
                if (P1[i].first == P2[i].first) {
                    P3.push_back({P1[i].first, std::min(P1[i].second, P2[i].second)});
                }
                break;
            }
        }
        return decode_path(P3);
    }

    // 分母が　max_d以下の範囲を探索
    // f: frac -> bool
    // 分母max_d以下の分数の境界を返す {trueの最大, falseの最小}
    template<typename F>
    static std::pair<frac, frac> binary_search(F f, T max_d) {
        frac L(0, 1), R(1, 0);
        while (true) {
            if (L.b + R.b > max_d) return {L, R};
            frac M(L.a + R.a, L.b + R.b);
            if (f(M)) {
                T l = 2, r = 2;
                while (L.b + (r - 1) * R.b <= max_d && f(frac(L.a + (r - 1) * R.a, L.b + (r - 1) * R.b))) {
                    r *= 2;
                }
                r *= 2;
                while (r - l > 1) {
                    T m = l + (r - l) / 2;
                    if (L.b + (m - 1) * R.b <= max_d && f(frac(L.a + (m - 1) * R.a, L.b + (m - 1) * R.b))) {
                        l = m;
                    } else {
                        r = m;
                    }
                }
                L = frac(L.a + (l - 1) * R.a, L.b + (l - 1) * R.b);
            } else {
                T l = 2, r = 2;
                while (L.b * (r - 1) + R.b <= max_d && !f(frac(L.a * (r - 1) + R.a, L.b * (r - 1) + R.b))) {
                    r *= 2;
                }
                while (r - l > 1) {
                    T m = l + (r - l) / 2;
                    if (L.b * (m - 1) + R.b <= max_d && !f(frac(L.a * (m - 1) + R.a, L.b * (m - 1) + R.b))) {
                        l = m;
                    } else {
                        r = m;
                    }
                }
                R = frac(L.a * (l - 1) + R.a, L.b * (l - 1) + R.b);
            }
        }
    }

    // ternary_search
};
#endif
#ifndef _SLOPE_TRICK_BBST_H_
#define _SLOPE_TRICK_BBST_H_
#include "../../data_structure/ordered_set/multiset_sum.hpp"
#include <queue>
#include <vector>
#include <limits>

// Q回の操作でmin_fxがABQ^3程度になる可能性がある(A : 座標の最大, B : 関数の係数の最大値)
template<typename T>
struct slope_trick_bbst {
  private:
    static constexpr T xmin = std::numeric_limits<T>::min() / 2;
    static constexpr T xmax = std::numeric_limits<T>::max() / 2;
    T min_fx;
    multiset_sum<T> L, R;
    T max_l() { return L.empty() ? xmin : L.max(); }
    T min_r() { return R.empty() ? xmax : R.min(); }
    
  public:
    slope_trick_bbst() : min_fx(0) {}
    
    // f(x)
    T get(T x) {
        T l0 = max_l();
        T r0 = min_r();
        if (l0 <= x && x <= r0) return min_fx;
        // R中のx未満の要素(傾きが変わる点)をa1, a2...とすると
        // f(x) = min_fx + (x - a1) + (x - a2) + ...
        if (r0 < x) {
            T asum = R.sum_left(x); // x未満の要素の和
            T acnt = R.low_count(x); // x未満の要素の数
            return min_fx + acnt * x - asum;
        } else {
            T asum = L.sum_right(x); // x以上の要素の和
            T acnt = L.up_count(x); // x以上の要素の数
            return min_fx + asum - acnt * x;
        }
    }
    
    // 最小値
    T min() {
        return min_fx;
    }
    
    // 最小値を取る区間[l, r]
    std::pair<T, T> min_range() {
        return std::make_pair(max_l(), min_r());
    }
    
    // [l, r)のmin
    T range_min(T l, T r) {
        T l0 = max_l();
        T r0 = min_r();
        if (r0 < l) {
            return get(l);
        } else if (r <= l0) {
            return get(r - 1);
        } else {
            return min_fx;
        }
    }

    // 定数xを足す
    void add_const(T x) {
        min_fx += x;
    }

    // max(0, k * (a - x))を足す
    void add_left(T a, T k = 1) {
        assert(k > 0);
        if (min_r() < a) {
            R.insert(a, k);
            auto v = R.split_left(k);
            min_fx += a * k - v->sum;
            L.merge_right(v);
        } else {
            L.insert(a, k);
        }
    }
    
    // max(0, k * (x - a))を足す
    void add_right(T a, T k = 1) {
        assert(k > 0);
        if (a < max_l()) {
            L.insert(a, k);
            auto v = L.split_right(k);
            min_fx += v->sum - a * k;
            R.merge_left(v);
        } else {
            R.insert(a, k);
        }
    }
    
    // k * |x - a|を足す
    void add_abs(T a, T k = 1) {
        add_left(a, k);
        add_right(a, k);
    }

    // 左側をクリア ([xmin, 左側の傾きの変化が終わる点]をmin_fxにする)
    void accumulate_min_left() {
        L.clear();
    }
    
    // 右側をクリア ([右側の傾きが始まる点, xmax]をmin_fxにする)
    void accumulate_min_right() {
        R.clear();
    }

    // 左側を右にa, 右側を右にb平行移動する
    // l0 + a > r0 + bだと左右で交差して壊れる
    void shift(T a, T b) {
        T l0 = max_l();
        T r0 = min_r();
        assert(r0 - l0 >= a - b);
        L.add_all(a);
        R.add_all(b);
    }
    
    // 傾きが変わる点で分割(区間[l, r], ax + bで表される線分の集合になる)
    // {l, a, b}
    std::vector<std::tuple<T, T, T>> to_func() {
        std::vector<std::tuple<T, T, T>> res;
        auto tmpl = L.enumerate();
        std::reverse(tmpl.begin(), tmpl.end());
        T d = 0, y1 = min_fx;
        for (int i = 0; i < tmpl.size(); ) {
            auto [x0, cnt] = tmpl[i];
            T x1 = res.empty() ? x0 : std::get<0>(res.back());
            T c = -d * x1 + y1;
            res.push_back({x0, d, c});
            y1 = x0 * d + c;
            while (i < tmpl.size() && tmpl[i].first == x0) d -= tmpl[i++].second;
        }
        res.push_back({xmin, d, -d * (res.empty() ? 0 : std::get<0>(res.back())) + y1});
        std::reverse(res.begin(), res.end());
        auto tmpr = R.enumerate();
        d = 0;
        T y0 = min_fx;
        for (int i = 0; i < tmpr.size(); ) {
            auto [x0, cnt] = tmpr[i];
            while (i < tmpr.size() && tmpr[i].first == x0) d += tmpr[i++].second;
            res.push_back({x0, d, -d * x0 + y0});
            if (i < tmpr.size()) {
                T x1 = tmpr[i].first;
                y0 = std::get<1>(res.back()) * x1 + std::get<2>(res.back());
            }
        }
        return res;
    }  
};
#endif
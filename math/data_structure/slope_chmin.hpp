#ifndef _SLOPE_CHMIN_H_
#define _SLOPE_CHMIN_H_
#include "li_chao_tree.hpp"
#include "../../data_structure/sparse_table/sparse_table.hpp"
#include "../../data_structure/fenwick_tree/fenwick_tree.hpp"

// chtのenumerateを使って以下を列挙する
// {l, r, f} := [l, r)で直線or線分fが最小値をとる

// 以下のことができる
// f(x)をxにおける最小値とする
// range_min(l, r) : l <= x < rを満たす整数xの中でf(x)の最小値
// range_max(l, r) : l <= x < rを満たす整数xの中でf(x)の最大値
// range_sum(l, r) : l <= x < rを満たす整数xの中でf(x)の和
template<typename T, typename TSum>
struct static_slope_chmin {
  private:
    bool is_built = false;
    T xmin, xmax;
    li_chao_tree<T> cht;
    std::vector<T> lx;
    std::vector<typename li_chao_tree<T>::line> ln;
    std::vector<T> sum;

    static constexpr T merge_min(T a, T b) { return std::min(a, b); }
    static constexpr T min_id() { return std::numeric_limits<T>::max(); }
    static constexpr T merge_max(T a, T b) { return std::max(a, b); }
    static constexpr T max_id() { return std::numeric_limits<T>::min(); }

    // ak + b のl <= k < rにおける和
    // a / 2 * {r * (r - 1) - l * (l - 1)} + b * (r - l)
    static constexpr TSum line_sum(T a, T b, T l, T r) {
        return a * ((TSum)r * (r - 1) - (TSum)l * (l - 1)) / 2 + (TSum)b * (r - l);
    }
    sparse_table<T, merge_min, min_id> st_min;
    sparse_table<T, merge_max, max_id> st_max;
    fenwick_tree<TSum> bit;
  public:
    static_slope_chmin(T xmin, T xmax): xmin(xmin), xmax(xmax), cht(xmin, xmax) {}

    void add_line(T a, T b) {
        assert(!is_built);
        cht.add_line(a, b);
    }
    void add_segment(T l, T r, T a, T b) {
        assert(!is_built);
        cht.add_segment(l, r, a, b);
    }
    void build() {
        is_built = true;
        auto v = cht.enumerate();
        int n = v.size();
        std::vector<T> _min(n), _max(n);
        std::vector<TSum> _sum(n);
        for (int i = 0; i < n; i++) {
            lx.push_back(v[i].first);
            ln.push_back(v[i].second);
            T r = (i == n - 1 ? xmax : lx[i + 1] - 1); // [l, r]
            T x = v[i].second.get(lx[i]);
            T y = v[i].second.get(r);
            if (x > y) std::swap(x, y);
            _min[i] = x;
            _max[i] = y;
        }
        for (int i = 0; i < n; i++) {
            T l = lx[i];
            T r = (i == n - 1 ? xmax + 1 : lx[i + 1]);
            _sum[i] = line_sum(ln[i].a, ln[i].b, l, r);
        }
        st_min = sparse_table<T, merge_min, min_id>(_min);
        st_max = sparse_table<T, merge_max, max_id>(_max);
        bit = fenwick_tree<TSum>(_sum);
    }
    
    T min(T x) {
        return cht.min(x);
    }
  
    T range_min(T l, T r) {
        assert(is_built);
        int l2 = std::lower_bound(lx.begin(), lx.end(), l) - lx.begin();
        int r2 = std::upper_bound(lx.begin(), lx.end(), r) - lx.begin();
        assert(l2 > 0 && r2 > 0);
        // [l2, r2 - 2]の線を完全に含む
        // l2 - 1, r2 - 1の線を部分的に含む
        T res = cht.inf;
        if (l2 < r2 - 1) res = st_min.query(l2, r2 - 1);
        if (l2 != r2) {
            // [l, lx[l2])
            if (l < lx[l2]) res = std::min(res, std::min(ln[l2 - 1].get(l), ln[l2 - 1].get(lx[l2] - 1)));
            // [lx[r2 - 1], r)
            if (lx[r2 - 1] < r) res = std::min(res, std::min(ln[r2 - 1].get(lx[r2 - 1]), ln[r2 - 1].get(r - 1)));
        } else {
            if (l < r) res = std::min(res, std::min(ln[l2 - 1].get(l), ln[l2 - 1].get(r - 1)));
        }
        return res;
    }
    
    T range_max(T l, T r) {
        assert(is_built);
        int l2 = std::lower_bound(lx.begin(), lx.end(), l) - lx.begin();
        int r2 = std::upper_bound(lx.begin(), lx.end(), r) - lx.begin();
        assert(l2 > 0 && r2 > 0);
        // [l2, r2 - 2]の線を完全に含む
        // l2 - 1, r2 - 1の線を部分的に含む
        T res = -cht.inf;
        if (l2 < r2 - 1) res = st_max.query(l2, r2 - 1);
        if (l2 != r2) {
            // [l, lx[l2])
            if (l < lx[l2]) res = std::max(res, std::max(ln[l2 - 1].get(l), ln[l2 - 1].get(lx[l2] - 1)));
            // [lx[r2 - 1], r)
            if (lx[r2 - 1] < r) res = std::max(res, std::max(ln[r2 - 1].get(lx[r2 - 1]), ln[r2 - 1].get(r - 1)));
        } else {
            if (l < r) res = std::max(res, std::max(ln[l2 - 1].get(l), ln[l2 - 1].get(r - 1)));
        }
        return res;
    }
    // https://atcoder.jp/contests/abc303/submissions/43402759
    TSum range_sum(T l, T r) {
        assert(is_built);
        int l2 = std::lower_bound(lx.begin(), lx.end(), l) - lx.begin();
        int r2 = std::upper_bound(lx.begin(), lx.end(), r) - lx.begin();
        assert(l2 > 0 && r2 > 0);
        // [l2, r2 - 2]の線を完全に含む
        // l2 - 1, r2 - 1の線を部分的に含む
        TSum res = 0;
        if (l2 < r2 - 1) res = bit.query(l2, r2 - 1);
            if (l2 != r2) {
            // [l, lx[l2])
            if (l < lx[l2]) res += line_sum(ln[l2 - 1].a, ln[l2 - 1].b, l, lx[l2]);
            // [lx[r2 - 1], r)
            if (lx[r2 - 1] < r) res += line_sum(ln[r2 - 1].a, ln[r2 - 1].b, lx[r2 - 1], r);
        } else {
            if (l < r) res += line_sum(ln[l2 - 1].a, ln[l2 - 1].b, l, r);
        }
        return res;
    }
};
#endif
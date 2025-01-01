#ifndef _PARTIAL_RETRO_PQ_H_
#define _PARTIAL_RETRO_PQ_H_
#include <vector>
#include <numeric>
#include <algorithm>
#include <functional>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

template<typename T, typename cmp, T (*op)(T, T), T (*e)()>
struct partial_retroactive_priority_queue {
  private:
    static constexpr T null_val = std::numeric_limits<T>::max();
    static constexpr T _min(T a, T b) { return (b == null_val || (a != null_val && cmp()(a, b))) ? a : b; }
    static constexpr T _max(T a, T b) { return (a == null_val || (b != null_val && cmp()(a, b))) ? b : a; }
    enum optype { ERASED, CURRENT, POP, NONE };
    struct node {
        int Ce, Cc, Cp;
        int Dmin, mode;
        T Me, Mc;
        T x, sum;
        node() : Ce(0), Cc(0), Cp(0), Dmin(0), mode(NONE), Me(null_val), Mc(null_val), sum(e()) {}
    };
    void to_current(int _v) {
        node &v = d[_v];
        v.mode = CURRENT;
        v.Ce = v.Cp = v.Dmin = 0;
        v.Cc = 1;
        v.Me = null_val;
        v.Mc = v.x;
        v.sum = v.x;
        while (_v > 1) {
            _v /= 2;
            update(_v);
        }
    }
    void to_erased(int _v) {
        node &v = d[_v];
        v.mode = ERASED;
        v.Ce = v.Dmin = 1;
        v.Cc = v.Cp = 0;
        v.Me = v.x;
        v.Mc = null_val;
        v.sum = e();
        while (_v > 1) {
            _v /= 2;
            update(_v);
        }
    }
    void to_pop(int _v) {
        node &v = d[_v];
        v.mode = POP;
        v.Ce = v.Cc = 0;
        v.Cp = 1;
        v.Dmin = -1;
        v.Me = v.Mc = null_val;
        v.sum = e();
        while (_v > 1) {
            _v /= 2;
            update(_v);
        }
    }
    void to_none(int _v) {
        node &v = d[_v];
        v.mode = NONE;
        v.Ce = v.Cc = v.Cp = v.Dmin = 0;
        v.Me = v.Mc = null_val;
        v.sum = e();
        while (_v > 1) {
            _v /= 2;
            update(_v);
        }
    }

    int log, sz;
    std::vector<node> d;
    
    void update(int _v) {
        node &v = d[_v];
        node &l = d[_v * 2];
        node &r = d[_v * 2 + 1];
        v.Ce = l.Ce + r.Ce;
        v.Cc = l.Cc + r.Cc;
        v.Cp = l.Cp + r.Cp;
        v.Me = _max(l.Me, r.Me);
        v.Mc = _min(l.Mc, r.Mc);
        v.Dmin = std::min(l.Dmin, l.Ce - l.Cp + r.Dmin);
        v.sum = op(l.sum, r.sum);
    }

    // sum[0, p]が最小となるようなpのうち(最左/最右)
    template<bool lmost>
    int pos_dmin() const {
        int v = 1;
        while (v < sz) {
            int cnt = d[v * 2].Ce - d[v * 2].Cp;
            int RDmin = cnt + d[v * 2 + 1].Dmin;
            if (d[v * 2].Dmin < RDmin + lmost) {
                v = v * 2;
            } else {
                v = v * 2 + 1;
            }
        }
        return v - sz;
    }

    // [0, r)のMc
    int pos_Mc(int r) const {
        int l = sz;
        r += sz;
        T Mc = null_val;
        while (l < r) {
            if (l & 1) Mc = _min(Mc, d[l++].Mc);
            if (r & 1) Mc = _min(Mc, d[--r].Mc);
            l >>= 1;
            r >>= 1;
        }
        int v = 1;
        while (v < sz) {
            bool is_l = (_min(d[v * 2].Mc, Mc) == d[v * 2].Mc);
            v = v * 2 + (!is_l);
        }
        return v - sz;
    }

    // [l, L)のMe
    int pos_Me(int l) const {
        l += sz;
        int r = 2 * sz;
        T Me = null_val;
        while (l < r) {
            if (l & 1) Me = _max(Me, d[l++].Me);
            if (r & 1) Me = _max(Me, d[--r].Me);
            l >>= 1;
            r >>= 1;
        }
        int v = 1;
        while (v < sz) {
            bool is_r = (_max(d[v * 2 + 1].Me, Me) == d[v * 2 + 1].Me);
            v = v * 2 + is_r;
        }
        return v - sz;
    }

  public:
    partial_retroactive_priority_queue(int L) : log(bit_ceil_log(std::max(2, L))), sz(1 << log), d(2 * sz) {}

    void set_op_push(int k, T x) {
        assert(0 <= k && k < sz);
        set_op_null(k);
        d[sz + k].x = x;
        to_erased(sz + k);
        int l = (d[1].Dmin == 0 ? pos_dmin<false>() : 0);
        int p = pos_Me(l);
        to_current(sz + p);
    }

    // 空の状態でpopするような操作列に一度なると壊れる
    // あらかじめinfをQ個pushしておけばいい
    void set_op_pop(int k) {
        assert(0 <= k && k < sz);
        if (d[sz + k].mode == POP) return;
        set_op_null(k);
        to_pop(sz + k);
        int r = pos_dmin<true>();
        int p = pos_Mc(r);
        to_erased(sz + p);
    }

    void set_op_null(int k) {
        assert(0 <= k && k < sz);
        int mode = d[sz + k].mode;
        if (mode == NONE) return;
        to_none(k + sz);
        if (mode == ERASED) {
            int p = pos_dmin<true>();
            p = pos_Mc(p);
            to_erased(sz + p);
        }
        if (mode == POP) {
            int p = (d[1].Dmin == 0 ? pos_dmin<false>() : 0);
            p = pos_Me(p);
            to_current(sz + p);
        }
    }

    int size() const {
        return d[1].Cc;
    }

    bool empty() const {
        return size() == 0;
    }

    T top() const {
        assert(!empty());
        return d[1].Mc;
    }

    // 残っている要素の追加した時刻の順のprod
    T all_prod() const {
        return d[1].sum;
    }
};
#endif
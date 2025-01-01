#ifndef _ROLLBACK_NOT_LAZY_SEGMENT_TREE_H_
#define _ROLLBACK_NOT_LAZY_SEGMENT_TREE_H_
#include <vector>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

// 作用する順番を入れ替えても結果が変わらない時に使える
template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
struct segtree_lazy_fast_rollback {
  public:
    segtree_lazy_fast_rollback() : segtree_lazy_fast_rollback(0) {}
    explicit segtree_lazy_fast_rollback(int n) : segtree_lazy_fast_rollback(std::vector<S>(n, e())) {}
    explicit segtree_lazy_fast_rollback(const std::vector<S>& v) : _n(int(v.size())) {
        size = bit_ceil((unsigned int)(_n));
        log = bit_ceil_log((unsigned int)size);
        d = std::vector<S>(4 * size, e());
        lz = std::vector<std::vector<F>>(2 * size, {id()});
        for (int i = 0; i < _n; i++) {
          d[size + i] = d[2 * (size + i)] = v[i];
          d[2 * (size + i) + 1] = e();
        }
        for (int i = size - 1; i >= 1; i--) {
            update(i);
        }
    }

    S get(int p) {
        assert(0 <= p && p < _n);
        p += size;
        F lazy = id();
        for (int i = log; i >= 1; i--) lazy = composition(lazy, lz[p >> i].back());
        return mapping(lazy, d[p], 1);
    }

    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return e();

        l += size;
        r += size;

        int llen = 0, rlen = 0;
        S sml = e(), smr = e();
        int i = 1, l2 = l, r2 = r;
        for (; l < r; i++, l >>= 1, r >>= 1) {
            if (l & 1) sml = op(sml, d[l++]), llen += 1 << (i - 1);
            if (r & 1) smr = op(d[--r], smr), rlen += 1 << (i - 1);
            if (((l2 >> i) << i) != l2) sml = mapping(lz[l2 >> i].back(), sml, llen);
            if (((r2 >> i) << i) != r2) smr = mapping(lz[(r2 - 1) >> i].back(), smr, rlen);
        }
        for (; i <= log; i++) {
            if (((l2 >> i) << i) != l2) sml = mapping(lz[l2 >> i].back(), sml, llen);
            if (((r2 >> i) << i) != r2) smr = mapping(lz[(r2 - 1) >> i].back(), smr, rlen);
        }
        return op(sml, smr);
    }

    S all_prod() { return d[1]; }

    void apply(int l, int r, F f) {
        assert(0 <= l && l <= r && r <= _n);
        hist.push_back({l, r});
        if (l == r) return;

        l += size;
        r += size;

        int i = 1, l2 = l, r2 = r;
        for (; l < r; i++, l >>= 1, r >>= 1) {
            if (l & 1) all_apply(l++, f);
            if (r & 1) all_apply(--r, f);
            update_with_lazy(l2 >> i);
            update_with_lazy((r2 - 1) >> i);
        }
        for (; i <= log; i++) {
            update_with_lazy(l2 >> i);
            update_with_lazy((r2 - 1) >> i);
        }
    }

    void rollback() {
        assert(!hist.empty());
        auto [l, r] = hist.back();
        hist.pop_back();
        if (l == r) return;
        
        l += size;
        r += size;

        int i = 1, l2 = l, r2 = r;
        for (; l < r; i++, l >>= 1, r >>= 1) {
            if (l & 1) {
              lz[l].pop_back();
              update_with_lazy(l);
              l++;
            }
            if (r & 1) {
              r--;
              lz[r].pop_back();
              update_with_lazy(r);
            }
            update_with_lazy(l2 >> i);
            update_with_lazy((r2 - 1) >> i);
        }

        for (; i <= log; i++) {
            update_with_lazy(l2 >> i);
            update_with_lazy((r2 - 1) >> i);
        }
    }
  private:
    int _n, size, log;
    std::vector<S> d;
    std::vector<std::vector<F>> lz;
    std::vector<std::pair<int, int>> hist;

    void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
    void update_with_lazy(int k) { d[k] = mapping(lz[k].back(), op(d[2 * k], d[2 * k + 1]), 1 << (log - 31 + __builtin_clz(k))); }
    void all_apply(int k, F f) {
        d[k] = mapping(f, d[k], 1 << (log - 31 + __builtin_clz(k)));
        lz[k].push_back(composition(f, lz[k].back()));
    }
};
template<typename monoid>
using segtree_lazy_fast_rollback_monoid = segtree_lazy_fast_rollback<typename monoid::S, monoid::op, monoid::e, typename monoid::F, monoid::mapping, monoid::composition, monoid::id>;
#endif
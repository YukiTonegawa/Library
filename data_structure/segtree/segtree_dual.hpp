#ifndef _DUAL_SEGTREE_H_
#define _DUAL_SEGTREE_H_
#include <vector>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

template <class S, S (*e)(), class F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
struct segtree_dual {
  public:
    segtree_dual() : segtree_dual(0) {}
    explicit segtree_dual(int n) : segtree_dual(std::vector<S>(n, e())) {}
    explicit segtree_dual(const std::vector<S>& v) : _n(int(v.size())) {
        size = (int)bit_ceil((unsigned int)(_n));
        log = countr_zero((unsigned int)size);
        d = std::vector<S>(size, e());
        lz = std::vector<F>(size, id());
        for (int i = 0; i < _n; i++) d[i] = v[i];
    }

    void set(int p, S x) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p - size] = x;
    }

    S get_acl(int p) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        return d[p - size];
    }

    S get(int p) {
        assert(0 <= p && p < _n);
        p += size;
        S res = d[p - size];
        for (int i = 1; i <= log; i++) res = mapping(lz[p >> i], res, 1);
        return res;
    }

    void apply(int p, F f) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p - size] = mapping(f, d[p - size], 1);
    }
    void apply(int l, int r, F f) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return;

        l += size;
        r += size;

        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        {
            int l2 = l, r2 = r;
            while (l < r) {
                if (l & 1) all_apply(l++, f);
                if (r & 1) all_apply(--r, f);
                l >>= 1;
                r >>= 1;
            }
            l = l2;
            r = r2;
        }
    }

  private:
    int _n, size, log;
    std::vector<S> d;
    std::vector<F> lz;

    void all_apply(int k, F f) {
        if (k < size) {
            lz[k] = composition(f, lz[k]);
        } else {
            d[k - size] = mapping(f, d[k - size], 1);
        }
    }
    void push(int k) {
        all_apply(2 * k, lz[k]);
        all_apply(2 * k + 1, lz[k]);
        lz[k] = id();
    }
};

template<typename monoid>
using segtree_dual_monoid = segtree_dual<typename monoid::S, monoid::e, typename monoid::F, monoid::mapping, monoid::composition, monoid::id>;
#endif
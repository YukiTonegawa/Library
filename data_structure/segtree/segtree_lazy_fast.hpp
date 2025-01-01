#ifndef _NOT_LAZY_SEGMENT_TREE_H_
#define _NOT_LAZY_SEGMENT_TREE_H_
#include <vector>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

// 作用する順番を入れ替えても結果が変わらない時に使える
template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
struct segtree_lazy_fast {
  public:
    segtree_lazy_fast() : segtree_lazy_fast(0) {}
    explicit segtree_lazy_fast(int n) : segtree_lazy_fast(std::vector<S>(n, e())) {}
    explicit segtree_lazy_fast(const std::vector<S>& v) : _n(int(v.size())) {
        size = bit_ceil((unsigned int)(_n));
        log = bit_ceil_log((unsigned int)size);
        d = std::vector<S>(2 * size, e());
        lz = std::vector<F>(size, id());
        for (int i = 0; i < _n; i++) d[size + i] = v[i];
        for (int i = size - 1; i >= 1; i--) {
            update(i);
        }
    }

    void set(int p, S x) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p] = x;
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    S get(int p) {
        assert(0 <= p && p < _n);
        p += size;
        F lazy = id();
        for (int i = log; i >= 1; i--) lazy = composition(lazy, lz[p >> i]);
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
            if (((l2 >> i) << i) != l2) sml = mapping(lz[l2 >> i], sml, llen);
            if (((r2 >> i) << i) != r2) smr = mapping(lz[(r2 - 1) >> i], smr, rlen);
        }
        for (; i <= log; i++) {
            if (((l2 >> i) << i) != l2) sml = mapping(lz[l2 >> i], sml, llen);
            if (((r2 >> i) << i) != r2) smr = mapping(lz[(r2 - 1) >> i], smr, rlen);
        }
        return op(sml, smr);
    }

    S all_prod() { return d[1]; }

    void apply(int p, F f) {
        assert(0 <= p && p < _n);
        p += size;
        d[p] = mapping(f, d[p], 1);
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    void apply(int l, int r, F f) {
        assert(0 <= l && l <= r && r <= _n);
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
    
    template <bool (*g)(S)> int max_right(int l) {
        return max_right(l, [](S x) { return g(x); });
    }
    template <class G> int max_right(int l, G g) {
        assert(0 <= l && l <= _n);
        assert(g(e()));
        if (l == _n) return _n;
        l += size;
        for (int i = log; i >= 1; i--) push(l >> i);
        S sm = e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!g(op(sm, d[l]))) {
                while (l < size) {
                    push(l);
                    l = (2 * l);
                    if (g(op(sm, d[l]))) {
                        sm = op(sm, d[l]);
                        l++;
                    }
                }
                return l - size;
            }
            sm = op(sm, d[l]);
            l++;
        } while ((l & -l) != l);
        return _n;
    }

    template <bool (*g)(S)> int min_left(int r) {
        return min_left(r, [](S x) { return g(x); });
    }
    template <class G> int min_left(int r, G g) {
        assert(0 <= r && r <= _n);
        assert(g(e()));
        if (r == 0) return 0;
        r += size;
        for (int i = log; i >= 1; i--) push((r - 1) >> i);
        S sm = e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!g(op(d[r], sm))) {
                while (r < size) {
                    push(r);
                    r = (2 * r + 1);
                    if (g(op(d[r], sm))) {
                        sm = op(d[r], sm);
                        r--;
                    }
                }
                return r + 1 - size;
            }
            sm = op(d[r], sm);
        } while ((r & -r) != r);
        return 0;
    }

  private:
    int _n, size, log;
    std::vector<S> d;
    std::vector<F> lz;

    void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
    void update_with_lazy(int k) { d[k] = mapping(lz[k], op(d[2 * k], d[2 * k + 1]), 1 << (log - 31 + __builtin_clz(k))); }
    void all_apply(int k, F f) {
        d[k] = mapping(f, d[k], 1 << (log - 31 + __builtin_clz(k)));
        if (k < size) lz[k] = composition(f, lz[k]);
    }
    // setする時にしかpushしない
    void push(int k) {
        all_apply(2 * k, lz[k]);
        all_apply(2 * k + 1, lz[k]);
        lz[k] = id();
    }
};

template<typename monoid>
using segtree_lazy_fast_monoid = segtree_lazy_fast<typename monoid::S, monoid::op, monoid::e, typename monoid::F, monoid::mapping, monoid::composition, monoid::id>;
#endif

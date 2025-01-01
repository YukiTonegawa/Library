#ifndef _LAZY_SEGTREE_SET_H_
#define _LAZY_SEGTREE_SET_H_
#include <cassert>
#include <vector>
#include "../../math/base/bit_ceil.hpp"

// 区間set可能
template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
struct segtree_lazy_set {
    static constexpr int Dmax = 20; // N < 2^Dmax
    using ptr = std::array<S, Dmax>*;
  public:
    segtree_lazy_set() : segtree_lazy_set(0) {}
    explicit segtree_lazy_set(int n) : segtree_lazy_set(std::vector<S>(n, e())) {}
    explicit segtree_lazy_set(const std::vector<S>& v) : _n(int(v.size())) {
        assert(_n < (1 << Dmax));
        size = bit_ceil((unsigned int)(_n));
        log = bit_ceil_log((unsigned int)size);
        d = std::vector<S>(2 * size, e());
        lz = std::vector<F>(size, id());
        rs = std::vector<ptr>(size, nullptr);
        for (int i = 0; i < _n; i++) d[size + i] = v[i];
        for (int i = size - 1; i >= 1; i--) {
            update(i);
        }
    }

    S get(int p) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        return d[p];
    }

    void set(int p, S x) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p] = x;
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    void set(int l, int r, S x) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return;
        ptr p(new std::array<S, Dmax>());
        {  
            (*p)[0] = x;
            for (int i = 1; i <= 31 - __builtin_clz(r - l); i++){
                (*p)[i] = op((*p)[i - 1], (*p)[i - 1]);
            }
        }

        l += size;
        r += size;

        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        {
            int l2 = l, r2 = r;
            while (l < r) {
                if (l & 1) all_set(l++, p);
                if (r & 1) all_set(--r, p);
                l >>= 1;
                r >>= 1;
            }
            l = l2;
            r = r2;
        }

        for (int i = 1; i <= log; i++) {
            if (((l >> i) << i) != l) update(l >> i);
            if (((r >> i) << i) != r) update((r - 1) >> i);
        }
    }

    void reset(int l, int r) {
        set(l, r, e());
    }

    void reset() {
        set(0, _n, e());
    }
    
    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return e();

        l += size;
        r += size;

        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        S sml = e(), smr = e();
        while (l < r) {
            if (l & 1) sml = op(sml, d[l++]);
            if (r & 1) smr = op(d[--r], smr);
            l >>= 1;
            r >>= 1;
        }

        return op(sml, smr);
    }

    S all_prod() { return d[1]; }

    void apply(int p, F f) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p] = mapping(f, d[p], 1);
        for (int i = 1; i <= log; i++) update(p >> i);
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

        for (int i = 1; i <= log; i++) {
            if (((l >> i) << i) != l) update(l >> i);
            if (((r >> i) << i) != r) update((r - 1) >> i);
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
    std::vector<ptr> rs;

    void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
    void all_apply(int k, F f) {
        d[k] = mapping(f, d[k], 1 << (log - 31 + __builtin_clz(k)));
        if (k < size) lz[k] = composition(f, lz[k]);
    }
    void all_set(int k, ptr p) {
        if (k < size) {
            lz[k] = id();
            rs[k] = p;
        }
        d[k] = (*p)[log - 31 + __builtin_clz(k)];
    }
    void push(int k) {
        if (rs[k]) {
            all_set(2 * k, rs[k]);
            all_set(2 * k + 1, rs[k]);
            rs[k] = nullptr;
        }
        all_apply(2 * k, lz[k]);
        all_apply(2 * k + 1, lz[k]);
        lz[k] = id();
    }
};
template<typename monoid>
using segtree_lazy_set_monoid = segtree_lazy_set<typename monoid::S, monoid::op, monoid::e, typename monoid::F, monoid::mapping, monoid::composition, monoid::id>;
#endif

#ifndef _BEATS_ABSTRUCT_H_
#define _BEATS_ABSTRUCT_H_
#include <vector>
#include <algorithm>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

template<typename S, void (*op)(S&, const S&, const S&), S (*e)(), typename F, void (*mapping)(const F&, S&, int), void (*composition)(const F&, F&)>
struct beats_abstract {
  public:
    beats_abstract() : beats_abstract(0) {}
    explicit beats_abstract(int n) : beats_abstract(std::vector<S>(n, e())) {}
    explicit beats_abstract(const std::vector<S>& v) : _n(int(v.size())) {
        size = (int)bit_ceil((unsigned int)(_n));
        log = bit_ceil_log((unsigned int)size);
        d = std::vector<S>(2 * size, e());
        lz = std::vector<F>(2 * size, F());
        que = std::vector<int>(2 * size);
        isid = std::vector<bool>(2 * size, true);
        for (int i = 0; i < _n; i++) d[size + i] = v[i];
        for (int i = size - 1; i >= 1; i--) update(i);
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
        for (int i = log; i >= 1; i--) push(p >> i);
        return d[p];
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

        S sml[2] = {e(), e()};
        S smr[2] = {e(), e()};
        bool fl = 0, fr = 0;
        {
            int l2 = l, r2 = r;
            while (l < r) {
                if (l & 1) {
                    op(sml[!fl], sml[fl], d[l++]);
                    fl = !fl;
                }
                if (r & 1) {
                    op(smr[!fr], d[--r], smr[fr]);
                    fr = !fr;
                }
                l >>= 1;
                r >>= 1;
            }
            l = l2;
            r = r2;
        }
        op(sml[!fl], sml[fl], smr[fr]);
        return sml[!fl];
    }

    template<bool (*break_check)(const S&, const F&), bool (*tag_check)(const S&, const F&), F (*transform)(const S&, const F&)>
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
                if (l & 1) all_apply_extra<break_check, tag_check, transform>(l++, f);
                if (r & 1) all_apply_extra<break_check, tag_check, transform>(--r, f);
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

  private:
    int _n, size, log;
    std::vector<S> d;
    std::vector<F> lz;
    std::vector<int> que;
    std::vector<bool> isid;
    
    void update(int k) { op(d[k], d[2 * k], d[2 * k + 1]); }

    void all_apply(int k, const F &f) {
        mapping(f, d[k], 1 << (log - 31 + __builtin_clz(k)));
        if (isid[k]) {
            isid[k] = false;
            lz[k] = f;
        } else {
            composition(f, lz[k]);
        }
    }

    template<bool (*break_check)(const S&, const F&), bool (*tag_check)(const S&, const F&), F (*transform)(const S&, const F&)>
    void all_apply_extra(int k, const F &f) {
        int pos = 0;
        que[pos] = k;
        while(pos >= 0) {
            int idx = que[pos--];
            if (idx < 0) {
                update(-idx);
            } else if (break_check(d[idx], f)) {
            } else if(tag_check(d[idx], f)) {
                all_apply(idx, transform(d[idx], f));
            } else {
                push(idx);
                que[++pos] = -idx;
                que[++pos] = idx * 2;
                que[++pos] = idx * 2 + 1;
            }
        }
    }

    void push(int k) {
        if (!isid[k]) {
            all_apply(2 * k, lz[k]);
            all_apply(2 * k + 1, lz[k]);
            isid[k] = true;
        }
    }
};
#endif
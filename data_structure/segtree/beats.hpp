#ifndef _BEATS_FAST_H_
#define _BEATS_FAST_H_
#include "../../math/base/bit_ceil.hpp"
#include <cassert>
#include <algorithm>

template <typename T, typename Tsum>
struct beats {
  public:
    static constexpr T inf = std::numeric_limits<T>::max();
    static constexpr T minf = std::numeric_limits<T>::min();
  private:
    struct S{
        T min, second_min, max, second_max;
        Tsum sum;
        int min_cnt, max_cnt;
        S(): min(inf), second_min(inf), max(minf), second_max(minf), sum(0), min_cnt(0), max_cnt(0){}
        S(T x): min(x), second_min(inf), max(x), second_max(minf), sum(x), min_cnt(1), max_cnt(1){}
    };
    
    struct F{
        T add, lower, upper;
        F(): add(0), lower(minf), upper(inf){}
        F(T a, T b, T c): add(a), lower(b), upper(c){}
        void reset(){
            add = 0;
            lower = minf;
            upper = inf;
        }
    };

    int _n, size, log;
    std::vector<S> d;
    std::vector<F> lz;
    std::vector<int> que;

    // v, lにfを作用
    void propagate(S &v, int len, F &l, const F &f) {
        if(f.add) {
            l.add += f.add;
            if (l.lower != minf) l.lower += f.add;
            if (l.upper != inf) l.upper += f.add;
            v.min += f.add;
            if (v.second_min != inf) v.second_min += f.add;
            v.max += f.add;
            if (v.second_max != minf) v.second_max += f.add;
            v.sum += (Tsum)f.add * len;
        }
        if(v.min < f.lower) {
            l.lower = f.lower;
            if (v.max < f.lower) l.upper = f.lower;
            v.sum += (Tsum)(f.lower - v.min) * v.min_cnt;
            if (v.second_max == v.min) v.second_max = f.lower;
            else if (v.max == v.min) v.max = f.lower, v.second_max = minf;
            v.min = f.lower;
        }
        if(f.upper < v.max) {
            l.upper = f.upper;
            if (f.upper < v.min) l.lower = f.upper;
            v.sum -= (Tsum)(v.max - f.upper) * v.max_cnt;
            if (v.second_min == v.max) v.second_min = f.upper;
            else if (v.min == v.max) v.min = f.upper, v.second_min = inf;
            v.max = f.upper;
        }
    }

    void update_inplace(S &v, const S &l, const S &r) {
        v.sum = l.sum + r.sum;
        if(l.max == r.max) {
            v.max = l.max;
            v.max_cnt = l.max_cnt + r.max_cnt;
            v.second_max = std::max(l.second_max, r.second_max);
        } else if(l.max > r.max) {
            v.max = l.max;
            v.max_cnt = l.max_cnt;
            v.second_max = std::max(l.second_max, r.max);
        } else {
            v.max = r.max;
            v.max_cnt = r.max_cnt;
            v.second_max = std::max(l.max, r.second_max);
        }
        if(l.min == r.min) {
            v.min = l.min;
            v.min_cnt = l.min_cnt + r.min_cnt;
            v.second_min = std::min(l.second_min, r.second_min);
        } else if(l.min < r.min) {
            v.min = l.min;
            v.min_cnt = l.min_cnt;
            v.second_min = std::min(l.second_min, r.min);
        } else {
            v.min = r.min;
            v.min_cnt = r.min_cnt;
            v.second_min = std::min(l.min, r.second_min);
        }
    }

    // prod sumで使う
    void propagate_prod_sum(S &v, int len, const F &f) {
        if(f.add) {
            v.min += f.add;
            v.max += f.add;
            v.sum += (Tsum)f.add * len;
        }
        if(v.min < f.lower) {
            v.sum += (Tsum)(f.lower - v.min) * v.min_cnt;
            if (v.max == v.min) v.max = f.lower;
            v.min = f.lower;
        }
        if(f.upper < v.max) {
            v.sum -= (Tsum)(v.max - f.upper) * v.max_cnt;
            if (v.min == v.max) v.min = f.upper;
            v.max = f.upper;
        }
    }

    // prod sumで使う
    void update_prod_sum(S &v, const S &u) {
        v.sum += u.sum;
        if (v.max <= u.max) {
            v.max_cnt = u.max_cnt + (v.max == u.max ? v.max_cnt : 0);
            v.max = u.max;
        }
        if (v.min >= u.min) {
            v.min_cnt = u.min_cnt + (v.min == u.min ? v.min_cnt : 0);
            v.min = u.min;
        }
    }

    void update(int k) { update_inplace(d[k], d[2 * k], d[2 * k + 1]); }
    
    void all_apply(int k, const F &f) {
        propagate(d[k], 1 << (log - 31 + __builtin_clz(k)), lz[k], f);
    }

    void all_apply_min(int k, T f) {
        int pos = 0;
        que[pos] = k;
        while(pos >= 0) {
            int idx = que[pos--];
            if (idx < 0) {
                update(-idx);
            } else if (d[idx].max <= f) {
            } else if(d[idx].second_max < f) {
                propagate(d[idx], 0, lz[idx], F(0, minf, f));
            } else {
                push(idx);
                que[++pos] = -idx;
                que[++pos] = idx * 2;
                que[++pos] = idx * 2 + 1;
            }
        }
    }

    void all_apply_max(int k, T f) {
        int pos = 0;
        que[pos] = k;
        while(pos >= 0) {
            int idx = que[pos--];
            if (idx < 0) {
                update(-idx);
            } else if (f <= d[idx].min) {
            } else if(f < d[idx].second_min) {
                propagate(d[idx], 0, lz[idx], F(0, f, inf));
            } else {
                push(idx);
                que[++pos] = -idx;
                que[++pos] = idx * 2;
                que[++pos] = idx * 2 + 1;
            }
        }
    }

    void all_apply_add(int k, T f) {
        propagate(d[k], 1 << (log - 31 + __builtin_clz(k)), lz[k], F(f, minf, inf));
    }

    void push(int k) {
        if (lz[k].add == 0 && lz[k].lower == minf && lz[k].upper == inf) return;
        all_apply(2 * k, lz[k]);
        all_apply(2 * k + 1, lz[k]);
        lz[k].reset();
    }

  public:
    
    beats() : beats(0) {}
    // n要素の0で初期化
    beats(int n) : beats(std::vector<T>(n, T(0))) {}
    beats(const std::vector<T>& v) : _n(int(v.size())) {
        size = (int)bit_ceil((unsigned int)(_n));
        log = bit_ceil_log((unsigned int)size);
        d = std::vector<S>(2 * size, S());
        lz = std::vector<F>(2 * size, F());
        que = std::vector<int>(2 * size);
        for (int i = 0; i < _n; i++) d[size + i] = S(v[i]);
        for (int i = size - 1; i >= 1; i--) update(i);
    }
    
    void set(int p, T x) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p] = x;
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    T get(int p) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        return d[p].min;
    }

    T prod_min(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return inf;
        l += size;
        r += size;
        T sml = inf, smr = inf;
        int i = 1, l2 = l, r2 = r;
        for (; l < r; i++, l >>= 1, r >>= 1) {
            if (l & 1) sml = std::min(sml, d[l++].min);
            if (r & 1) smr = std::min(d[--r].min, smr);
            if (sml != inf) sml = std::min(lz[l2 >> i].upper, sml + lz[l2 >> i].add);
            if (smr != inf) smr = std::min(lz[(r2 - 1) >> i].upper, smr + lz[(r2 - 1) >> i].add);
        }
        for (; i <= log; i++) {
            if (sml != inf) sml = std::min(lz[l2 >> i].upper, sml + lz[l2 >> i].add);
            if (smr != inf) smr = std::min(lz[(r2 - 1) >> i].upper, smr + lz[(r2 - 1) >> i].add);
        }
        return std::min(sml, smr);
    }

    T prod_max(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return minf;
        l += size;
        r += size;
        T sml = minf, smr = minf;
        int i = 1, l2 = l, r2 = r;
        for (; l < r; i++, l >>= 1, r >>= 1) {
            if (l & 1) sml = std::max(sml, d[l++].max);
            if (r & 1) smr = std::max(d[--r].max, smr);
            if (sml != minf) sml = std::max(lz[l2 >> i].lower, sml + lz[l2 >> i].add);
            if (smr != minf) smr = std::max(lz[(r2 - 1) >> i].lower, smr + lz[(r2 - 1) >> i].add);
        }
        for (; i <= log; i++) {
            if (sml != minf) sml = std::max(lz[l2 >> i].lower, sml + lz[l2 >> i].add);
            if (smr != minf) smr = std::max(lz[(r2 - 1) >> i].lower, smr + lz[(r2 - 1) >> i].add);
        }
        return std::max(sml, smr);
    }

    Tsum prod_sum(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return 0;
        l += size;
        r += size;
        S sml, smr;
        int llen = 0, rlen = 0;
        int i = 1, l2 = l, r2 = r;
        for (; l < r; i++, l >>= 1, r >>= 1) {
            if (l & 1) update_prod_sum(sml, d[l++]), llen += 1 << (i - 1);
            if (r & 1) update_prod_sum(smr, d[--r]), rlen += 1 << (i - 1);
            if (llen) propagate_prod_sum(sml, llen, lz[l2 >> i]);
            if (rlen) propagate_prod_sum(smr, rlen, lz[(r2 - 1) >> i]);
        }
        for (; i <= log; i++) {
            if (llen) propagate_prod_sum(sml, llen, lz[l2 >> i]);
            if (rlen) propagate_prod_sum(smr, rlen, lz[(r2 - 1) >> i]);
        }
        return sml.sum + smr.sum;
    }

    // all info
    S _prod(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return S();
        l += size;
        r += size;
        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }
        S res, tmp;
        for (; l < r; l >>= 1, r >>= 1) {
            if (l & 1) {
                tmp = res;
                update_inplace(res, tmp, d[l++]);
            }
            if (r & 1) {
                tmp = res;
                update_inplace(res, tmp, d[--r]);
            }
        }
        return res;
    }

    void apply(int p, F f) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p] = mapping(f, d[p]);
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    void apply_min(int l, int r, T f) {
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
                if (l & 1) all_apply_min(l++, f);
                if (r & 1) all_apply_min(--r, f);
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

    void apply_max(int l, int r, T f) {
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
                if (l & 1) all_apply_max(l++, f);
                if (r & 1) all_apply_max(--r, f);
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

    void apply_add(int l, int r, T f) {
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
                if (l & 1) all_apply_add(l++, f);
                if (r & 1) all_apply_add(--r, f);
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
};
#endif
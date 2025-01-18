#ifndef _DYNAMIC_PREFIX_MAX_H_
#define _DYNAMIC_PREFIX_MAX_H_
#include <algorithm>
#include <vector>
#include <numeric>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

template<typename T, bool eq> 
struct dynamic_prefix_max {
  private:
    static constexpr T minf = std::numeric_limits<T>::min();
    struct S {
        int len, len_right;
        T max_sub;
        S(T x = minf) : len(1), len_right(0), max_sub(x) {}
    };
    int _n, log, size;
    std::vector<S> d;

    // 左最大値lmaxを持ってkの部分木に潜る
    std::pair<int, T> dive(int k, T lmax) const {
        T rmax = d[k].max_sub;
        if (lmax >= d[k].max_sub + eq) return {0, lmax};
        int len = 1;
        while (k < size) {
            if (lmax < d[k * 2].max_sub + eq) {
                len += d[k].len_right;
                k = k * 2;
            } else {
                k = k * 2 + 1;
            }
        }
        return {len, rmax};
    }

    void update(int k) {
        auto [rlen, rmax] = dive(k * 2 + 1, d[k * 2].max_sub);
        d[k].len = d[k * 2].len + rlen;
        d[k].len_right = rlen;
        d[k].max_sub = rmax;
    }

  public:
    dynamic_prefix_max() : _n(0) {}
    dynamic_prefix_max(const std::vector<T> &V) : _n(V.size()) {
        log = bit_ceil_log(_n);
        size = 1 << log;
        d = std::vector<S>(2 * size);
        for (int i = 0; i < (int)V.size(); i++) {
            d[i + size] = S(V[i]);
        }
        for (int i = size - 1; i > 0; i--) {
            update(i);
        }
    }

    void set(int p, T x) {
        p += size;
        d[p] = S(x);
        for (int i = 1; i <= log; i++) {
            update(p >> i);
        }
    }

    void get(int p) const {
        return d[p].max_sub;
    }

    // Max[l, r)
    T prod_max(int l, int r) const {
        assert(0 <= l && l <= r && r <= _n);
        T res = minf;
        l += size;
        r += size;
        while (l < r) {
            if (l & 1) res = std::max(res, d[l++].max_sub);
            if (r & 1) res = std::max(res, d[--r].max_sub);
            l >>= 1;
            r >>= 1;
        }
        return res;
    }

    // lmaxから始めたLen[l, r)
    int prod_len(int l, int r, T lmax = minf) const {
        assert(0 <= l && l <= r && r <= _n);
        int len = 0;
        auto dfs = [&](auto &&dfs, int k, int L, int R) -> void {
            if (r <= L || R <= l) return;
            if (l <= L && R <= r) {
                auto [rlen, rmax] = dive(k, lmax);
                len += rlen;
                lmax = rmax;
                return;
            }
            int M = (L + R) / 2;
            dfs(dfs, k * 2, L, M);
            dfs(dfs, k * 2 + 1, M, R);
        };
        dfs(dfs, 1, 0, size);
        return len;
    }

    // f(Len[l, r), Max[l, r))がtrueになる最大のr
    template<typename F>
    int max_right(int l, F f) const {
        assert(0 <= l && l <= _n);
        assert(f(0, minf));
        if (l == _n) return _n;
        l += size;
        int len = 0;
        T lmax = minf;
        do {
            while (l % 2 == 0) l >>= 1;
            auto [rlen, rmax] = dive(l, lmax);
            if (!f(len + rlen, rmax)) {
                while (l < size) {
                    l = (2 * l);
                    std::tie(rlen, rmax) = dive(l, lmax);
                    if (f(len + rlen, rmax)) {
                        len += rlen;
                        lmax = rmax;
                        l++;
                    }
                }
                return l - size;
            }
            len += rlen;
            lmax = rmax;
            l++;
        } while ((l & -l) != l);
        return _n;    
    }
};
#endif
#ifndef _LINEAR_ADD_RANGE_SUM_H_
#define _LINEAR_ADD_RANGE_SUM_H_
#include <vector>
#include <limits>
#include <algorithm>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

template <typename T>
struct linear_add_range_sum {
  private:
    struct node {
        T sum, lza, lzb;
        long long isum;
        int sz;
        node(int i = -1, T v = 0) : sum(v), lza(0), lzb(0), isum(i == -1 ? 0 : i), sz(i == -1 ? 0 : 1) {}
    };
  public:
    linear_add_range_sum() : linear_add_range_sum(0) {}
    explicit linear_add_range_sum(int n) : linear_add_range_sum(std::vector<T>(n, 0)) {}
    explicit linear_add_range_sum(const std::vector<T>& v) : _n(int(v.size())) {
        size = bit_ceil((unsigned int)(_n));
        log = bit_ceil_log((unsigned int)size);
        nd = std::vector<node>(2 * size);
        for (int i = 0; i < size; i++) nd[size + i] = node(i < _n ? i : -1, i < _n ? v[i] : 0);
        for (int i = size - 1; i >= 1; i--) {
            nd[i].sz = nd[2 * i].sz + nd[2 * i + 1].sz;
            nd[i].isum = nd[2 * i].isum + nd[2 * i + 1].isum;
            nd[i].sum = nd[2 * i].sum + nd[2 * i + 1].sum;
        }
    }
    
    void set(int p, T x) {
        assert(0 <= p && p < _n);
        int P = p;
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        nd[p] = node(P, x);
        for (int i = 1; i <= log; i++) pull(P >> i);
    }

    T get(int p) {
        assert(0 <= p && p < _n);
        p += size;
        T a = 0, b = 0;
        for (int i = log; i >= 1; i--) a += nd[p >> i].lza, b += nd[p >> i].lzb;
        return nd[p].sum + nd[p].isum * a + b;
    }

    void apply(int l, int r, T a, T b) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return;

        l += size;
        r += size;
        {
            int l2 = l, r2 = r;
            while (l < r) {
                if (l & 1) all_apply(l++, a, b);
                if (r & 1) all_apply(--r, a, b);
                l >>= 1;
                r >>= 1;
            }
            l = l2;
            r = r2;
        }

        for (int i = 1; i <= log; i++) {
            if (((l >> i) << i) != l) pull(l >> i);
            if (((r >> i) << i) != r) pull((r - 1) >> i);
        }
    }

    // [l, r)のsum
    T prod(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return 0;

        l += size;
        r += size;

        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        T res = 0;
        while (l < r) {
            if (l & 1) res += nd[l++].sum;
            if (r & 1) res += nd[--r].sum;
            l >>= 1;
            r >>= 1;
        }

        return res;
    }
    
  private:
    int _n, size, log;
    std::vector<node> nd;
    std::vector<bool> correct;

    void all_apply(int k, T a, T b) {
        nd[k].sum += nd[k].isum * a + nd[k].sz * b;
        if (k < size) nd[k].lza += a, nd[k].lzb += b;
    }

    void push(int k) {
        all_apply(2 * k, nd[k].lza, nd[k].lzb);
        all_apply(2 * k + 1, nd[k].lza, nd[k].lzb);
        nd[k].lza = nd[k].lzb = 0;
    }

    void pull(int k) {
        nd[k].sum = nd[k * 2].sum + nd[k * 2 + 1].sum;
    }
};
#endif
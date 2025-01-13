#ifndef _LINEAR_ADD_RANGE_MIN_H_
#define _LINEAR_ADD_RANGE_MIN_H_
#include <vector>
#include <limits>
#include <algorithm>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

// T : 2 * 値が収まる型
// TN : 2 * (Tの最大値) * (要素数) が収まる型
// TN2 : 2 * (Tの最大値) * (要素数)^2 が収まる型
template <typename T, typename TN, typename TN2>
struct linear_add_range_min {
  private:
    struct point {
        int x;
        T y;
        static TN cross(const point &a, const point &b, const point &c) {
            return (TN)(b.y - a.y) * (c.x - a.x) - (TN)(c.y - a.y) * (b.x - a.x);
        }
    };
    struct node {
        point lbr, rbr;
        T lza, lzb;
        node(int x, T y): lbr{x, y}, rbr{x, y}, lza(0), lzb(0) {}
        node(): lza(0), lzb(0) {}
    };

    int _n, size, log;
    std::vector<node> nd;
    std::vector<bool> correct;

    void all_apply(int k, T a, T b) {
        nd[k].lbr.y += a * nd[k].lbr.x + b;
        nd[k].rbr.y += a * nd[k].rbr.x + b;
        if (k < size) nd[k].lza += a, nd[k].lzb += b;
    }

    void push(int k) {
        all_apply(2 * k, nd[k].lza, nd[k].lzb);
        all_apply(2 * k + 1, nd[k].lza, nd[k].lzb);
        nd[k].lza = nd[k].lzb = 0;
    }

    int leftmost(int k) {
        int msb = 31 - __builtin_clz(k);
        return (k - (1 << msb)) << (log - msb);
    }

    void pull(int k) {
        assert(k < size);
        push(k);
        int l = k * 2, r = k * 2 + 1;
        if (!correct[l]) pull(l);
        if (!correct[r]) pull(r);
        int splitx = leftmost(r);
        T lza = 0, lzb = 0, lzA = 0, lzB = 0;
        point a = nd[l].lbr, b = nd[l].rbr, c = nd[r].lbr, d = nd[r].rbr;

        auto movel = [&](bool f) {
            lza += nd[l].lza, lzb += nd[l].lzb;
            l = l * 2 + f;
            a = nd[l].lbr, b = nd[l].rbr;
            a.y += lza * a.x + lzb;
            b.y += lza * b.x + lzb;
        };
        auto mover = [&](bool f) {
            lzA += nd[r].lza, lzB += nd[r].lzb;
            r = r * 2 + f;
            c = nd[r].lbr, d = nd[r].rbr;
            c.y += lzA * c.x + lzB;
            d.y += lzA * d.x + lzB;
        };

        while ((l < size) || (r < size)) {
            TN s1 = point::cross(a, b, c);
            if (l < size && s1 > 0) {
                movel(0);
            } else if (r < size && point::cross(b, c, d) > 0) {
                mover(1);
            } else if (l >= size) {
                mover(0);
            } else if (r >= size) {
                movel(1);
            } else {
                TN2 s2 = point::cross(b, a, d);
                if (s1 + s2 == 0 || (TN2)s1 * (d.x - splitx) < s2 * (splitx - c.x)) {
                    movel(1);
                } else {
                    mover(0);
                }
            }
        }
        nd[k].lbr = a;
        nd[k].rbr = c;
        correct[k] = true;
    }

    T min_subtree(int k, T a = 0, T b = 0) {
        if (!correct[k]) pull(k);
        while (k < size) {
            bool f = (nd[k].lbr.y - nd[k].rbr.y) > a * (nd[k].rbr.x - nd[k].lbr.x);
            a += nd[k].lza;
            b += nd[k].lzb;
            k = k * 2 + f;
        }
        return nd[k].lbr.y + a * nd[k].lbr.x + b;
    }

  public:
    linear_add_range_min() : linear_add_range_min(0) {}
    linear_add_range_min(int n) : linear_add_range_min(std::vector<T>(n, 0)) {}
    linear_add_range_min(const std::vector<T>& v) : _n(int(v.size())) {
        log = bit_ceil_log((unsigned int)_n);
        size = 1 << log;
        nd = std::vector<node>(2 * size);
        correct = std::vector<bool>(2 * size, true);
        for (int i = 0; i < size; i++) nd[size + i] = node(i, (i < _n ? v[i] : 0));
        for (int i = size - 1; i >= 1; i--) pull(i);
    }
    
    void set(int p, T x) {
        assert(0 <= p && p < _n);
        int P = p;
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        if (nd[p].lbr.y == x) return;
        nd[p] = node(P, x);
        for (int i = 1; i <= log; i++) correct[p >> i] = false;
    }

    T get(int p) {
        assert(0 <= p && p < _n);
        p += size;
        T a = 0, b = 0;
        for (int i = log; i >= 1; i--) a += nd[p >> i].lza, b += nd[p >> i].lzb;
        return nd[p].lbr.y + (p - size) * a + b;
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
            if (((l >> i) << i) != l) correct[l >> i] = false;
            if (((r >> i) << i) != r) correct[(r - 1) >> i] = false;
        }
    }

    static constexpr T inf = std::numeric_limits<T>::max();
    
    // [l, r)のmin
    // a, bを与えた場合ax+bを足したと仮定してのmin (実際には足さない)
    T prod(int l, int r, T a = 0, T b = 0) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return inf;
        l += size;
        r += size;
        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }
        T res = inf;
        while (l < r) {
            if (l & 1) res = std::min(res, min_subtree(l++, a, b));
            if (r & 1) res = std::min(res, min_subtree(--r, a, b));
            l >>= 1;
            r >>= 1;
        }
        return res;
    }
};
#endif
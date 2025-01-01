#ifndef _FENWICK_TREE_RANGE_ADD_H_
#define _FENWICK_TREE_RANGE_ADD_H_
#include <vector>
#include <iostream>
#include "../../math/base/bit_ceil.hpp"

template<typename T>
struct fenwick_tree_range_add {
    int M, W;
    std::vector<std::pair<T, T>> V;
    fenwick_tree_range_add() {}
    fenwick_tree_range_add(int N) : M(N), W(bit_ceil(M)), V(M + 1, {0, 0}) {}
    fenwick_tree_range_add(const std::vector<T> &v) : M(v.size()), W(bit_ceil(M)), V(M + 1, {0, 0}) {
        for (int i = 0; i < M; i++) V[i + 1].first = v[i];
        for (int i = 1; i <= M; i++) {
            int nxt = i + (i & (-i));
            if (nxt <= M) V[nxt].first += V[i].first;
        }
    }

    // V[k] += x
    void apply(int k, T x) {
        for (int i = k + 1; i <= M; i += (i & (-i))) {
            V[i].first += x;
        }
    }

    // V[l, r) += x
    void apply(int l, int r, T x) {
        T a = x * l, b = x * r;
        for (int i = l + 1; i <= M; i += (i & (-i))) {
            V[i].first -= a;
            V[i].second += x;
        }
        for (int i = r + 1; i <= M; i += (i & (-i))) {
            V[i].first += b;
            V[i].second -= x;
        }
    }

    // V[0, r)
    T prod(int r) {
        T a = 0, b = 0;
        for (int i = r; i > 0; i -= (i & (-i))) {
            a += V[i].first;
            b += V[i].second;
        }
        return a + b * r;
    }

    // V[l, r)
    T prod(int l, int r) {
        if (l >= r) return 0;
        return prod(r) - prod(l);
    }

    // f(op(a[0], a[1], ..., a[r - 1])) = trueとなる最大のr
    // f(id) = true
    // 0 <= r <= N
    template<bool (*f)(T)>
    int max_right() {
        assert(f(0));
        int r = 0, w = W;
        T a = 0, b = 0, c, d;
        while (w) {
            if (r + w <= M) {
                c = a + V[r + w].first;
                d = b + V[r + w].second;
                if (f(c + d * (r + w))) {
                    a = c;
                    b = d;
                    r += w;
                }
            }
            w >>= 1;
        }
        return r;
    }
};
#endif
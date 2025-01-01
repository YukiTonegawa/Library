#ifndef _FENWICK_TREE_H_
#define _FENWICK_TREE_H_
#include <vector>
#include <cassert>
#include "../../math/base/bit_ceil.hpp"

// 可換でないと壊れる
// prefixだけなら逆元がいらない
// 区間クエリは逆元が必要
template<typename T, T (*id)(), T (*op)(T, T), T (*inv)(T)>
struct fenwick_tree_abstract {
  private:
    int N, W;
    std::vector<T> V;
  
  public:
    fenwick_tree_abstract() {}
    fenwick_tree_abstract(int _N) : N(_N), W(bit_ceil(N)), V(N + 1 , id()) {}
    fenwick_tree_abstract(const std::vector<T> &v): N(v.size()), W(bit_ceil(N)), V(1, id()) {
        V.insert(V.begin() + 1, v.begin(), v.end());
        for (int i = 1; i <= N; i++) {
            int nxt = i + (i & (-i));
            if (nxt <= N) V[nxt] += V[i];
        }
    }

    int size() {
        return N;
    }

    // V[k] <- op(V[k], x)
    void apply(int k, T x) {
        for (int i = k + 1; i <= N; i += (i & (-i))) {
            V[i] = op(V[i], x);
        }
    }

    T get(int k) {
        k++;
        T res = V[k];
        int lsb = 1;
        while ((k & lsb) == 0) {
            res = op(res, inv(V[k - lsb]));
            lsb <<= 1;
        }
        return res;
    }

    // prod[0, r)
    T prod(int r) {
        T res = id();
        for (int k = r; k > 0; k -= (k & (-k))) {
            res = op(V[k], res);
        }
        return res;
    }

    // 逆元がある必要がある
    T prod(int l, int r) {
        T sumr = id(), suml = id();
        while ((r | l) != l) {
            sumr = op(V[r], sumr);
            r -= (r & (-r));
        }
        while (l > r) {
            suml = op(V[l], suml);
            l -= (l & (-l));
        }
        return op(sumr, inv(suml));
    }

    // f(op(a[0], a[1], ..., a[r - 1])) = trueとなる最大のr
    // f(id) = true
    // 0 <= r <= N
    template<bool (*f)(T)>
    int max_right() {
        assert(f(id()));
        int r = 0, w = W;
        T s = 0, tmp;
        while (w) {
            if (r + w <= N && f((tmp = op(s, V[r + w])))) {
                s = tmp;
                r += w;
            }
            w >>= 1;
        }
        return r;
    }
};

template<typename T>
struct _fenwick_add_sum {
    using Val = T;
    static Val id() { return 0; }
    static Val inv(Val a) { return -a; }
    static Val merge(Val a, Val b) { return a + b; }
};

template<typename T>
using fenwick_tree = fenwick_tree_abstract<T, _fenwick_add_sum<T>::id, _fenwick_add_sum<T>::merge, _fenwick_add_sum<T>::inv>;
#endif
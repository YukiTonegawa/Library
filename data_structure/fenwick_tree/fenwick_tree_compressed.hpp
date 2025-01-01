#ifndef _FENWICK_TREE_COMPRESSED_H_
#define _FENWICK_TREE_COMPRESSED_H_
#include <vector>
#include <iostream>

template<typename Idx, typename T>
struct fenwick_tree_compressed {
  private:
    int M;
    std::vector<std::pair<T, T>> sum;
  
  public:
    fenwick_tree_compressed() {}
    fenwick_tree_compressed(int N) : M(N), sum(N + 1, {0, 0}) {}
    fenwick_tree_compressed(const std::vector<T> &v) : M(v.size()), sum(M + 1, {0, 0}) {
        for (int i = 0; i < M; i++) sum[i + 1].first = v[i];
        for (int i = 1; i <= M; i++) {
            int nxt = i + (i & (-i));
            if (nxt <= M) sum[nxt].first += sum[i].first;
        }
    }

    // V[k(座圧後)] += x
    void apply(int kc, T x){
        for(int i = kc + 1; i <= M; i += (i & (-i))) {
            sum[i].first += x;
        }
    }

    // [lc, rc) (元の数直線上では[l, r))にzを加算
    void apply(Idx l, Idx r, int lc, int rc, T z){
        T a = l * z, b = r * z;
        for (int i = lc + 1; i <= M; i += (i & (-i))) {
            sum[i].first -= a;
            sum[i].second += z;
        }

        for (int i = rc + 1; i <= M; i += (i & (-i))) {
            sum[i].first += b;
            sum[i].second -= z;
        }
    }

    // [0, rc) 元の数直線では[(座圧後に0になる値), r)の和
    T prod(Idx r, int rc) {
        T a = 0, b = 0;
        for (int i = rc; i > 0; i -= (i & (-i))) {
            a += sum[i].first;
            b += sum[i].second;
        }
        return a + (b * r);
    }

    // [lc, rc) (元の数直線上では[l, r))の和
    T prod(Idx l, Idx r, int lc, int rc) {
        if (lc >= rc) return 0;
        return prod(r, rc) - prod(l, lc);
    }
};
#endif
#ifndef _FENWICK_TREE_SPARSE_H_
#define _FENWICK_TREE_SPARSE_H_
#include <vector>
#include <cassert>
#include <cassert>

template<typename Idx, typename T>
struct fenwick_tree_sparse {
  private:
    int N;
    std::vector<Idx> I;
    std::vector<T> V;
  
  public:
    fenwick_tree_sparse() {}
    // idxの昇順にソート済
    fenwick_tree_sparse(const std::vector<std::pair<Idx, T>> &v): I(1, std::numeric_limits<Idx>::min()), V(1, 0) {
        for (int i = 0; i < v.size(); i++) {
            assert(I.back() <= v[i].first);
            if (I.back() == v[i].first) {
                V.back() += v[i].second;
            } else {
                I.push_back(v[i].first);
                V.push_back(v[i].second);
            }
        }
        N = I.size() - 1;
        for (int i = 1; i <= N; i++) {
            int nxt = i + (i & (-i));
            if (nxt <= N) V[nxt] += V[i];
        }
    }

    int size() {
        return N;
    }

    // V[k] <- op(V[k], x)
    // kを事前に追加していない場合エラー
    void apply(Idx k, T x) {
        auto itr = std::lower_bound(I.begin(), I.end(), k);
        assert(itr != I.end() && *itr == k);
        int l = itr - I.begin();
        for (int i = l; i <= N; i += (i & (-i))) {
            V[i] += x;
        }
    }

    // k番目に小さい点にx足す
    void apply_raw(int k, T x) {
        for (int i = k + 1; i <= N; i += (i & (-i))) {
            V[i] += x;
        }
    }

    // prod[0, r)
    T prod(Idx R) {
        int r = std::lower_bound(I.begin(), I.end(), R) - I.begin() - 1;
        T res = 0;
        for (int k = r; k > 0; k -= (k & (-k))) {
            res = op(V[k], res);
        }
        return res;
    }

    // 逆元がある必要がある
    T prod(Idx L, Idx R) {
        if (L >= R) return 0;
        int r = std::lower_bound(I.begin(), I.end(), R) - I.begin() - 1;
        int l = std::lower_bound(I.begin(), I.end(), L) - I.begin() - 1;
        if (l >= r) return 0;
        T res = 0;
        while ((r | l) != l) {
            res += V[r];
            r -= (r & (-r));
        }
        while (l > r) {
            res -= V[l];
            l -= (l & (-l));
        }
        return res;
    }
};
#endif

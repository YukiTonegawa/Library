#ifndef _TREE_PATH_STRING_H_
#define _TREE_PATH_STRING_H_
#include <numeric>
#include <algorithm>
#include <cassert>
#include "ds_accumulate.hpp"
#include "../../graph/tree/hld.hpp"

template<size_t MAX_LEN = 2000000>
struct tree_path_string {
  private:
    using rh = rolling_hash_base<MAX_LEN>;
    using ull = uint64_t;
    int N;
    hld<int> H;
    ds_accumulate<MAX_LEN> table, table_rev;
    
  public:
    tree_path_string() : N(0) {}

    template<typename T>
    tree_path_string(const csr<int> &G, const std::vector<T> &V) : N(G.N()), H(0, G) {
        std::vector<ull> v(N);
        for (int i = 0; i < N; i++) v[H.ord_point(i)] = V[i];
        table = ds_accumulate<MAX_LEN>(v);
        std::reverse(v.begin(), v.end());
        table_rev = ds_accumulate<MAX_LEN>(v);
    }

    int size() const {
        return N;
    }

    int dist(int a, int b) const {
        return H.dist(a, b);
    }

    ull get(int a) const {
        int p = H.ord_point(a);
        return table.hash_range(p, p + 1);
    }

    // 0 <= k <= dist(a, b)でない場合-1
    int jump_on_tree(int a, int b, int k) const {
        return H.jump_on_tree(a, b, k);
    }

    // a-bパス(端点含む)
    ull hash_path(int a, int b) const {
        int sz = 0;
        ull res = 0;
        for (auto [l, r] : H.ord_path(a, b)) {
            ull h = 0;
            if (l > r) {
                std::swap(l, r);
                h = table_rev.hash_range(N - r, N - l);
            } else {
                h = table.hash_range(l, r);
            }
            res = rh::concat(sz, res, h);
            sz += r - l;
        }
        return res;
    }

    // a-bパスとc-dパスのlcp(端点含む)
    int lcp(int a, int b, int c, int d) const {
        int maxlen = std::min(dist(a, b) + 1, dist(c, d) + 1);
        int idx1 = 0, idx2 = 0, res = 0;
        auto P1 = H.ord_path(a, b);
        auto P2 = H.ord_path(c, d);
        while (res < maxlen) {
            auto [l1, r1] = P1[idx1];
            auto [l2, r2] = P2[idx2];
            const ds_accumulate<MAX_LEN> &T1 = (l1 > r1 ? table_rev : table);
            const ds_accumulate<MAX_LEN> &T2 = (l2 > r2 ? table_rev : table);
            int lim = std::min(abs(r1 - l1), abs(r2 - l2));
            if (l1 > r1) l1 = N - (r1 + lim);
            if (l2 > r2) l2 = N - (r2 + lim);
            if (T1.hash_range(l1, l1 + lim) != T2.hash_range(l2, l2 + lim)) {
                int _lcp = T1.lcp(l1, T2, l2);
                return res + _lcp;
            }
            res += lim;
            if (P1[idx1].first > P1[idx1].second) P1[idx1].second += lim;
            else P1[idx1].first += lim;
            if (P1[idx1].first == P1[idx1].second) idx1++;
            if (P2[idx2].first > P2[idx2].second) P2[idx2].second += lim;
            else P2[idx2].first += lim;
            if (P2[idx2].first == P2[idx2].second) idx2++;
        }
        return res;
    }

    // -1 : [a, b] < [c, d]
    // 0 : [a, b] == [c, d]
    // 1 : [1, b] > [c, d]
    int compare(int a, int b, int c, int d) const {
        int d1 = dist(a, b) + 1, d2 = dist(c, d) + 1;
        int _lcp = lcp(a, b, c, d);
        if (_lcp == d1 || _lcp == d2) {
            if (_lcp != d2) return -1;
            if (_lcp != d1) return 1;
            return 0;
        }
        int p1 = H.jump_on_tree(a, b, _lcp);
        int p2 = H.jump_on_tree(c, d, _lcp);
        return get(p1) < get(p2) ? -1 : 1;
    } 
};
#endif
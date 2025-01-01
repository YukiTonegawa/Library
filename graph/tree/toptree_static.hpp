#ifndef _STATIC_TOPTREE_H_
#define _STATIC_TOPTREE_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include "hld.hpp"

// use_rev : パスを反転した値を覚えておくか(retooting, 探索を使わない場合、反転しても同値な演算の場合定数倍高速化できる)
template<typename H, typename L, H (*eH)(), L (*toL)(const H&), L (*mergeLL)(const L&, const L&), H (*mergeHL)(const H&, const L&), H (*mergeHH)(const H&, const H&), bool use_rev>
struct toptree_static {
  private:
    int N;
    std::vector<int> in;
    std::vector<int> Hleft, Hright, Hpar, Lleft, Lright, Lpar, Ltop;
    std::vector<H> val, Allsum, Allsumrev;
    std::vector<L> Lsum;
  
    void update_heavy(int idx) {
        int l = Hleft[idx], r = Hright[idx];
        H cval = (r == -1 ? eH() : Allsum[r]);
        if (Ltop[idx] != -1) cval = mergeHL(cval, Lsum[Ltop[idx]]);
        Allsum[idx] = mergeHH(val[idx], cval);
        if (l != -1) Allsum[idx] = mergeHH(Allsum[l], Allsum[idx]);

        if constexpr (use_rev) {
            H cvalrev = (l == -1 ? eH() : Allsumrev[l]);
            if (Ltop[idx] != -1) cvalrev = mergeHL(cvalrev, Lsum[Ltop[idx]]);
            Allsumrev[idx] = mergeHH(val[idx], cvalrev);
            if (r != -1) Allsumrev[idx] = mergeHH(Allsumrev[r], Allsumrev[idx]);
        }
    }
  
    void update_light(int idx) {
        Lsum[idx] = toL(Allsum[idx]);
        if (Lleft[idx] != -1) Lsum[idx] = mergeLL(Lsum[idx], Lsum[Lleft[idx]]);
        if (Lright[idx] != -1) Lsum[idx] = mergeLL(Lsum[idx], Lsum[Lright[idx]]);
    }
    
    void build(const hld<int> &tree, const std::vector<H> &_val) {
        std::vector<int> par(N), head(N), tail(N);
        for (int i = 0; i < N; i++) {
            int v = tree.tour[i];
            par[i] = (i == 0 ? -1 : tree.in[tree.par[v]]);
            head[i] = tree.in[tree.head[v]];
            tail[head[i]] = i;
            val[i] = _val[v];
        }

        auto compress_heavy = [&](auto &&f, int l, int r) -> int {
            if (r - l == 1) {
                update_heavy(l);
                return l;
            }
            int m = (l + r) / 2;
            if (l < m) {
                Hleft[m] = f(f, l, m);
                Hpar[Hleft[m]] = m;
            }
            if (m + 1 < r) {
                Hright[m] = f(f, m + 1, r);
                Hpar[Hright[m]] = m;
            }
            update_heavy(m);
            return m;
        };

        auto compress_light = [&](const std::vector<int> &lights) -> int {
            int m = lights.size();
            for (int i = m - 1; i >= 0; i--) {
                int idx = lights[i];
                if (2 * i + 1 < m) {
                    Lleft[idx] = lights[2 * i + 1];
                    Lpar[Lleft[idx]] = idx;
                }
                if (2 * i + 2 < m) {
                    Lright[idx] = lights[2 * i + 2];
                    Lpar[Lright[idx]] = idx;
                }
                update_light(idx);
            }
            return lights[0];
        };
        
        std::vector<std::vector<int>> Llist(tree.N);
        for (int i = N - 1; i >= 0; i--) {
            if (!Llist[i].empty()) {
                Ltop[i] = compress_light(Llist[i]);
                Lpar[Ltop[i]] = i;
            }
            if (head[i] != i) continue;
            int lid = compress_heavy(compress_heavy, i, tail[i] + 1);
            if (par[i] != -1) {
                Llist[par[i]].push_back(lid);
            }
        }
    }
public:
    toptree_static() {}
    template<typename edge = int>
    toptree_static(const hld<edge> &tree, const std::vector<H> &_val) : N(tree.N), in(tree.in), Hleft(N, -1), Hright(N, -1), 
        Hpar(N, -1), Lleft(N, -1), Lright(N, -1), Lpar(N, -1), Ltop(N, -1), val(N, eH()), Allsum(N, eH()), Lsum(N, toL(eH())) {
        if constexpr(use_rev) Allsumrev.resize(N, eH());
        build(tree, _val);
    }

    void set(int v, H x) {
        int idx = in[v];
        val[idx] = x;
        while (idx != -1) {
            while (true) {
                update_heavy(idx);
                if (Hpar[idx] == -1) break;
                idx = Hpar[idx];
            }
            while (true) {
                update_light(idx);
                if (Lpar[idx] == -1 || Ltop[Lpar[idx]] == idx) break;
                idx = Lpar[idx];
            }
            idx = Lpar[idx];
        }
    }

    L all_prod(){
        int idx = 0;
        while (Hpar[idx] != -1) idx = Hpar[idx];
        return toL(Allsum[idx]);
    }

    // rootを根としてvの部分木
    L sub_prod(int v){
        int idx = in[v], i = idx;
        H res = eH();
        while (i != -1) {
            if (idx <= i) {
                H tmp = (Hright[i] != -1 ? Allsum[Hright[i]] : eH());
                if (Ltop[i] != -1) tmp = mergeHL(tmp, Lsum[Ltop[i]]);
                res = mergeHH(res, mergeHH(val[i], tmp));
            }
            i = Hpar[i];
        }
        return toL(res);
    }

    // = evert(v) -> all_prod() (実際にはevertしない)
    L reroot_all_prod(int v) {
        static_assert(use_rev);
        auto calc_rec = [&](auto &&calc_rec, int idx) -> H {
            int i = idx;
            H A = (Hright[i] != -1 ? Allsum[Hright[i]] : eH());
            H B = (Hleft[i] != -1 ? Allsumrev[Hleft[i]] : eH());
            while (true) {
                if (idx < i) {
                    H tmp = (Hright[i] != -1 ? Allsum[Hright[i]] : eH());
                    if (Ltop[i] != -1) tmp = mergeHL(tmp, Lsum[Ltop[i]]);
                    A = mergeHH(A, mergeHH(val[i], tmp));
                }
                if (i < idx) {
                    H tmp = (Hleft[i] != -1 ? Allsumrev[Hleft[i]] : eH());
                    if (Ltop[i] != -1) tmp = mergeHL(tmp, Lsum[Ltop[i]]);
                    B = mergeHH(mergeHH(B, val[i]), tmp);
                }
                if (Hpar[i] == -1) break;
                i = Hpar[i];
            }
            if (Lpar[i] == -1) return mergeHL(A, toL(B));
            L C = toL(eH());
            int j = i;
            while (Ltop[j] != i) {
                if (j != i) C = mergeLL(C, toL(Allsum[j]));
                if (Lleft[j] != -1 && Lleft[j] != i) C = mergeLL(C, Lsum[Lleft[j]]);
                if (Lright[j] != -1 && Lright[j] != i) C = mergeLL(C, Lsum[Lright[j]]);
                i = j;
                j = Lpar[j];
            }
            H D = calc_rec(calc_rec, j);
            return mergeHL(A, toL(mergeHH(B, mergeHH(val[j], mergeHL(D, C)))));
        };
        int u = in[v];
        H x = calc_rec(calc_rec, u);
        if (Ltop[u] != -1) x = mergeHL(x, Lsum[Ltop[u]]);
        return toL(mergeHH(val[u], x));
    }
};
#endif
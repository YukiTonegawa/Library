#ifndef _RECTANGLE_CHMIN_RECTANGLE_MIN_H_
#define _RECTANGLE_CHMIN_RECTANGLE_MIN_H_
#include <vector>
#include <cassert>
#include <set>
#include "../heap/all_apply_heap.hpp"

// どの長方形も含まない領域は-infとする
// 
template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
struct rectangle_chmin_rectangle_min_static {
    struct Event {
        int lx, rx, ly, ry;
        S z;
        int i;
    };

    std::vector<Idx> Xu, Yu, Xq, Yq;
    std::vector<Event> U, Q;

    // [lx, rx), [ly, ry)の領域を op(z, 元の値) で置き換える (min, max演算のみ可能)
    void apply(Idx lx, Idx rx, Idx ly, Idx ry, S z) {
        if (lx >= rx || ly >= ry) return;
        Xu.push_back(lx);
        Xu.push_back(rx);
        Yu.push_back(ly);
        Yu.push_back(ry);
        U.push_back({(int)Xu.size() - 2, (int)Xu.size() - 1, (int)Yu.size() - 2, (int)Yu.size() - 1, z, -2});
    }
    
    // [lx, rx), [ly, ry)の領域のprod (min, max演算のみ可能)
    void prod(Idx lx, Idx rx, Idx ly, Idx ry) {
        Xq.push_back(lx);
        Xq.push_back(rx);
        Yq.push_back(ly);
        Yq.push_back(ry);
        Q.push_back({(int)Xq.size() - 2, (int)Xq.size() - 1, (int)Yq.size() - 2, (int)Yq.size() - 1, 0,(int)Q.size()});
    }

    std::vector<S> solve() {
        auto X = Xu;
        auto Y = Yu;
        std::sort(X.begin(), X.end());
        std::sort(Y.begin(), Y.end());
        X.erase(std::unique(X.begin(), X.end()), X.end());
        Y.erase(std::unique(Y.begin(), Y.end()), Y.end());

        std::vector<Event> E;

        auto _lbx = [&](Idx v) { return std::lower_bound(X.begin(), X.end(), v) - X.begin(); };
        auto _lby = [&](Idx v) { return std::lower_bound(Y.begin(), Y.end(), v) - Y.begin(); };

        for (Event &u : U) {
            u.lx = _lbx(Xu[u.lx]);
            u.rx = _lbx(Xu[u.rx]);
            u.ly = _lby(Yu[u.ly]);
            u.ry = _lby(Yu[u.ry]);
            E.push_back(u);
            u.lx = u.rx;
            u.i = -1;
            E.push_back(u);
        }

        for (Event &q : Q) {
            q.lx = _lbx(Xq[q.lx]);
            q.rx = _lbx(Xq[q.rx]);
            q.ly = _lby(Yq[q.ly]);
            q.ry = _lby(Yq[q.ry]);
            if (q.lx < q.rx && q.ly < q.ry) {
                E.push_back(q);
                q.lx = q.rx;
                q.i = -3 - q.i;
                E.push_back(q);
            }
        }
        std::vector<S> res(Q.size(), e());
        // メモリ解放
        {
            std::vector<Idx>().swap(Xu);
            std::vector<Idx>().swap(Yu);
            std::vector<Idx>().swap(Xq);
            std::vector<Idx>().swap(Yq);
            std::vector<Event>().swap(U);
            std::vector<Event>().swap(Q);
        }
        
        // クエリ長方形の終了 -> 更新 -> クエリ長方形の開始
        std::sort(E.begin(), E.end(), [](Event &a, Event &b) { 
            if (a.lx != b.lx) return a.lx < b.lx;
            return a.i < b.i;
        });

        int size = 1, log = 0;
        while (size < Y.size()) size *= 2, log++;

        {
            std::vector<all_apply_heap<std::pair<int, int>, S, op, e>> d(2 * size);
            std::vector<S> lz(2 * size, {0, 0}); //
            
            auto update = [&](int k, S z) {
                d[k].all_apply(z);
            };

            auto all_apply = [&](int k, S z) {
                d[k].all_apply(z);
                lz[k] = op(lz[k], z);
            };
            
            auto push = [&](int k) {
                all_apply(2 * k, lz[k]);
                all_apply(2 * k + 1, lz[k]);
                lz[k] = e();
            };

            auto apply = [&](int l, int r, S z) {
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
                        if (l & 1) all_apply(l++, z);
                        if (r & 1) all_apply(--r, z);
                        l >>= 1;
                        r >>= 1;
                    }
                    l = l2;
                    r = r2;
                }
                for (int i = 1; i <= log; i++) {
                    if (((l >> i) << i) != l) update(l >> i, z);
                    if (((r >> i) << i) != r) update((r - 1) >> i, z);
                }
            };

            auto insert = [&](int l, int r, int qid, int rx) {
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
                        if (l & 1) d[l++].push({rx, qid}, e());
                        if (r & 1) d[--r].push({rx, qid}, e());
                        l >>= 1;
                        r >>= 1;
                    }
                    l = l2;
                    r = r2;
                }

            };

            auto erase = [&](int l, int r) {
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
                        if (l & 1) {
                            auto [p, x] = d[l++].pop_min();
                            res[p.second] = op(res[p.second], x);
                        }
                        if (r & 1) {
                            auto [p, x] = d[--r].pop_min();
                            res[p.second] = op(res[p.second], x);
                        }
                        l >>= 1;
                        r >>= 1;
                    }
                    l = l2;
                    r = r2;
                }
            };

            for (Event &el : E) {
                if (el.i == -1) continue;
                if (el.i == -2) {
                    apply(el.ly, el.ry, el.z);
                } else if (el.i >= 0) {
                    insert(el.ly, el.ry, el.i, el.rx);
                } else {
                    erase(el.ly, el.ry);
                }
            }
        }

        {
            std::vector<std::multiset<S>> d(2 * size);

            auto insert = [&](auto &&insert, int k, int l, int r, int L, int R, S z) -> void {
                if (R <= l || r <= L) return;
                d[k].insert(z);
                if (l <= L && R <= r) return;
                insert(insert, k * 2, l, r, L, (L + R) / 2, z);
                insert(insert, k * 2 + 1, l, r, (L + R) / 2, R, z);
            };

            auto erase = [&](auto &&erase, int k, int l, int r, int L, int R, S z) -> void {
                if (R <= l || r <= L) return;
                d[k].erase(d[k].find(z));
                if (l <= L && R <= r) return;
                erase(erase, k * 2, l, r, L, (L + R) / 2, z);
                erase(erase, k * 2 + 1, l, r, (L + R) / 2, R, z);
            };

            auto prod = [&](auto &&prod, int k, int l, int r, int L, int R) -> S {
                if (R <= l || r <= L) return e();
                S x = (d[k].empty() ? e() : *(--d[k].end()));
                if (l <= L && R <= r) return x;
                x = op(x, prod(prod, k * 2, l, r, L, (L + R) / 2));
                x = op(x, prod(prod, k * 2 + 1, l, r, (L + R) / 2, R));
                return x;
            };

            for (Event &el : E) {
                if (el.i <= -3) continue;
                if (el.i == -2) {
                    insert(insert, 1, el.ly, el.ry, 0, size, el.z);
                } else if (el.i == -1) {
                    erase(erase, 1, el.ly, el.ry, 0, size, el.z);
                } else {
                    res[el.i] = op(res[el.i], prod(el.ly, el.ry));
                }
            }
        }

        return res;
    }
};
#endif
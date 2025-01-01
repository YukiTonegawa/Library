#ifndef _RECT_ADD_RECT_MIN_STATIC_H_
#define _RECT_ADD_RECT_MIN_STATIC_H_
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <limits>
#include <set>
#include "../heap/all_apply_heap.hpp"

// どの長方形も含まない領域は0とする
template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
struct rectangle_add_rectangle_min_static {
    struct Event {
        int lx, rx, ly, ry;
        S z;
        int i;
    };

    std::vector<Idx> Xu, Yu, Xq, Yq;
    std::vector<Event> U, Q;

    void add(Idx lx, Idx rx, Idx ly, Idx ry, S z) {
        if (lx >= rx || ly >= ry) return;
        Xu.push_back(lx);
        Xu.push_back(rx);
        Yu.push_back(ly);
        Yu.push_back(ry);
        U.push_back({(int)Xu.size() - 2, (int)Xu.size() - 1, (int)Yu.size() - 2, (int)Yu.size() - 1, z, -1});
    }

    void max(Idx lx, Idx rx, Idx ly, Idx ry) {
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
            u.z = -u.z;
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
                q.i = -2 - q.i;
                E.push_back(q);
            }
        }
        std::vector<S> res(Q.size(), 0);
        // メモリ解放
        {
            std::vector<Idx>().swap(Xu);
            std::vector<Idx>().swap(Yu);
            std::vector<Idx>().swap(Xq);
            std::vector<Idx>().swap(Yq);
            std::vector<Event>().swap(U);
            std::vector<Event>().swap(Q);
        }
        // クエリ長方形の終了 -> 加算/減算(最大値が更新されないように負の変化を優先) -> クエリ長方形の開始
        std::sort(E.begin(), E.end(), [](Event &a, Event &b) { 
            if (a.lx != b.lx) return a.lx < b.lx;
            if (a.i == -1 && b.i == -1) return op(a.z, b.z) != a.z;
            return a.i < b.i;
        });

        int size = 1, log = 0;
        while (size < Y.size()) size *= 2, log++;

        std::vector<all_apply_heap<std::pair<int, int>, S, op, e>> d(2 * size);
        std::vector<S> val(2 * size, 0);
        std::vector<std::pair<S, S>> lz(2 * size, {0, 0});
        
        auto update = [&](int k) {
            val[k] = op(val[2 * k], val[2 * k + 1]);
            d[k].all_apply(val[k]);
        };

        auto all_apply = [&](int k, std::pair<S, S> z) {
            d[k].all_apply(val[k] + z.second);
            val[k] += z.first;
            lz[k].second = op(lz[k].second, lz[k].first + z.second);
            lz[k].first += z.first;
        };
        
        auto push = [&](int k) {
            all_apply(2 * k, lz[k]);
            all_apply(2 * k + 1, lz[k]);
            lz[k] = {0, 0};
        };

        auto add = [&](int l, int r, S z) {
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
                    if (l & 1) all_apply(l++, {z, op(z, S(0))});
                    if (r & 1) all_apply(--r, {z, op(z, S(0))});
                    l >>= 1;
                    r >>= 1;
                }
                l = l2;
                r = r2;
            }
            for (int i = 1; i <= log; i++) {
                if (((l >> i) << i) != l) update(l >> i);
                if (((r >> i) << i) != r) update((r - 1) >> i);
            }
        };

        auto insert = [&](int l, int r, int qe, int rx) {
            if (l == r) return;

            l += size;
            r += size;

            for (int i = log; i >= 1; i--) {
                if (((l >> i) << i) != l) push(l >> i);
                if (((r >> i) << i) != r) push((r - 1) >> i);
            }
            S x = e();
            {
                int l2 = l, r2 = r;
                while (l < r) {
                    if (l & 1) x = op(x, val[l++]);
                    if (r & 1) x = op(x, val[--r]);
                    l >>= 1;
                    r >>= 1;
                }
                l = l2;
                r = r2;
            }

            {
                int l2 = l, r2 = r;
                while (l < r) {
                    if (l & 1) d[l++].push({rx, qe}, x);
                    if (r & 1) d[--r].push({rx, qe}, x);
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
            if (el.i == -1) {
                add(el.ly, el.ry, el.z);
            } else if (el.i >= 0) {
                insert(el.ly, el.ry, el.i, el.rx);
            } else {
                erase(el.ly, el.ry);
            }
        }

        return res;
    }
};
#endif
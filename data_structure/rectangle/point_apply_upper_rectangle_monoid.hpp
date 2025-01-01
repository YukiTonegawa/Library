#ifndef _POINT_APPLY_UPPER_RECT_MONOID_H_
#define _POINT_APPLY_UPPER_RECT_MONOID_H_
#include <vector>
#include "../segtree/segtree.hpp"

template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
struct point_apply_upper_rectangle_monoid {
  private:
    struct query_type {
        int id;
        Idx rx, ly, ry;
        S z;
        query_type(int id, Idx rx, Idx ly, Idx ry, S z): id(id), rx(rx), ly(ly), ry(ry), z(z) {}
    };
    struct event_type {
        Idx x;
        int q, id;
        int lyc, ryc;
        S z;
        event_type(Idx x, int q, int id, int lyc, int ryc, S z): x(x), q(q), id(id), lyc(lyc), ryc(ryc), z(z) {}
    };

    std::vector<query_type> q;
    std::vector<int> qcount{0};
    int qs = 0;
    
    void solve(int l, int r, std::vector<S> &ans) {
        if (r - l < 2) return;
        int mid = (l + r) >> 1;
        solve(l, mid, ans);
        solve(mid, r, ans);
        int left_update = (mid - l) - (qcount[mid] - qcount[l]);
        int right_query= qcount[r] - qcount[mid];
        if (left_update == 0 || right_query == 0) return;
        // compress y
        std::vector<Idx> y;
        for (int i = l; i < mid; i++) if (q[i].id == -1) y.push_back(q[i].ly);
        std::sort(y.begin(), y.end());
        y.erase(std::unique(y.begin(), y.end()), y.end());

        segtree<S, op, e> seg(y.size());

        std::vector<event_type> E;
        for (int i = l; i < mid; i++) {
            if (q[i].id == -1) {
                int y_idx = std::lower_bound(y.begin(), y.end(), q[i].ly) - y.begin();
                E.push_back(event_type(q[i].rx, 1, -1, y_idx, 0, q[i].z));
            }
        }
        for (int i = mid; i < r; i++) {
            if (q[i].id != -1) {
                int ly_idx = std::lower_bound(y.begin(), y.end(), q[i].ly) - y.begin();
                int ry_idx = std::lower_bound(y.begin(), y.end(), q[i].ry) - y.begin();
                E.push_back(event_type(q[i].rx, 0, q[i].id, ly_idx, ry_idx, 0));
            }
        }
        std::sort(E.begin(), E.end(), [](event_type &a, event_type &b) {
            if (a.x != b.x) return a.x < b.x;
            return a.q < b.q;
        });
        bool updated = false;
        for (event_type &ei : e) {
            if (ei.q == 0) {
                if (updated) {
                    S x = seg.prod(ei.lyc, ei.ryc);
                    ans[ei.id] = op(ans[ei.id], x);
                }
            } else {
                updated = true;
                seg.apply(ei.lyc, ei.z);
            }
        }
    }
  
  public:
    point_apply_upper_rectangle_monoid() {}
    
    void apply(Idx x, Idx y, S z) {
        q.push_back(query_type(-1, x, y, 0, z));
        qcount.push_back(0);
    }

    // [0, rx)　× [ly, ry)
    void prod(Idx rx, Idx ly, Idx ry) {
        q.push_back(query_type(qs++, rx, ly, ry, 0));
        qcount.push_back(1);
    }

    std::vector<S> solve() {
        std::vector<S> res(qs, e());
        for(int i = 1; i < qcount.size(); i++) qcount[i] += qcount[i - 1];
        solve(0, q.size(), res);
        return res;
    }
};
#endif
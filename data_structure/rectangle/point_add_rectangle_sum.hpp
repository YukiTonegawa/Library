#ifndef _POINT_ADD_RECTANGLE_SUM_H_
#define _POINT_ADD_RECTANGLE_SUM_H_
#include <vector>
#include "../fenwick_tree/fenwick_tree.hpp"

template<typename Idx, typename Val>
struct point_add_rectangle_sum{
  private:
    struct query_type{
        int id;
        Idx lx, rx, ly, ry;
        Val z;
        query_type(int a, Idx b, Idx c, Idx d, Idx e, Val f): id(a), lx(b), rx(c), ly(d), ry(e), z(f){}
    };

    struct event_type{
        Idx x;
        int q, id;
        int lyc, ryc;
        Val z;
        event_type(Idx a, int b, int c, int d, int e, Val f): x(a), q(b), id(c), lyc(d), ryc(e), z(f){}
    };

    std::vector<query_type> q;
    std::vector<int> qcount{0};
    int qs = 0;
    
    void solve(int l, int r, std::vector<Val> &ans) {
        static constexpr int DO_NAIVE = 100;
        if (r - l < 2) return;
        if (r - l <= DO_NAIVE) {
            for (int i = l; i < r; i++) {
                if (q[i].id == -1) continue;
                for (int j = l; j < i; j++) {
                    if (q[j].id == -1 && q[i].lx <= q[j].lx && q[j].lx < q[i].rx && q[i].ly <= q[j].ly && q[j].ly < q[i].ry) {
                        ans[q[i].id] += q[j].z;
                    }
                }
            }
            return;
        }

        int mid = (l + r) >> 1;
        solve(l, mid, ans);
        solve(mid, r, ans);
        int left_update = (mid - l) - (qcount[mid] - qcount[l]);
        int right_query= qcount[r] - qcount[mid];
        if (left_update == 0 || right_query == 0) return;

        std::vector<Idx> y;
        for (int i = l; i < mid; i++) {
            if (q[i].id == -1) {
                y.push_back(q[i].ly);
            }
        }
        std::sort(y.begin(), y.end());
        y.erase(std::unique(y.begin(), y.end()), y.end());

        fenwick_tree<Val> ft(y.size());
        std::vector<event_type> e;
        for (int i = l; i < mid; i++) {
            if (q[i].id == -1) {
                int y_idx = std::lower_bound(y.begin(), y.end(), q[i].ly) - y.begin();
                e.push_back(event_type(q[i].lx, 2, -1, y_idx, 0, q[i].z));
            }
        }

        for (int i = mid; i < r; i++) {
            if (q[i].id != -1) {
                int ly_idx = std::lower_bound(y.begin(), y.end(), q[i].ly) - y.begin();
                int ry_idx = std::lower_bound(y.begin(), y.end(), q[i].ry) - y.begin();
                e.push_back(event_type(q[i].lx, 0, q[i].id, ly_idx, ry_idx, 0));
                e.push_back(event_type(q[i].rx, 1, q[i].id, ly_idx, ry_idx, 0));
            }
        }

        std::sort(e.begin(), e.end(), [](event_type &a, event_type &b) {
            if (a.x != b.x) return a.x < b.x;
            return a.q < b.q;
        });

        for (event_type &ei : e) {
            if (ei.q == 0) {
                ans[ei.id] -= ft.prod(ei.lyc, ei.ryc);
            } else if(ei.q == 1) {
                ans[ei.id] += ft.prod(ei.lyc, ei.ryc);
            } else {
                ft.apply(ei.lyc, ei.z);
            }
        }
    }

  public:
    point_add_rectangle_sum(){}

    void apply(Idx x, Idx y, Val z) {
        q.push_back(query_type(-1, x, 0, y, 0, z));
        qcount.push_back(0);
    }

    void prod(Idx lx, Idx rx, Idx ly, Idx ry) {
        q.push_back(query_type(qs++, lx, rx, ly, ry, 0));
        qcount.push_back(1);
    }

    std::vector<Val> solve() {
        std::vector<Val> res(qs, 0);
        for (int i = 1; i < qcount.size(); i++) {
            qcount[i] += qcount[i - 1];
        }
        solve(0, q.size(), res);
        return res;
    }
};


#endif
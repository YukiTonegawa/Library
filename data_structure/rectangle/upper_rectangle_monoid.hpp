#ifndef _UPPER_RECT_MONOID_H_
#define _UPPER_RECT_MONOID_H_
#include <vector>
#include "../segtree/segtree.hpp"

template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
struct upper_rectangle_monoid {
  private:
    struct Point {
        Idx x, y;
        S z;
    };
    struct Query {
        Idx rx, ly, ry;
    };
    std::vector<Point> P;
    std::vector<Query> Q;

  public:
    void apply(Idx x, Idx y, S z) {
        P.push_back(Point{x, y, z});
    }
    
    // [0, rx) × [ly, ry)のmin
    void prod(Idx rx, Idx ly, Idx ry) {
        Q.push_back(Query{rx, ly, ry});
    }
  
    std::vector<S> solve() {
        struct Event {
            Idx x;
            int ly, ry;
            int id;
        };
        int N = Q.size();
        std::vector<S> ans(N, e());
        if (P.empty() || Q.empty()) return ans;
        std::vector<Event> Q2(N);
        std::vector<Idx> Y;
        std::sort(P.begin(), P.end(), [](const Point &a, const Point &b) { return a.y < b.y; });
        for (Point &t : P) {
            if (Y.empty() || Y.back() != t.y) Y.push_back(t.y);
            t.y = int(Y.size()) - 1;
        }
        for (int i = 0; i < N; i++) {
            int ly = std::lower_bound(Y.begin(), Y.end(), Q[i].ly) - Y.begin();
            int ry = std::lower_bound(Y.begin(), Y.end(), Q[i].ry) - Y.begin();
            Q2[i] = Event{Q[i].rx, ly, ry, i};
        }
        std::sort(P.begin(), P.end(), [](const Point &a, const Point &b) { return a.x < b.x; });
        std::sort(Q2.begin(), Q2.end(), [](const Event &a, const Event &b) { return a.x < b.x; });
        
        segtree<S, op, e> seg(Y.size());
    
        int p = 0, q = 0;
        while (q < N) {
            if (p == P.size() || Q2[q].x <= P[p].x) {
                S x = seg.prod(Q2[q].ly, Q2[q].ry);
                ans[Q2[q].id] = op(ans[Q2[q].id], x);
                q++;
            } else {
                S tmp = seg.get(P[p].y);
                seg.set(P[p].y, op(tmp, P[p].z));
                p++;
            }
        }
        return ans;
    }
};
#endif
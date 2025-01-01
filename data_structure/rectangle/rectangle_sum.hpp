#ifndef _RECTANGLE_SUM_H_
#define _RECTANGLE_SUM_H_
#include <vector>
#include "../fenwick_tree/fenwick_tree.hpp"

template<typename Idx, typename Val>
struct rectangle_sum {
  private:
    struct Point{
        Idx x, y;
        Val z;
    };
    struct Query{
        Idx rx, ly, ry;
        int id;
    };
    std::vector<Point> P;
    std::vector<Query> Q;
  public:
  
    void apply(Idx x, Idx y, Val z) {
        P.push_back({x, y, z});
    }
    
    void prod(Idx lx, Idx rx, Idx ly, Idx ry) {
        int id = Q.size() / 2 + 1;
        Q.push_back({lx, ly, ry, -id});
        Q.push_back({rx, ly, ry, id});
    }
  
    std::vector<Val> solve() {
        int N = Q.size() / 2;
        if (P.empty() || Q.empty()) return std::vector<Val>(N, 0);
        std::vector<Val> ans(N, 0);
        
        std::vector<Idx> Y;
        std::sort(P.begin(), P.end(), [](const Point &a, const Point &b){return a.y < b.y;});
        for (Point &t : P) {
            if (Y.empty() || Y.back() != t.y) Y.push_back(t.y);
            t.y = int(Y.size()) - 1;
        }
        for (int i = 0; i < N; i++) {
            int ly = std::lower_bound(Y.begin(), Y.end(), Q[2 * i].ly) - Y.begin();
            int ry = std::lower_bound(Y.begin(), Y.end(), Q[2 * i].ry) - Y.begin();
            Q[2 * i].ly = Q[2 * i + 1].ly = ly;
            Q[2 * i].ry = Q[2 * i + 1].ry = ry;
        }

        std::sort(P.begin(), P.end(), [](const Point &a, const Point &b){return a.x < b.x;});
        std::sort(Q.begin(), Q.end(), [](const Query &a, const Query &b){return a.rx < b.rx;});

        fenwick_tree<Val> ft(Y.size());
        int p = 0, q = 0;
        while (q < 2 * N) {
            if (p == P.size() || Q[q].rx <= P[p].x) {
                Val sum = ft.prod(Q[q].ly, Q[q].ry);
                if (Q[q].id < 0) {
                    int id = -Q[q].id - 1;
                    ans[id] -= sum;
                } else {
                    int id = Q[q].id - 1;
                    ans[id] += sum;
                }
                q++;
            } else {
                ft.apply(P[p].y, P[p].z);
                p++;
            }
        }
        return ans;
    }
};
#endif
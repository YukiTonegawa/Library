#ifndef _CUBOID_SUM_H_
#define _CUBOID_SUM_H_
#include "point_add_rectangle_sum.hpp"

template<typename Idx, typename Val>
struct cuboid_sum {
  private:
    point_add_rectangle_sum<Idx, Val> rect;
    std::vector<std::tuple<Idx, Idx, Idx, Val>> P;
    struct qst {
        Idx rx, ly, ry, lz, rz;
        int id;
        bool add;
    };

    std::vector<qst> Q;
    std::vector<Val> _solve() {
        std::sort(P.begin(), P.end());
        std::sort(Q.begin(), Q.end(), [&](qst &a, qst &b) {return a.rx < b.rx;});
        int j = 0;
        for (auto &q : Q) {
            while (j < P.size() && std::get<0>(P[j]) < q.rx) {
                auto [x, y, z, w] = P[j++];
                rect.apply(y, z, w);
            }
            rect.prod(q.ly, q.ry, q.lz, q.rz);
        }

        std::vector<Val> ans(Q.size() / 2);
        auto s = rect.solve();
        j = 0;
        for (auto &q : Q) {
            if (q.add) ans[q.id] += s[j++];
            else ans[q.id] -= s[j++];
        }
        return ans;
    }

  public:
    cuboid_sum() {}
    
    // (x, y, z)に重みwを追加
    void apply(Idx x, Idx y, Idx z, Val w) {
        P.push_back({x, y, z, w});
    }

    // [lx, rx) × [ly, ry) × [lz, rz)の点の重みの和
    void prod(Idx lx, Idx rx, Idx ly, Idx ry, Idx lz, Idx rz) {
        int id = Q.size() / 2;
        Q.push_back({rx, ly, ry, lz, rz, id, true});
        Q.push_back({lx, ly, ry, lz, rz, id, false});
    }

    // O((N + Q)log^2(N + Q))
    std::vector<Val> solve() {
        return _solve();
    }
};
#endif
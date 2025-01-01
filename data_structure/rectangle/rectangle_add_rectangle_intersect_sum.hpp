#ifndef _RECT_ADD_RECT_INTERSECT_SUM_H_
#define _RECT_ADD_RECT_INTERSECT_SUM_H_
#include <vector>
#include <numeric>
#include <cassert>
#include "point_add_rectangle_sum.hpp"

template<typename Idx, typename Val>
struct rectangle_add_rectangle_intersect_sum {
  private:
    point_add_rectangle_sum<Idx, Val> rect_ur, rect_ul, rect_lr, rect_ll;
    static constexpr Idx inf = std::numeric_limits<Idx>::max() / 2;
    static constexpr Idx minf = std::numeric_limits<Idx>::min() / 2;
    int q = 0;
    Val sum = 0;
    bool built = false;
    std::vector<Val> ans;
  
  public:
    // [lx, rx) × [ly, ry)を追加
    void apply(Idx lx, Idx rx, Idx ly, Idx ry, Val z) {
        assert(!built);
        if (lx == rx || ly == ry) return;
        sum += z;
        // 右上
        rect_ur.apply(lx, ry, z);
        // 左上
        rect_ul.apply(lx, ly, z);
        // 左下
        rect_ll.apply(rx, ly, z);
        // 右下
        rect_lr.apply(rx, ry, z);
    }

    // [lx, rx) × [ly, ry)との共通部分の面積が0より大きい長方形の数
    void prod(Idx lx, Idx rx, Idx ly, Idx ry) {
        assert(!built);
        ans.push_back(sum); // クエリ時点での長方形の和
        q++;
        // (1)
        // 1, 2
        rect_ur.prod(minf, rx, minf, ly + 1);

        // (2)
        // 3, 4
        rect_ul.prod(rx, inf, minf, ry);

        // (3)
        // 5, 6
        rect_ll.prod(lx + 1, inf, ry, inf);

        // (4)
        // 7 8
        rect_lr.prod(minf, lx + 1, ly + 1, inf);
    }

    std::vector<Val> solve() {
        assert(!built);
        built = true;
        auto qur = rect_ur.solve();
        auto qul = rect_ul.solve();
        auto qll = rect_ll.solve();
        auto qlr = rect_lr.solve();
        for (int i = 0; i < q; i++) {
            ans[i] -= qur[i];
            ans[i] -= qul[i];
            ans[i] -= qll[i];
            ans[i] -= qlr[i];
        }
        return ans;
    }
};

#endif
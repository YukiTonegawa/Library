#ifndef _RECTANGLE_INTERSECT_SUM_H_
#define _RECTANGLE_INTERSECT_SUM_H_
#include <array>
#include <tuple>
#include <algorithm>
#include "rectangle_sum.hpp"

//       ┃         ┃
//   1   ┃    8    ┃    7
//       ┃         ┃  
// ━━━━━━━━━━━━━━━━━━━━━━━━━━
//       ┃         ┃
//   2   ┃         ┃    6
//       ┃         ┃
// ━━━━━━━━━━━━━━━━━━━━━━━━━━
//       ┃         ┃
//   3   ┃    4    ┃    5
//       ┃         ┃

// 質問される長方形に対して交差する(共通部分の面積が0より大きい)長方形の重みの和は
//                                  (1)                     (2)                     (3)                      (4)
// (追加されている長方形の和) - (右上が1, 2にある長方形) - (左上が3, 4にある長方形) - (左下が5, 6にある長方形) - (右下が7, 8にある長方形)

// これらは点追加長方形和で求められる クエリあたり O(logN)(静的) O(log^2 N)(動的)

template<typename Idx, typename Val>
struct rectangle_intersect_sum {
  private:
    rectangle_sum<Idx, Val> rect_ur, rect_ul, rect_lr, rect_ll;
    static constexpr Idx inf = std::numeric_limits<Idx>::max() / 2;
    static constexpr Idx minf = std::numeric_limits<Idx>::min() / 2;
    bool built = false;
    int q = 0;
    Val sum = 0;

  public:
    // [lx, rx) × [ly, ry), 重みzを追加
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

    // [lx, rx) × [ly, ry)との共通部分の面積が0より大きい長方形の和
    void prod(Idx lx, Idx rx, Idx ly, Idx ry) {
        assert(!built);
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
        std::vector<Val> ans(q, sum);
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
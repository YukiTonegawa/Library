#ifndef _MANHATTAN_CONTOUR_QUERY_H_
#define _MANHATTAN_CONTOUR_QUERY_H_

// 距離がd以内の点に更新取得 (rectangle sum)
#include <tuple>

// {x, y}からマンハッタン距離d以内の領域を45度回転(チェビシェフ距離)した矩形領域 {lx, rx, ly, ry}
template<typename Idx>
std::tuple<Idx, Idx, Idx, Idx> manhattan_contour(Idx x, Idx y, Idx d) {
    return {x + y - d, x + y + d + 1, x - y - d, x - y + d + 1};
}


#endif
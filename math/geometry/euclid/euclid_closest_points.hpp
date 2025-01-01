#ifndef _EUCLID_CLOSEST_POINTS_H_
#define _EUCLID_CLOSEST_POINTS_H_
#include <cassert>
#include "../../../typical/sequence/sort.hpp"

// T2は距離の2乗を表現できる型
// https://judge.u-aizu.ac.jp/onlinejudge/review.jsp?rid=8589677#1
template<typename T, typename T2>
std::pair<int, int> euclid_closest_points(const std::vector<std::pair<T, T>> &P){
  int n = P.size();
  assert(n >= 2);
  using point = std::tuple<T, T, int>;
  std::vector<point> p(n);
  for(int i = 0; i < n; i++) p[i] = {P[i].first, P[i].second, i};
  std::sort(p.begin(), p.end());
  auto dist2 = [&](int i, int j) -> T2 {
    T2 dx = P[i].first - P[j].first;
    T2 dy = P[i].second - P[j].second;
    return dx * dx + dy * dy;
  };
  auto diffsq = [](T a, T b) -> T2 {
    return T2(a - b) * T2(a - b);
  };
  auto calc = [&](auto &&calc, int l, int r) -> std::tuple<int, int, T2> {
    if(r - l <= 1) return {-1, -1, std::numeric_limits<T2>::max()};
    int m = (l + r) / 2;
    T midx = std::get<0>(p[m]);
    auto [a, b, c] = calc(calc, l, m);
    auto [d, e, f] = calc(calc, m, r);
    if(f < c) std::swap(a, d), std::swap(b, e), std::swap(c, f);
    std::vector<point> rp;
    for(int i = m; i < r; i++) if(diffsq(std::get<0>(p[i]), midx) < c) rp.push_back(p[i]);
    for(int i = l, j = 0; i < m; i++){
      auto [x, y, id] = p[i];
      if(diffsq(x, midx) >= c) continue;
      while(j < rp.size() && (std::get<1>(rp[j]) < y || diffsq(std::get<1>(rp[j]), y) <= c)) j++;
      for(int k = j - 1; k >= 0 && (std::get<1>(rp[k]) >= y || diffsq(std::get<1>(rp[k]), y) < c); k--){
        T2 tmp = dist2(id, std::get<2>(rp[k]));
        if(tmp < c) a = id, b = std::get<2>(rp[k]), c = tmp;
      }
    }
    __merge_sorted(p, l, m, r, [](const point &p1, const point &p2){return std::get<1>(p1) < std::get<1>(p2);});
    return {a, b, c};
  };
  auto [resa, resb, _] = calc(calc, 0, n);
  return {resa, resb};
}

#endif
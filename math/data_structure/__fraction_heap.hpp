/*
#ifndef _FRACTION_HEAP_H_
#define _FRACTION_HEAP_H_
#include <vector>
#include <algorithm>
#include <cassert>
#include <set>
#include "../geometry/hull/dynamic_upperhull.hpp"


// T : 2 * 座標の最大値が収まる型
// T2 : (2 * 座標の最大値)^2 が収まる型
// T3 : (2 * 座標の最大値)^3 が収まる型
template<typename T, typename T2, typename T3>
struct fraction_heap{
  dynamic_upper_hull<T, T2, T3> hull;
  std::multiset<std::pair<T, T>> st; // 同じ点が凸包に複数追加されないようにする(同じx座標の点のうちy座標が最大のもの1つのみ追加)
  std::vector<std::pair<T, T>> v;
  T2 dx, dy;
  fraction_heap(): dx(0), dy(0){}
  // vector<{分子, 分母}>
  fraction_heap(const std::vector<std::pair<T, T>> &V): dx(0), dy(0){
    std::vector<T> X;
    for(auto [y, x] : V){
      assert(x != 0);
      st.insert({x, y});
      X.push_back(x);
    }
    std::sort(X.begin(), X.end());
    X.erase(std::unique(X.begin(), X.end()), X.end());
    for(T x : X){
      auto itr = st.lower_bound({x + 1, std::numeric_limits<T>::min()});
      itr--;
      v.push_back(*itr);
    }
    hull = dynamic_upper_hull<T, T2, T3>(v);
  }
  int size(){ return st.size(); }
  void add_bunshi(T a){ dy += a; }
  void add_bunbo(T a){ dx += a; }

  // a / bが最大になる{a, b}
  std::pair<T, T> max(){
    int i = hull.max_slope(-dx, -dy);
    auto [x, y] = v[i];
    return {y + dy, x + dx};
  }
  std::pair<T, T> pop_max(){
    int i = hull.max_slope(-dx, -dy);
    auto [x, y] = v[i];
    auto itr = st.find({x, y});
    itr = st.erase(itr);
    // 同じx座標の点が存在する場合置換
    if(itr != st.begin() && (--itr)->first == x){
      hull.change_y(i, itr->second);
      v[i].second = itr->second;
    }else{
      hull.erase(i);
    }
    return {y + dy, x + dx};
  }
};
#endif
*/
#ifndef _FRACTION_HEAP_BBST_H_
#define _FRACTION_HEAP_BBST_H_
#include <vector>
#include <algorithm>
#include <cassert>
#include <set>
#include "../geometry/hull/dynamic_upper_hull_bbst.hpp"

// T2は(yの最大値) * (xの最大値)^2くらいになる
template<typename T, typename T2>
struct fraction_heap_bbst{
  static constexpr T minf = std::numeric_limits<T>::min();
  dynamic_upper_hull_bbst<T, T2> hull;
  
  std::multiset<std::pair<T, T>> st; // 同じ点が凸包に複数追加されないようにする(同じx座標の点のうちy座標が最大のもの1つのみ追加)
  fraction_heap_bbst(){}
  // vector<{分子, 分母}>
  fraction_heap_bbst(const std::vector<std::pair<T, T>> &V){
    std::vector<T> X;
    for(auto [y, x] : V){
      assert(x != 0);
      st.insert({x, y});
      X.push_back(x);
    }
    std::sort(X.begin(), X.end());
    X.erase(std::unique(X.begin(), X.end()), X.end());
    std::vector<std::pair<T, T>> v;
    for(T x : X){
      auto itr = st.lower_bound({x + 1, minf});
      itr--;
      v.push_back(*itr);
    }
    hull = dynamic_upper_hull_bbst<T, T2>(v);
  }
  int size(){ return st.size(); }
  void add_bunshi(T x){ hull.shift_all_y(x); }
  void add_bunbo(T x){ hull.shift_all_x(x); }
  // a / bが最大になる{a, b}
  std::pair<T, T> max(){
    auto [x, y] = hull.get_val(hull.find_max_frac());
    return {y, x};
  }
  std::pair<T, T> pop_max(){
    auto v = hull.find_max_frac();
    auto [x, y] = hull.get_val(v);
    auto itr = st.find({v->lbr.x, v->lbr.y});
    itr = st.erase(itr);
    // 同じx座標の点が存在する場合置換
    if(itr != st.begin() && (--itr)->first == v->lbr.x){
      hull.change_y(v, y - (v->lbr.y - itr->second));
    }else{
      hull.erase(v);
    }
    return {y, x};
  }
  // {分子, 分母}
  void push(T y, T x){
    auto v = hull.find_x(x);
    x -= hull.dx, y -= hull.dy;
    if(!v){
      hull.insert(x + hull.dx, y + hull.dy);
    }else if(v->lbr.x == x && v->lbr.y < y){
      hull.change_y(v, y);
    }
    st.insert({x, y});
  }
};
#endif
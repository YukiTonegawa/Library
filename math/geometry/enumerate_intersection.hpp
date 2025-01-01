#ifndef _ENUMERATE_INTERSECTION_H_
#define _ENUMERATE_INTERSECTION_H_
#include <vector>
#include <cassert>
#include <queue>
#include <algorithm>
// 直線の交点を平面操作で列挙
// O(交点数 * logN)
template<typename T>
struct enumerate_intersection{
  int N;
  using __Line = std::pair<T, T>;
  std::vector<__Line> L;
  T curX = std::numeric_limits<T>::min();
  using P = std::pair<T, int>;
  std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
  void push(T a, T b){L.push_back({a, b});}
  void build(){
    std::sort(L.begin(), L.end(), [](__Line A, __Line B){
      if(A.first != B.first) return A.first > B.first;
      return A.second < B.second;
    });
    N = L.size();
    for(int i = 0; i < N - 1; i++){
      auto [a, b] = L[i];
      auto [c, d] = L[i + 1];
      if(a == c) continue;
      assert(a > c);
      T tp = ceil_div(d - b, a - c);
      pq.push({tp, i});
    }
  }
  void next(T nx){
    assert(curX <= nx);
    curX = nx;
    while(!pq.empty() && pq.top().first <= curX){
      auto [t, id] = pq.top();
      pq.pop();
      assert(id + 1 < N);
      auto [a, b] = L[id];
      auto [c, d] = L[id + 1];
      if(a <= c) continue;
      T t2 = ceil_div(d - b, a - c);
      if(t2 > curX) continue;
      std::swap(L[id], L[id + 1]); // 交差
      if(id >= 1){
        std::tie(a, b) = L[id - 1];
        std::tie(c, d) = L[id];
        if(a > c) pq.push({ceil_div(d - b, a - c), id - 1});
      }
      if(id + 2 < L.size()){
        std::tie(a, b) = L[id + 1];
        std::tie(c, d) = L[id + 2];
        if(a > c) pq.push({ceil_div(d - b, a - c), id + 1});
      }
    }
  }
  // curXにおいてy以上の数
  int uppercnt(T y){
    int l = -1, r = N;
    while(r - l > 1){
      int m = (l + r) / 2;
      if((__int128_t)L[m].first * curX + L[m].second >= y) r = m;
      else l = m;
    }
    assert(0 <= r && r <= N);
    return N - r;
  }
};
#endif
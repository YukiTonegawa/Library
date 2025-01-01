#ifndef _CIRCLE_H_
#define _CIRCLE_H_
#include <functional>
#include <cassert>

// F := 普通の区間クエリ
// G := 合成
template<typename T>
T cicle_range_query(long long N, long long l, long long r, 
  std::function<T(long long, long long)> F, std::function<T(T, T)> G){
  assert(l <= r);
  long long d = l / N;
  l -= d * N;
  r -= d * N;
  if(r <= N) return F(l, r);
  T res = F(l, N);
  r -= N;
  d = r / N;
  r -= d * N;
  // pow
  if(d){
    T a, b = F(0, N);
    bool first = true;
    while(d){
      if(d & 1){
        if(first){
          a = b;
          first = false;
        }else{
          a = G(a, b);
        }
      }
      b = G(b, b);
      d >>= 1;
    }
    res = G(res, a);
  }
  if(r) res = G(res, F(0, r));
  return res;
}

template<typename T>
std::pair<long long, T> circle_binary_search(long long N, long long l, 
  std::function<T(long long, long long)> F, std::function<T(T, T)> G, std::function<bool(T)> H){
  long long rem = l % N;
  T x = F(rem, N);
  if(H(x)){
    long long L = rem - 1, R = N;
    while(R - L > 1){
      long long M = (L + R) / 2;
      if(H(F(rem, M + 1))){
        R = M;
      }else{
        L = M;
      }
    }
    return {l - rem + R, F(rem, R + 1)};
  }
  long long idx = l - rem + N;
  {
    long long loopcnt = 0;
    T y = F(0, N);
    std::vector<T> tmp;
    while(!H(G(x, y))){
      tmp.push_back(y);
      y = G(y, y);
    }
    for(int i = (int)tmp.size() - 1; i >= 0; i--){
      if(!H(x, tmp[i])){
        x = G(x, tmp[i]);
        loopcnt += 1LL << i;
      }
    }
    idx += loopcnt * N;
  }
  {
    long long L = -1, R = N;
    while(R - L > 1){
      long long M = (L + R) / 2;
      if(H(G(x, F(0, M + 1)))){
        R = M;
      }else{
        L = M;
      }
    }
    idx += R;
    x = G(x, G(0, R + 1));
  }
  return {idx, x};
}
#endif
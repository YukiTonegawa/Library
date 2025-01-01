#ifndef _ENUMERATE_H_
#define _ENUMERATE_H_
#include <vector>
#include <cassert>

template<typename T>
struct enumerate_permutation{
  int n;
  std::vector<T> v;
  enumerate_permutation(const std::vector<T> &v): n(v.size()), v(v){
    assert(0 < n);
  }
  // 次の状態がある場合は更新してtrue, そうでない場合は何もせずfalse
  bool next(){
    return std::next_permutation(v.begin(), v.end());
  }
  // 前の状態ある場合は更新してtrue, そうでない場合は何もせずfalse
  bool prev(){
    return std::prev_permutation(v.begin(), v.end());
  }
  // 配列 -> ハッシュ
  // 開始の1, 要素の数の1, 仕切りの0 {1, 2, 3} -> 1101101110
  // assert (任意の値 >= 0) かつ (総和 + n <= 63)
  unsigned long long encode_small_sum(const std::vector<T> &val)const{
    unsigned long long res = 1;
    T S = 0;
    for(T x : val){
      assert(x >= 0);
      res <<= x;
      res |= (1ULL << x) - 1;
      res <<= 1;
      S += x;
      assert(S + n <= 63);
    }
    return res;
  }
  // ハッシュ -> 配列
  std::vector<T> decode_small_sum(unsigned long long x)const{
    assert(x && x % 2 == 0);
    std::vector<T> res;
    int cnt = 0;
    for(int i = 62 - __builtin_clzll(x); i >= 0; i--){
      if((x >> i) & 1){
        cnt++;
      }else{
        res.push_back(cnt);
        cnt = 0;
      }
    }
    return res;
  }
};
#endif
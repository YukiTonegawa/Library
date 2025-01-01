#ifndef _TIMER_H__
#define _TIMER_H__
#include <chrono>
#include <cassert>

// ms単位で現在の時刻を取得
long long timems(){
  auto p = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(p).count();
}
// 初回呼び出し時からの経過時間(ms)
template<int id>
long long elapse(){
  static long long s = timems();
  return timems() - s;
}

template<int id>
struct timer{
  static bool ok; // set済みか
  static long long T0;
  // 基準点をセット
  static void set(){
    ok = true;
    T0 = timems();
  }
  // 基準点からの経過時間(ms単位)
  static long long elapse(){
    assert(ok);
    return timems() - T0;
  };
};
template<int id>
bool timer<id>::ok(false);
template<int id>
long long timer<id>::T0(0);
#endif
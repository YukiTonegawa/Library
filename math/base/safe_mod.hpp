#ifndef _SAFE_MOD_H_
#define _SAFE_MOD_H_
// @param m `1 <= m`
constexpr long long safe_mod(long long x, long long m){
  x %= m;
  if (x < 0) x += m;
  return x;
}
#endif
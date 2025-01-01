#ifndef _BARRETT_H_
#define _BARRETT_H_

#include <vector>
#include <algorithm>
#include <cstdint>
#include <algorithm>

struct barrett{
  unsigned int _m;
  unsigned long long im;
  explicit barrett(unsigned int m) : _m(m), im((unsigned long long)(-1) / m + 1){}
  unsigned int umod()const{return _m;}
  unsigned int mul(unsigned int a, unsigned int b)const{
    unsigned long long z = a;
    z *= b;
#ifdef _MSC_VER
    unsigned long long x;
    _umul128(z, im, &x);
#else
    unsigned long long x = (unsigned long long)(((unsigned __int128)(z) * im) >> 64);
#endif
    unsigned long long y = x * _m;
    return (unsigned int)(z - y + (z < y ? _m : 0));
  }
};
#endif
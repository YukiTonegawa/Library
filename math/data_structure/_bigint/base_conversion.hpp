#ifndef _BASE_CONVERSION_H_
#define _BASE_CONVERSION_H_
#include <string>
#include <vector>
#include <cmath>
#include "bigint.hpp"
#include "bigbinary.hpp"

// 16進 -> 2進
std::string hex_to_binary(const std::string &s){
  std::string res = "";
  if(s.empty()) return "";
  bool neg = (s[0] == '-' ? 1 : 0);
  int n = s.size();
  for(int i = n - 1; i >= neg; i--){
    int x = ('0' <= s[i] && s[i] <= '9') ? s[i] - '0' : (s[i] - 'A') + 10;
    for(int j = 0; j < 4; j++) res += ((x >> j) & 1) + '0';
  }
  while(!res.empty() && res.back() == '0') res.pop_back();
  if(res.empty()){
    res = '0';
  }else{
    std::reverse(res.begin(), res.end());
    if(neg) res = '-' + res;
  }
  return res;
}
// 2進 -> 16進
std::string binary_to_hex(const std::string &s){
  std::string res = "";
  if(s.empty()) return "";
  bool neg = (s[0] == '-' ? 1 : 0);
  int n = s.size();
  int j = 0, tmp = 0;
  for(int i = n - 1; i >= neg; i--){
    tmp += ((s[i] == '1') << j);
    if(j == 3){
      res += (tmp < 10 ? tmp + '0' : tmp - 10 + 'A');
      j = 0;
      tmp = 0;
    }else j++;
  }
  if(tmp) res += (tmp < 10 ? tmp + '0' : tmp - 10 + 'A');
  while(!res.empty() && res.back() == '0') res.pop_back();
  if(res.empty()){
    res = '0';
  }else{
    std::reverse(res.begin(), res.end());
    if(neg) res = '-' + res;
  }
  return res;
}
/*
// 10進 -> d進
std::vector<int> decimal_to_arbitrary(const std::string &s, int d){
  //
}
// d進 -> 10進
std::string arbitrary_to_decimal(const std::vector<int> &s, int d){
  //
}
*/
/*
// 2進 -> d進
std::vector<int> binary_to_arbitrary(const std::string &s, int d){
  assert(d >= 2);
  //int n = s.size();
  //int m = std::ceil(n * (1 / std::log2(d))); // d進での桁 log_d(2^n) = n * log_d(2) = n * (1 / log_2(d))
  //auto f = [&]
}
// d進 -> 2進
std::string arbitrary_to_binary(const std::vector<int> &s, int d){
  //
}
*/
#endif
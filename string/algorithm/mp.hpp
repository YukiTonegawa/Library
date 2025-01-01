#ifndef _MP_H_
#define _MP_H_
#include <string>
#include <vector>
#include "../base/stovector.hpp"
// res[i] := s_0, s_1....s_k と s_i-k ...s_i-1, s_iが一致するような最大のk
// ただし, k != i
// res[0] = -1
template<typename T>
std::vector<int> mp_algorithm(const std::vector<T> &s) {
    int n = s.size();
    std::vector<int> res(n + 1);
    res[0] = -1;
    int j = -1;
    for (int i = 0; i < n; i++) {
        while (j >= 0 && s[i] != s[j]) j = res[j];
        res[i + 1] = ++j;
    }
    return res;
}

#endif
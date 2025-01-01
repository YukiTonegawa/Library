#ifndef _RUNLENGTH_H_
#define _RUNLENGTH_H_
#include <vector>
#include <string>
#include "stovector.hpp"

template<typename T>
std::vector<std::pair<T, int>> runlength(const std::vector<T> &v) {
    std::vector<std::pair<T, int>> ret;
    int n = v.size();
    for (int i = 0; i < n; i++) {
        if (ret.empty() || ret.back().first != v[i]) {
            ret.push_back({v[i], 1});
        } else {
            ret.back().second++;
        }
    }
    return ret;
}

std::vector<std::pair<char, int>> runlength(const std::string &S) {
    std::vector<char> s = stovector(S);
    return runlength<char>(s);
}

#endif
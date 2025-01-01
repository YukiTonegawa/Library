#ifndef _STOVECTOR_H_
#define _STOVECTOR_H_
#include <vector>
#include <string>

std::vector<char> stovector(const std::string &s) {
    int n = s.size();
    std::vector<char> res(n);
    for (int i = 0; i < n; i++) res[i] = s[i];
    return res;
}

// cを0としてintに直す
std::vector<int> stovector_int(const std::string &s, char c = 'a') {
    int n = s.size();
    std::vector<int> res(n);
    for (int i = 0; i < n; i++) res[i] = s[i] - c;
    return res;
}

#endif
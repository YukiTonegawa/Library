#ifndef _Z_ALGORITHM_H_
#define _Z_ALGORITHM_H_
#include <vector>
#include <string>
#include "../base/stovector.hpp"

// z[i] := lcp(substr(0...n), substr(i...n))
template<typename T>
std::vector<int> z_algorithm(const std::vector<T>& s) {
    int n = int(s.size());
    if (n == 0) return {};
    std::vector<int> z(n);
    z[0] = 0;
    for (int i = 1, j = 0; i < n; i++) {
        int& k = z[i];
        k = (j + z[j] <= i) ? 0 : std::min(j + z[j] - i, z[i - j]);
        while (i + k < n && s[k] == s[i + k]) k++;
        if (j + z[j] < i + z[i]) j = i;
    }
    z[0] = n;
    return z;
}

std::vector<int> z_algorithm(const std::string& s) {
    return z_algorithm(stovector(s));
}

// res[i] := lcp(s[i以降], pat)
std::vector<int> find_lcp(const std::string &s, const std::string &pat) {
    std::string s2 = pat + '$' + s;
    std::vector<int> res = z_algorithm(s2);
    res.erase(res.begin(), res.begin() + (int)pat.size() + 1);
    return res;
}

#endif
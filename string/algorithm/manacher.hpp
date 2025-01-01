#ifndef _MANACHER_H_
#define _MANACHER_H_
#include <vector>

//S[i]を中心とする最大回文半径
//偶数文字の回文を検出したい場合$を挟む必要がある
template<typename T>
std::vector<int> manacher(const std::vector<T> &s) {
    int n = s.size(), c = 0;
    std::vector<int> rad(n);
    for (int r = 0; r < n; r++) {
        int l = c - (r - c);
        if (r + rad[l] < c + rad[c]) {
            rad[r] = rad[l];
        } else {
            int rr = c + rad[c];
            int rl = r - (rr - r);
            while (rl >= 0 && rr < n && s[rl] == s[rr]) rr++, rl--;
            rad[r] = rr - r;
            c = r;
        }
    }
    return rad;
}

template<typename T>
std::vector<T> insert_dollar(const std::vector<T> &s) {
    int n = s.size();
    std::vector<T> res(n * 2 + 1, '$');
    for (int i = 0; i < n; i++) res[i * 2 + 1] = s[i];
    return res;
}

#endif
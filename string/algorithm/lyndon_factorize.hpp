#ifndef _LYNDON_FACTORIZE_H_
#define _LYNDON_FACTORIZE_H_
#include <vector>
#include <tuple>

// {l, 長さ, 繰り返し}
template<typename T>
std::vector<std::tuple<int, int, int>> lyndon_factorize(const std::vector<T> &S) {
    std::vector<std::tuple<int, int, int>> res;
    int N = S.size(), l = 0;
    while (l < N) {
        int i = l, j = l + 1;
        while (j < N && S[i] <= S[j]) {
            if (S[i] == S[j]) {
                i++, j++;
            } else {
                i = l;
                j++;
            }
        }
        int d = j - i, r = (j - l) / d;
        res.push_back({l, d, r});
        l += d * r;
    }
    return res;
}
#endif
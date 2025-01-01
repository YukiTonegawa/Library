#ifndef _RANKED_ZETA_H_
#define _RANKED_ZETA_H_
#include <vector>
#include <type_traits>
#include <cassert>
#include <array>
#include <algorithm>

template<int mod, int n, std::enable_if_t<(n == 0)>* = nullptr>
std::array<std::array<int, n + 1>, (1 << n)>  ranked_zeta(std::vector<int>::iterator _begin, std::vector<int>::iterator _end) {
    std::array<std::array<int, 1>, 1> res;
    res[0][0] = *_begin;
    return res;
}

template<int mod, int n, std::enable_if_t<(n > 0)>* = nullptr>
std::array<std::array<int, n + 1>, (1 << n)> ranked_zeta(std::vector<int>::iterator _begin, std::vector<int>::iterator _end) {
    static constexpr int m = 1 << n;
    static constexpr int mh = m >> 1;
    assert(_end - _begin == m);
    auto _mid = _begin + mh;
    std::array<std::array<int, n + 1>, m> res;
    {
        auto L = ranked_zeta<mod, n - 1>(_begin, _mid);
        for (int i = 0; i < mh; i++) {
            for (int j = 0; j < n; j++) {
                res[i][j] = L[i][j];
            }
            res[i][n] = 0;
        }
    }
    {
        auto R = ranked_zeta<mod, n - 1>(_mid, _end);
        for (int i = 0; i < mh; i++) {
            for (int j = 0; j < n; j++) {
                res[i + mh][j + 1] = R[i][j];
            }
            res[i + mh][0] = 0;
            for (int j = 0; j < n; j++) {
                res[i + mh][j] += res[i][j];
                if (res[i + mh][j] >= mod) res[i + mh][j] -= mod;
            }
        }
    }
    return res;
}
#endif

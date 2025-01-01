#ifndef _RANKED_MOBIUS_H_
#define _RANKED_MOBIUS_H_
#include <vector>
#include <type_traits>
#include <cassert>
#include <array>
#include <algorithm>

template<int mod, int n, std::enable_if_t<(n == 0)>* = nullptr>
void ranked_mobius(std::array<std::array<int, n + 1>, (1 << n)> &A) {
    return;
}

template<int mod, int n, std::enable_if_t<(n > 0)>* = nullptr>
void ranked_mobius(std::array<std::array<int, n + 1>, (1 << n)> &A) {
    static constexpr int m = 1 << n;
    static constexpr int mh = m >> 1;
    assert(A.size() == m);
    {
        std::array<std::array<int, n>, mh> R;
        for (int i = 0; i < mh; i++) {
            for (int j = 1; j <= n; j++) {
                R[i][j - 1] = A[i + mh][j] - A[i][j];
                if (R[i][j - 1] < 0) R[i][j - 1] += mod;
            }
        }
        ranked_mobius<mod, n - 1>(R);
        for (int i = 0; i < mh; i++) {
            A[i + mh][0] = 0;
            for (int j = 0; j < n; j++) {
                A[i + mh][j + 1] = R[i][j];
            }
        }
    }
    {
        std::array<std::array<int, n>, mh> L;
        for (int i = 0; i < mh; i++) {
            for (int j = 0; j < n; j++) {
                L[i][j] = A[i][j];
            }
        }
        ranked_mobius<mod, n - 1>(L);
        for (int i = 0; i < mh; i++) {
            A[i][n] = 0;
            for (int j = 0; j < n; j++) {
                A[i][j] = L[i][j];
            }
        }
    }
}
#endif

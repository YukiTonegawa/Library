#ifndef _DIGITS_DP_H_
#define _DIGITS_DP_H_
#include <array>
#include "../../data_structure/bit/n_ary_operation.hpp"

template<int base, int N>
struct digits_dp {
    using Db = n_ary_operation<uint64_t, base>;
    std::array<uint64_t, N> L, R;
    digits_dp() {}

    // [L, R]
    void set_range(int i, uint64_t _L, uint64_t _R) {
        assert(_L <= _R);
        L[i] = _L;
        R[i] = _R;
    }
    // s = (0, 1, 2, 3) ^ K
    // Xi += xik * base^k
    // Xiのk桁目が0の時のみ使える
    // エラーなら-1
    int next(int s, int i, int k, int xik) {
        int si = (s >> (2 * i)) & 3;
        auto [Ln, Rn] = choose_range(s, i, k);
        bool gtl = si & 1, ltr = (si >> 1) & 1;
        if (xik < Ln || Rn < xik) return -1;
        int sn = int(gtl | (Ln < xik)) + 2 * int(ltr | (xik < Rn));
        return s - (si << (2 * i)) + (sn << (2 * i));
    }

    // 選べる次の数の範囲[L, R]
    std::pair<uint64_t, uint64_t> choose_range(int s, int i, int k) {
        int si = (s >> (2 * i)) & 3;
        if (si == 3) return {0, base - 1};
        bool gtl = si & 1, ltr = (si >> 1) & 1;
        uint64_t Lik = Db::get(L[i], k), Rik = Db::get(R[i], k);
        return {(gtl ? 0 : Lik), (ltr ? base - 1 : Rik)};
    }
};

#endif
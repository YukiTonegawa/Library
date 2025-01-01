#ifndef _FAST_LOWER_BOUND_H_
#define _FAST_LOWER_BOUND_H_
// バグ
/*
#include <numeric>
#include <vector>
#include <algorithm>

template<typename S>
struct fast_lower_bound {
    static constexpr int K = 16;
    static constexpr S inf = std::numeric_limits<S>::max();
    int lmost;
    std::vector<S> V;
    fast_lower_bound() {}
    fast_lower_bound(const std::vector<S> &_V) {
        int N = _V.size(), M = 0, x = 1;
        N = ((N + K - 1) / K) * K;
        while (M + x < N) {
            M += x;
            x *= K;
        }
        lmost = M;
        V = std::vector<S>(M + N, inf);
        for (int i = 0; i < (int)_V.size(); i++) {
            V[i + M] = _V[i];
        }
        for (int i = std::min(M - 1, (N + M - 2) / K); i >= 0; i--) {
            V[i] = V[i * K + 1];
        }
    }
    
    // x未満の数がいくつあるか (x以上の数が現れる最小インデックス)
    int lower_bound(S x) const {
        if (V.empty() || x <= V[0]) return 0;
        int i = 0;
        while (i < lmost) {
            i = i * K + 1;
            if (V[i + 8] < x) i += 8;
            if (V[i + 4] < x) i += 4;
            if (V[i + 2] < x) i += 2;
            if (V[i + 1] < x) i += 1;
        }
        return i - lmost + 1;
    }
    // x以下の数がいくつあるか (xより大きい数が現れる最小インデックス)
    int upper_bound(S x) const {
        return lower_bound(x + 1); 
    }
};
*/
#endif
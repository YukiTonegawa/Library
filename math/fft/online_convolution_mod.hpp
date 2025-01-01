#ifndef _ONLINE_CONVOLUTION_MOD_H_
#define _ONLINE_CONVOLUTION_MOD_H_
#include "convolution_mod.hpp"
#include <vector>
#include <cassert>

template<typename mint>
struct online_convolution_mod {
    int N = 0;
    std::vector<mint> A, B, C;
    void push(mint Ai, mint Bi) {
        A.push_back(Ai);
        B.push_back(Bi);
        int k = __builtin_ctz(N + 2);
        if ((1 << k) > N) k--;
        C.push_back(0);
        C.push_back(0);
        N++;
        for (int i = 0; i <= k; i++) {
            int r = (1 << (i + 1)) - 1;
            int l = r - (1 << i);
            std::vector<mint> x(A.end() - (1 << i), A.end());
            std::vector<mint> y(B.begin() + l, B.begin() + r);
            auto c = convolution_mod<mint>(x, y);
            for (int j = 0, d = l + N - (1 << i); j < c.size(); j++, d++) {
                C[d] += c[j];
            }
            if (r != N) {
                x = std::vector<mint>(B.end() - (1 << i), B.end());
                y = std::vector<mint>(A.begin() + l, A.begin() + r);
                c = convolution_mod<mint>(x, y);
                for (int j = 0, d = l + N - (1 << i); j < c.size(); j++, d++) {
                    C[d] += c[j];
                }
            }
        }
    }
    
    // [x^i]
    mint operator [](int i) const { 
        assert(0 <= i && i < N);
        return C[i];
    }
};
#endif
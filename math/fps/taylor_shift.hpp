#ifndef _TAYLOR_SHIFT_H_
#define _TAYLOR_SHIFT_H_
#include "fps.hpp"
#include "../modint/combination.hpp"

template<typename FPS>
FPS taylor_shift(FPS f, typename FPS::mint_t c) {
    using mint = typename FPS::mint_t;
    int N = f.size();
    combination_mod<mint>::build(N);
    for (int i = 0; i < N; i++) {
        f[i] *= combination_mod<mint>::fac(i);
    }    
    //FPS g = FPS{0, c}.exp(N);
    FPS g(N);
    mint C = 1;
    for (int i = 0; i < N; i++) {
        g[i] = C * combination_mod<mint>::ifac(i);
        C *= c;
    }
    f = (f.reverse() * g).prefix(N);
    f = f.reverse();
    for (int i = 0; i < N; i++) {
        f[i] *= combination_mod<mint>::ifac(i);
    }
    return f;
}

#endif
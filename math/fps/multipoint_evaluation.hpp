#ifndef _MULTIPOINT_EVAL_H_
#define _MULTIPOINT_EVAL_H_
#include "../base/bit_ceil.hpp"
#include "fps.hpp"
#include <vector>

template<typename FPS>
std::vector<typename FPS::mint_t> multipoint_evaluation(const FPS &f, const std::vector<typename FPS::mint_t> &x) {
    assert(!f.empty());
    int M = x.size(), K = bit_ceil(M);
    std::vector<FPS> t(2 * K - 1, {1});
    for (int i = 0; i < M; i++) t[K - 1 + i] = {-x[i], 1};
    for (int i = K - 2; i >= 0; i--) t[i] = t[i * 2 + 1] * t[i * 2 + 2];    
    t[0] = f % t[0];
    for (int i = 1; i < 2 * K - 1; i++) t[i] = t[(i - 1) / 2] % t[i];
    FPS res(M);
    for (int i = 0; i < M; i++) res[i] = t[K - 1 + i][0];
    return res;
}

template<typename FPS>
std::vector<typename FPS::mint_t> multipoint_evaluation2(const FPS &f, const std::vector<typename FPS::mint_t> &x) {
    assert(!f.empty());
    int M = x.size(), K = bit_ceil(M);
    std::vector<FPS> t(2 * K - 1, {1});
    for (int i = 0; i < M; i++) t[K - 1 + i] = {-x[i], 1};
    for (int i = K - 2; i >= 0; i--) t[i] = t[i * 2 + 1] * t[i * 2 + 2];    
    t[0] = f % t[0];
    for (int i = 1; i < 2 * K - 1; i++) t[i] = t[(i - 1) / 2] % t[i];
    FPS res(M);
    for (int i = 0; i < M; i++) res[i] = t[K - 1 + i][0];
    return res;
}
#endif
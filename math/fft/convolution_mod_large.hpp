#ifndef _CONVOLUTION_MOD_LARGE_H_
#define _CONVOLUTION_MOD_LARGE_H_
#include "butterfly.hpp"

template<typename mint>
std::vector<mint> convolution_mod_large(const std::vector<mint> &a, const std::vector<mint> &b){
    if (a.empty() || b.empty()) return {};
    static constexpr int block_size = 1 << 22; // 998244353
    auto split = [](const std::vector<mint> &_a) {
        std::vector<std::vector<mint>> res;
        int n = _a.size();
        for (int i = 0; i < n; i++) {
            if (i % block_size == 0) res.push_back(std::vector<mint>{});
            res.back().push_back(_a[i]);
        }
        return res;
    };
    auto a2 = split(a);
    auto b2 = split(b);
    int n = (int)a2.size();
    int m = (int)b2.size();
    for (int i = 0; i < n; i++) {
        a2[i].resize(block_size << 1);
        butterfly(a2[i]);
    }
    for (int i = 0; i < m; i++) {
        b2[i].resize(block_size << 1);
        butterfly(b2[i]);
    }
    std::vector<mint> tmp(block_size << 1);
    std::vector<mint> res((n + m) * block_size);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < (block_size << 1); k++) {
                tmp[k] = a2[i][k] * b2[j][k];
            }
            butterfly_inv(tmp);
            int offset = (i + j) * block_size;
            for (int k = 0; k < (block_size << 1); k++) res[offset + k] += tmp[k];
        }
    }
    mint iz = mint(block_size << 1).inv();
    int res_size = (int)a.size() + (int)b.size() - 1;
    res.resize(res_size);
    for (int i = 0; i < res_size; i++) res[i] *= iz;
    return res;
}
#endif
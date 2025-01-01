#ifndef _MULTIPRICATIVE_ORDER_H_
#define _MULTIPRICATIVE_ORDER_H_

#include <vector>
#include "rho.hpp"

// res[i] := v[i]の位数
template<typename mint>
std::vector<long long> multiplicative_order(const std::vector<mint> &v) {
    assert(miller_rabin64(mint::mod()));
    int n = v.size();
    std::vector<long long> res(n, 1);
    for (auto [p, q] : rho::factorize2(mint::mod() - 1)) {
        long long x = mint(p).pow(q).val();
        long long y = (mint::mod() - 1) / x;
        for (int i = 0; i < n; i++) {
            long long z = x;
            for (int j = 0; j < q; j++) {
                z /= p;
                if (v[i].pow(y * z).val() != 1) {
                    res[i] *= z * p;
                    break;
                }
            }
        }
    }
    return res;
}
#endif
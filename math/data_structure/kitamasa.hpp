#ifndef _KITAMASA_H_
#define _KITAMASA_H_
#include <vector>
#include <algorithm>
#include "../../function/semi_ring.hpp"

template<typename semi_ring>
struct kitamasa {
    using Val = typename semi_ring::Val;
    static constexpr auto id_add = semi_ring::id_add;
    static constexpr auto id_mul = semi_ring::id_mul;
    static constexpr auto add = semi_ring::add;
    static constexpr auto mul = semi_ring::mul;

    // A[n] -> A[n + 1], O(s)
    static std::vector<Val> increment(const std::vector<Val> &x, const std::vector<Val> &c) {
        int s = c.size();
        std::vector<Val> res(s, id_add());
        for (int i = 0; i < s; i++) {
            if (i) res[i] = x[i - 1];
            res[i] = add(res[i], mul(x[s - 1], c[s - 1 - i]));
        }
        return res;
    }
    // A[n] -> A[2n], O(s^2)
    static std::vector<Val> shift(const std::vector<Val> &x, const std::vector<Val> &c) {
        int s = c.size();
        std::vector<Val> A = x;
        std::vector<Val> res(s, id_add());
        for (int i = 0; i < s; i++) {
            for (int j = 0; j < s; j++) {
                res[j] = add(res[j], mul(x[i], A[j]));
            }
            if (i != s - 1) A = increment(A, c);
        }
        return res;
    }

    // O(s^2 log(k))
    // A[i] = A[i - 1] * c[0] + A[i - 2] + c[1]...を満たす漸化式のk項目
    std::vector<Val> operator ()(long long k, const std::vector<Val> &c) {
        int s = c.size();
        std::vector<Val> res(s, id_add());
        if (k < s) {
            res[k] = id_mul();
            return res;
        }
        res[0] = id_mul();
        for (int l = 63 - __builtin_clzll(k); l >= 0; l--) {
            res = shift(res, c);
            if ((k >> l) & 1) {
                res = increment(res, c);
            }
        }
        return res;
    }
};


#endif
#ifndef _MATRIX_ABST_CONST_H_
#define _MATRIX_ABST_CONST_H_
#include <vector>
#include <iostream>
#include <cassert>
#include <limits>
#include "../../function/semi_ring.hpp"

// 静的サイズのN * N正方行列
template<int N, typename semi_ring>
struct matrix_abstract_const_size {
    using Val = typename semi_ring::Val;
    static constexpr auto id_add = semi_ring::id_add;
    static constexpr auto id_mul = semi_ring::id_mul;
    static constexpr auto add = semi_ring::add;
    static constexpr auto mul = semi_ring::mul;

    using vec = std::array<Val, N>;
    using mat = std::array<vec, N>;
    using mat_ab = matrix_abstract_const_size<N, semi_ring>;
    mat val;
    static mat_ab _mul(const mat_ab &vl, const mat_ab &vr) {
        mat_ab ret;
        auto vr_t = vr.t();
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    ret[i][j] = add(ret[i][j], mul(vl.val[i][k], vr_t[j][k]));
                }
            }
        }
        return ret;
    }

    matrix_abstract_const_size() { for (int i = 0; i < N; i++) val[i].fill(id_add()); }
    matrix_abstract_const_size(const mat_ab &v): val(v.val) {}

    bool operator == (const mat_ab &vr) const {
        for (int i = 0; i < N; i++) {
            if (val[i] != vr.val[i]) return false;
        }
        return true;
    }

    bool operator != (const mat_ab &vr) const { return !((*this) == vr); }
    mat_ab operator *  (const mat_ab  &vr) { return           _mul(*this, vr); }
    mat_ab operator *= (const mat_ab  &vr) { return (*this) = _mul(*this, vr); }
    vec&    operator [] (const int i) { return val[i]; }

    // n次の単位行列
    static mat_ab eye() {
        mat_ab ret;
        for (int i = 0; i < N; i++) ret[i][i] = id_mul();
        return ret;
    }

    void print() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                std::cout << val[i][j] << (j == N - 1 ? '\n' : ' ');
            }
        }
    }

    mat_ab pow(long long k) {
        mat_ab ret = eye(); // k == 0の場合単位行列を返す
        mat_ab mul_mat(*this);
        while (k) {
            if (k & 1) ret *= mul_mat;
            k >>= 1;
            mul_mat *= mul_mat;
        }
        return ret;
    }

    // 転置
    mat_ab t() const {
        mat_ab ret;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                ret[j][i] = val[i][j];
            }
        }
        return ret;
    }
};
#endif
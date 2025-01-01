#ifndef _FPS_Y_SMALL_H_
#define _FPS_Y_SMALL_H_
#include "fps.hpp"

// 2変数fps
// yが次数K(2, 3程度)までしか必要ない場合
// ntt_friendly専用 
template<typename mint, int dimY>
struct fps_y_small {
    using fps_t = fps<mint>;
    using self_t = fps_y_small<mint, dimY>;
    using mint_t = mint;

    std::array<fps_t, dimY> f;

    fps_y_small(const std::array<fps_t, dimY> &v) : f(v) {}
    fps_y_small(const std::vector<mint> &v) { f[0] = fps_t(v); }
    fps_y_small() {}

    int size(int d) const {
        return f[d].size();
    }

    // operator
    self_t operator - () const { 
        self_t res(*this);
        for (int d = 0; d < dimY; d++) {
            for (int i = 0; i < res[d].size(); i++) {
                res[d][i] = -res[d][i];
            }
        }
        return res;
    }
    self_t operator += (const self_t &B) {
        for (int d = 0; d < dimY; d++) {
            if (f[d].size() < B[d].size()) f[d].resize(B[d].size(), 0);
            for (int i = 0; i < B[d].size(); i++) {
                f[d][i] += B[d][i];
            }
        }
        return *this;
    }
    self_t operator + (const self_t &B) const { self_t res(*this); return res += B; }
    self_t operator -= (const self_t &B) {
        for (int d = 0; d < dimY; d++) {
            if (f[d].size() < B[d].size()) f[d].resize(B[d].size(), 0);
            for (int i = 0; i < B[d].size(); i++) {
                f[d][i] -= B[d][i];
            }
        }
    }
    self_t operator - (const self_t &B) const { self_t res(*this); return res -= B; }
    self_t operator * (const self_t &B) const { 
        self_t res(*this);
        for (int d = dimY - 1; d >= 0; d--) {
            for (int dl = d; dl >= 0; dl--) {
                int dr = d - dl;
                if (dl == d) res[d] = res[dl] * B[dr];
                else res[d] += res[dl] * B[dr];
            }
        }
        return res;
    }
    self_t operator *= (const self_t &B) { return *this = *this * B; }
    self_t operator /= (const self_t &B) { return *this *= B.inv(); }
    self_t operator / (const self_t &B) const { self_t res(*this); return res /= B; }

    /*
    self_t operator %= (const self_t &B) { return *this = division_polynomial(B).second; }
    self_t operator % (const self_t &B) const { fps res(*this); return res %= B; }
    */

    self_t operator += (const mint &B) {
        if (f[0].size() == 0) f[0].push_back(B);
        else f[0][0] += B;
        return *this;
    }
    self_t operator + (const mint &B) const { self_t res(*this); return res += B; }
    self_t operator -= (const mint &B) {
        if (f[0].size() == 0) f[0].push_back(-B);
        else f[0][0] -= B;
        return *this;
    }
    self_t operator - (const mint &B) const { self_t res(*this); return res -= B; }
    self_t operator * (const mint &B) const { self_t res(*this); return res *= B; }
    self_t operator *= (const mint &B) { 
        for (int d = 0; d < dimY; d++) {
            for (int i = 0; i < f[d].size(); i++) {
                f[d][i] *= B;
            }
        }
        return *this;
    }
    self_t operator /= (const mint &B) {
        mint Bi = B.inv(); 
        for (int d = 0; d < dimY; d++) {
            for (int i = 0; i < f[d].size(); i++) {
                f[d][i] *= Bi;
            }
        }
        return *this;
    }
    self_t operator / (const mint &B) const { self_t res(*this); return res /= B; }
    fps_t& operator [] (const int i) { return f[i]; }
};

#endif
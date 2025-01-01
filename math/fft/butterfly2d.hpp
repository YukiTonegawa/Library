#ifndef _BUTTERFLY2D_H_
#define _BUTTERFLY2D_H_
#include "butterfly.hpp"

template<typename mint> 
void butterfly2d(std::vector<std::vector<mint>> &a) {
    int n = a.size();
    int m = a.empty() ? 0 : a[0].size();
    std::vector<std::vector<mint>> b(m, std::vector<mint>(n));
    for (int i = 0; i < n; i++) butterfly(a[i]);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            b[j][i] = a[i][j];
        }
    }
    for (int i = 0; i < m; i++) butterfly(b[i]);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            a[i][j] = b[j][i];
        }
    }
}

template<typename mint> 
void butterfly_inv2d(std::vector<std::vector<mint>> &a) {
    int n = a.size();
    int m = a.empty() ? 0 : a[0].size();
    std::vector<std::vector<mint>> b(m, std::vector<mint>(n));
    for (int i = 0; i < n; i++) { 
        for (int j = 0; j < m; j++) {
            b[j][i] = a[i][j];
        }
    }
    for (int i = 0; i < m; i++) butterfly_inv(b[i]);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            a[i][j] = b[j][i];
        }
    }
    for (int i = 0; i < n; i++) butterfly_inv(a[i]);
}
#endif
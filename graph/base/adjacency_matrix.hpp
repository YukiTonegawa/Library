#ifndef _ADJACENCY_MATRIX_H_
#define _ADJACENCY_MATRIX_H_
#include <array>

// 頂点数が定数
template<typename T, int N>
using adjacency_matrix = std::array<std::array<T, N>, N>;

template<typename T>
struct adjacency_matrix_dynamic {
    adjacency_matrix_dynamic(int _N) {

    }
};
#endif
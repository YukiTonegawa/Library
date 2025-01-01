#ifndef _GMATCHING_NUMBER_H_
#define _GMATCHING_NUMBER_H_
#include "../../math/matrix/matrix_mod.hpp"
#include "../../math/modint/modint32_static.hpp"
#include <random>

// マッチングの数のみ
struct general_matching_tutte {
    int N;
    static constexpr int mod = 999999893;
    using mint = modint32_static<mod>;
    using matrix = matrix_mod<mint>;
    matrix m;
    general_matching_tutte() : general_matching_tutte(0) {}
    general_matching_tutte(int N) : N(N), m(N, N, 0) {}

    void add_edge(int a, int b) {
        static std::random_device seed_gen;
        static std::mt19937_64 engine(seed_gen());
        if (a > b) std::swap(a, b);
        if (a == b) return;
        uint64_t x = engine();
        m[a][b] = x, m[b][a] = x, m[b][a] *= -1;
    }
    
    // 一般マッチングのペアの数の最大値
    int solve() {
        return m.gaussian_elimination().first.rank() / 2;
    }
};
#endif
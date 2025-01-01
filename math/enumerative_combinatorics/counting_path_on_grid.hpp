#ifndef _COUNT_PATH_GRID_H_
#define _COUNT_PATH_GRID_H_
#include <array>
#include "../base/bit_ceil.hpp"
#include "../modint/combination.hpp"
#include "../fft/butterfly.hpp"
#include "../fft/convolution_mod.hpp"

template<typename mint>
struct counting_path_on_grid {
    static std::vector<std::vector<mint>> F; // F[k] = ntt(0! + 1!x + 2!x^2 .... (2^k - 1)!x^(2^k - 1))
    // 畳み込む時に次数が超えるが問題ないはず

    // F[k]を作る
    static void make_f(int k) {
        combination_mod<mint>::build(1 << k);
        F[k].resize(1 << k, 0);
        for (int i = 0; i < (1 << k); i++) {
            F[k][i] = combination_mod<mint>::fac(i);
        }
        butterfly(F[k]);
    }

    static constexpr int naive_lim1 = 100;
    static constexpr int naive_lim2 = 10000;
    static std::vector<mint> solve_naive(int H, int W, const std::vector<mint> &w) {
        if (!H || !W) return {};
        static std::array<std::array<mint, naive_lim1>, naive_lim2> table;
        std::vector<mint> res(H + W - 1);
        assert(w.size() == H + W - 1);
        if (H < W && H <= naive_lim1) {
            for (int i = 0; i < W; i++) table[H - 1][i] = w[H - 1 + i];
            for (int i = H - 1; i >= 0; i--) {
                for (int j = 1; j < W; j++) table[i][j] += table[i][j - 1];
                if (i) {
                    table[i - 1][0] = table[i][0] + w[i - 1];
                    for (int j = 1; j < W; j++) table[i - 1][j] = table[i][j];
                }
            }
            for (int i = 0; i < W; i++) res[i] = table[0][i];
            for (int i = 1; i < H; i++) res[W - 1 + i] = table[i][W - 1];
        } else {
            for (int i = 0; i < H; i++) table[W - 1][i] = w[H - 1 - i];
            for (int i = W - 1; i >= 0; i--) {
                for (int j = 1; j < H; j++) table[i][j] += table[i][j - 1];
                if (i) {
                    table[i - 1][0] = table[i][0] + w[H + W - 1 - i];
                    for (int j = 1; j < H; j++) table[i - 1][j] = table[i][j];
                }
            }
            for (int i = 0; i < W; i++) res[i] = table[W - 1 - i][H - 1];
            for (int i = 1; i < H; i++) res[W - 1 + i] = table[0][H - 1 - i];
        }
        return res;
    }

    // |s/t0| t1 | t2 | t3 |
    // | s1 |    |    | t4 |
    // | s2 |    |    | t5 |
    // | s3 | s4 | s5 |s/t6|
    // s_0 ~ s_h+w-2からt_0 ~ t_h+w-2への重みつき経路数数え上げ
    // (右か上移動だけでs_i -> t_jへ移動できる時に(経路数) * w_iをres[j]に足す)  O((H + W) log(H + W))
    static std::vector<mint> solve(int H, int W, const std::vector<mint> &w) {
        assert(w.size() == H + W - 1);
        std::vector<mint> res(H + W - 1, 0);
        if (std::min(H, W) <= naive_lim1 && std::max(H, W) <= naive_lim2) return solve_naive(H, W, w);
        if (!H || !W) return {};
        combination_mod<mint>::build(H + W);
        int logHW = bit_ceil_log(H + W);
        
        std::vector<mint> A(1 << logHW, 0);
        mint iz = mint(1 << logHW).inv();

        bool use_sita = false;
        for (int i = 1; i < W; i++) {
            if (w[H - 1 + i] != 0) {
                use_sita = true;
                break;
            }
        }

        // 左 -> 上
        {
            for (int i = 0; i < H; i++) {
                A[i] = w[H - 1 - i] * combination_mod<mint>::ifac(H - 1 - i);
            }
            butterfly(A);
            if (F[logHW].empty()) make_f(logHW);
            for(int i = 0; i < (1 << logHW); i++) {
                A[i] *= F[logHW][i];
            }
            butterfly_inv(A);
            for (int i = 0, j = H - 1; i < W; i++, j++) {
                res[i] += A[j] * iz * combination_mod<mint>::ifac(i);
            }
        }

        // 下 -> 右
        if (use_sita) {
            std::fill(A.begin(), A.end(), 0);
            for (int i = 1; i < W; i++) {
                A[i] = w[H - 1 + i] * combination_mod<mint>::ifac(W - 1 - i);
            }
            butterfly(A);
            for(int i = 0; i < (1 << logHW); i++) {
                A[i] *= F[logHW][i];
            }
            butterfly_inv(A);
            for (int i = 0; i < H; i++) {
                res[H + W - 2 - i] += A[W - 1 + i] * iz * combination_mod<mint>::ifac(i);
            }
        }

        // 左 -> 右
        {
            std::vector<mint> B(w.begin(), w.begin() + H);
            std::vector<mint> C(H);
            for (int i = 0; i < H; i++) {
                C[i] = combination_mod<mint>::comb(H + W - 2 - i, W - 1);
            }
            C = convolution_mod<mint>(B, C);
            for (int i = 1; i < H; i++) {
                res[W - 1 + i] += C[H - 1 + i];
            }
        }

        // 下 -> 上
        if (use_sita) {
            std::vector<mint> B(W, 0);
            for (int i = 0; i < W - 1; i++) {
                B[i] = w[H + W - 2 - i];
            }
            std::vector<mint> C(W);
            for (int i = 0; i < W; i++) {
                C[i] = combination_mod<mint>::comb(H + W - 2 - i, H - 1);
            }
            C = convolution_mod<mint>(B, C);
            for (int i = 1; i < W; i++) {
                res[W - 1 - i] += C[W - 1 + i];
            }
        }
        return res;
    }
};
template<typename mint>
std::vector<std::vector<mint>> counting_path_on_grid<mint>::F(25);

#endif
#ifndef _FALLING_FACTORIAL_H_
#define _FALLING_FACTORIAL_H_
#include "../fps/fps.hpp"
#include "../modint/combination.hpp"

// 下降冪
// https://mathlog.info/articles/3351
// a0 + a1x + a2x(x-1) + .... anx(x-1)...(x-n+1)
template<typename mint>
struct falling_factorial : std::vector<mint> {
    using std::vector<mint>::vector;
    using self_t = falling_factorial<mint>;

    falling_factorial(const std::vector<mint> &v) : std::vector<mint>::vector(v) {}

    int size() const {
        return this->std::vector<mint>::size();
    }

    // f(0), f(1), .... f(N) からN次のfalling factorialを復元
    static self_t interpolation(const std::vector<mint> &y) {
        assert(!y.empty());
        int N = int(y.size()) - 1;
        combination_mod<mint>::build(N);
        std::vector<mint> X(N + 1), Y(N + 1);
        for (int i = 0; i <= N; i++) {
            X[i] = y[i] * combination_mod<mint>::ifac(i);
            Y[i] = (i % 2 == 0 ? 1 : -1) * combination_mod<mint>::ifac(i);
        }
        X = convolution_fft<mint>(X, Y);
        X.resize(N + 1);
        return self_t(X);
    }

    // f(0), f(1), .... f(K)を得る
    std::vector<mint> multipoint_evaluation(int K) const {
        combination_mod<mint>::build(K);
        std::vector<mint> y(K + 1);
        for (int i = 0; i <= K; i++) y[i] = combination_mod<mint>::ifac(i);
        auto res = convolution_fft<mint>(*this, y);
        res.resize(K + 1);
        for (int i = 0; i <= K; i++) res[i] *= combination_mod<mint>::fac(i);
        return res;
    }
    
    // f(x + c)
    self_t shift(mint c) const {
        int N = size() - 1;
        combination_mod<mint>::build(N);
        std::vector<mint> X(N + 1), Y(N + 1);
        mint C = 1;
        for (int i = 0; i <= N; i++) {
            X[i] = (*this)[i] * combination_mod<mint>::fac(i);
            Y[N - i] = C * combination_mod<mint>::ifac(i);
            C *= c - i;
        }
        X = convolution_fft<mint>(X, Y);
        self_t res(N + 1);
        for (int i = 0; i <= N; i++) {
            res[i] = X[N + i] * combination_mod<mint>::ifac(i);
        }
        return res;
    }
};

#endif
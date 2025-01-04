#ifndef _ENCODER_H_
#define _ENCODER_H_
#include <vector>
#include <array>

template<typename T, T... Tail>
struct encoder {
    static constexpr int len = sizeof...(Tail);
    static constexpr std::array<T, len> V = {Tail...};
    static constexpr std::array<T, len> Vprod = [](){
        std::array<T, len> res;
        for (int i = len - 1; i >= 0; i--) {
            if (i + 1 == len) {
                res[i] = 1;
            } else {
                res[i] = res[i + 1] * V[i + 1];
            }
        }
        return res;
    }();
    static T encode(decltype(Tail)... args) {
        std::array<T, len> v = {args...};
        T res = 0;
        for (int i = 0; i < len; i++) {
            assert(0 <= v[i] && v[i] < V[i]);
            res += v[i] * Vprod[i];
        }
        return res;
    }

    static std::array<T, len> decode(T x) {
        std::array<T, len> res;
        for (int i = 0; i < len; i++) {
            T q = x / Vprod[i];
            assert(0 <= q && q < V[i]);
            res[i] = q;
            x -= q * Vprod[i];
        }
        return res;
    }
};
#endif
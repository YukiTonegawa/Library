#ifndef _FUNCTION_H_
#define _FUNCTION_H_
#include <algorithm>
#include <numeric>

namespace func {
    template<typename T>
    static constexpr T add(const T &a, const T &b) { return a + b; }
    template<typename T>
    static constexpr T sub(const T &a, const T &b) { return a - b; }
    template<typename T>
    static constexpr T mul(const T &a, const T &b) { return a * b; }
    template<typename T>
    static constexpr T div(const T &a, const T &b) { return a / b; }

    template<typename T>
    static constexpr bool less(const T &a, const T &b) { return a < b; }
    template<typename T>
    static constexpr bool less_equal(const T &a, const T &b) { return a <= b; }
    template<typename T>
    static constexpr bool greater(const T &a, const T &b) { return a > b; }
    template<typename T>
    static constexpr bool greater_equal(const T &a, const T &b) { return a >= b; }

    template<typename T>
    static constexpr T min(const T &a, const T &b) { return std::min(a, b); }
    template<typename T>
    static constexpr T max(const T &a, const T &b) { return std::max(a, b); }
    template<typename T>
    static constexpr bool chmin(T &a, const T &b) { return (a > b ? a = b : false); }
    template<typename T>
    static constexpr bool chmin_equal(T &a, const T &b) { return (a >= b ? a = b : false); }
    template<typename T>
    static constexpr bool chmax(T &a, const T &b) { return (a < b ? a = b : false); }
    template<typename T>
    static constexpr bool chmax_equal(T &a, const T &b) { return (a <= b ? a = b : false); }

    template<typename T>
    static constexpr T get_inf() { return std::numeric_limits<T>::max(); }
    template<typename T>
    static constexpr T get_minf() { return std::numeric_limits<T>::min(); }
};
#endif
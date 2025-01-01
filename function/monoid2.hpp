#ifndef _MONOID2_H_
#define _MONOID2_H_
#include <algorithm>
#include "function.hpp"
#include "../math/base/gcd.hpp"
#include "clamp_function.hpp"

template<typename monoid>
struct reversible_monoid {
    using s = typename monoid::S;
    using S = std::pair<s, s>;
    static constexpr S e() { return {monoid::e(), monoid::e()}; }
    static constexpr S op(S a, S b) { return {monoid::op(a.first, b.first), monoid::op(b.second, a.second)}; }
    static constexpr S flip(S a) { return {a.second, a.first}; }
};
template<typename T>
struct range_gcd {
    using S = T;
    static constexpr S e() { return 0; }
    static constexpr S op(S a, S b) { return gcd(a, b); }
};
// オーバーフローする値をinfで表現
template<typename T>
struct range_lcm {
    using S = T;
    static constexpr S inf = func::get_inf<S>();
    static constexpr S e() { return 1; }
    static constexpr S op(S a, S b) {
        if (a == inf || b == inf) return inf;
        S g = gcd(a, b);
        if (__builtin_mul_overflow(a / g, b, &a)) return inf;
        return a;
    }
};

// f(x) = min(max(x + a, b), c)
template<typename T>
struct range_clamp {
    using S = clamp_function<T>;
    static constexpr S e() { return S(); }
    // g(f(x))
    static constexpr S op(S f, S g) { return S::merge(f, g); }
};
// f(x) = min(max(x + a, b), c), minで減少した値をスコアとする
template<typename T, typename Tsum>
struct range_clamp_score {
    using S = clamp_function_score<T, Tsum>;
    static constexpr S e() { return S(); }
    static constexpr S op(S a, S b) { return S::merge(a, b); }
};

// min(a, max(b, x))の形のclamp関数
// 区間作用, 区間積
template<typename T>
struct range_clamp_apply_range_clamp_prod {
    using S = clamp_function<T>;
    using F = S;
    static constexpr S e() { return S(); }
    static constexpr F id() { return {1, 0}; }
    // g(f(x))
    static constexpr S op(S f, S g) { return S::merge(f, g); }
    static constexpr S mapping(F a, S b, int c) { return S::merge(b, a); }
    static constexpr F composition(F a, F b){return F::merge(b, a);}
};
/*
struct range_excess_value{
  using Val = excess_value;
  static Val id(){return Val();}
  static Val merge(Val a, Val b){return Val::merge(a, b);}
};
*/
template<typename T>
struct range_prefixsum_min {
    using S = prefixsum_min<T>;
    static constexpr S e() { return S::id(); }
    static constexpr S op(S a, S b) { return S::merge(a, b);}
};
template<typename T>
struct range_prefixsum_max {
    using S = prefixsum_max<T>;
    static constexpr S e() { return S::id(); }
    static constexpr S op(S a, S b) { return S::merge(a, b);}
};
template<typename T>
struct range_substringsum_max {
    using S = substringsum_max<T>;
    static constexpr S e() { return S::id(); }
    static constexpr S op(S a, S b) { return S::merge(a, b);}
};
#endif
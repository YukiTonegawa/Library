#ifndef _ADD_CHMIN_CHMAX_H_
#define _ADD_CHMIN_CHMAX_H_
#include <numeric>
#include <algorithm>

template<typename T, typename Tsum>
struct add_chmin_chmax {
    static constexpr T inf = std::numeric_limits<T>::max();
    static constexpr T minf = std::numeric_limits<T>::min();

    struct S {
        T min, second_min, max, second_max;
        Tsum sum;
        int min_cnt, max_cnt;
        S() : min(inf), second_min(inf), max(minf), second_max(minf), sum(0), min_cnt(0), max_cnt(0) {}
        S(T x) : min(x), second_min(inf), max(x), second_max(minf), sum(x), min_cnt(1), max_cnt(1) {}
    };

    struct F { 
        T add, lower, upper;
        F() : add(0), lower(minf), upper(inf) {}
        F(T a, T b, T c) : add(a), lower(b), upper(c) {}
    };

    static constexpr void op(S &v, const S &l, const S &r) {
        v.sum = l.sum + r.sum;
        if (l.max == r.max) {
            v.max = l.max;
            v.max_cnt = l.max_cnt + r.max_cnt;
            v.second_max = std::max(l.second_max, r.second_max);
        } else if (l.max > r.max) {
            v.max = l.max;
            v.max_cnt = l.max_cnt;
            v.second_max = std::max(l.second_max, r.max);
        } else {
            v.max = r.max;
            v.max_cnt = r.max_cnt;
            v.second_max = std::max(l.max, r.second_max);
        }
        if (l.min == r.min) {
            v.min = l.min;
            v.min_cnt = l.min_cnt + r.min_cnt;
            v.second_min = std::min(l.second_min, r.second_min);
        } else if (l.min < r.min) {
            v.min = l.min;
            v.min_cnt = l.min_cnt;
            v.second_min = std::min(l.second_min, r.min);
        } else {
            v.min = r.min;
            v.min_cnt = r.min_cnt;
            v.second_min = std::min(l.min, r.second_min);
        }
    }

    static constexpr S e() {
        return S();
    }

    static constexpr void mapping(const F &f, S &s, int len) {
        if (f.add) {
            s.min += f.add;
            if (s.second_min != inf) s.second_min += f.add;
            s.max += f.add;
            if (s.second_max != minf) s.second_max += f.add;
            s.sum += f.add * len;
        }
        if (s.min < f.lower) {
            s.sum += Tsum(f.lower - s.min) * s.min_cnt;
            if (s.second_max == s.min) s.second_max = f.lower;
            else if (s.max == s.min) s.max = f.lower, s.second_max = minf;
            s.min = f.lower;
        }
        if (f.upper < s.max) {
            s.sum -= Tsum(s.max - f.upper) * s.max_cnt;
            if (s.second_min == s.max) s.second_min = f.upper;
            else if (s.min == s.max) s.min = f.upper, s.second_min = inf;
            s.max = f.upper;
        }
    }

    static constexpr void composition(const F &f, F &a) {
        if (f.add) {
            a.add += f.add;
            if (a.lower != minf) a.lower += f.add;
            if (a.upper != inf) a.upper += f.add;
        }
        if (a.upper <= f.lower) {
            a.lower = a.upper = f.lower;
        } else if(f.upper <= a.lower) {
            a.lower = a.upper = f.upper;
        } else {
            a.lower = std::max(a.lower, f.lower);
            a.upper = std::min(a.upper, f.upper);
        }
    }

    static constexpr bool break_add(const S &a, const F &f) {
        return false;
    }

    static constexpr bool tag_add(const S &a, const F &f) {
        return true;
    }

    static constexpr bool break_chmin(const S &a, const F &f) {
        return a.max <= f.upper;
    }

    static constexpr bool tag_chmin(const S &a, const F &f) {
        return a.second_max < f.upper;
    }

    static constexpr bool break_chmax(const S &a, const F &f) {
        return a.min >= f.lower;
    }

    static constexpr bool tag_chmax(const S &a, const F &f) {
        return a.second_min > f.lower;
    }

    static constexpr F transform(const S &a, const F &f) {
        return f;
    }
};

#endif
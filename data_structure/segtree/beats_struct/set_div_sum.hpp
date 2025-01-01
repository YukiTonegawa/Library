#ifndef _SET_DIV_SUM_H_
#define _SET_DIV_SUM_H_
#include <numeric>
#include <algorithm>

template<typename T>
struct set_div_sum {
    static constexpr T minf = std::numeric_limits<T>::min();

    struct S {
        T sum, unique;
        S() : sum(0), unique(minf) {}
        S(T x) : sum(x), unique(x) {}
    };
    
    struct F {
        T setx;
        F() : setx(minf) {}
        F(T x) : setx(x) {}
    };

    static constexpr void op(S &v, const S &l, const S &r) {
        v.sum = l.sum + r.sum;
        if (l.unique == r.unique) v.unique = l.unique;
        else v.unique = minf;
    }

    static constexpr S e() {
        return S();
    }

    static constexpr void mapping(const F &f, S &s, int len) {
        s.sum = f.setx * len;
        s.unique = f.setx;
    }

    static constexpr void composition(const F &f, F &a) {
        a = f;
    }

    static constexpr bool break_set(const S &a, const F &f) {
        return false;
    }

    static constexpr bool tag_set(const S &a, const F &f) {
        return true;
    }

    static constexpr F transform_set(const S &a, const F &f) {
        return f;
    }

    static constexpr bool break_div(const S &a, const F &f) {
        return a.unique == 0;
    }

    static constexpr bool tag_div(const S &a, const F &f) {
        return a.unique != minf;
    }
    
    static constexpr F transform_div(const S &a, const F &f) {
        assert(f.setx != 0);
        return F(a.unique / f.setx);
    }
};

#endif
#ifndef _ZETA_H_
#define _ZETA_H_
#include <vector>
#include <limits>
#include <cassert>
#include "../base/bit_ceil.hpp"

namespace zeta {

template<typename T>
T _add_func(const T a, const T b) { return a + b; }
template<typename T>
T _sub_func(const T a, const T b) { return a - b; }
template<typename T>
T _zero_func() { return 0; }
template<typename T>
T _minus_func(const T x) { return -x; }


// z[S] = f(v[T], T ⊂ S)   O(N * 2^N)
template<typename T, T (*f)(T, T), T (*e)()>
std::vector<T> zeta_subset(std::vector<T> v) {
    if (v.empty()) return {};
    int m = bit_ceil(v.size());
    v.resize(m, e());
    for (int i = 1; i < m; i <<= 1) {
        for (int j = 0; j < m; j += i << 1) {
            for (int k = j; k < (j | i); k++) {
                v[k | i] = f(v[k | i], v[k]);
            }
        }
    }
    return v;
}

// z[S] = sum(v[T], T⊂S)
template<typename T>
std::vector<T> zeta_subset_sum(const std::vector<T> &v) {
    return zeta_subset<T, _add_func<T>, _zero_func<T>>(v);
}

// 包除原理(sumの逆変換)
template<typename T>
std::vector<T> mobius_subset_sum(const std::vector<T> &v) {
    return zeta_subset<T, _sub_func<T>, _zero_func<T>>(v);
}

// z[S] = max(v[T], T⊂S)
template<typename T>
std::vector<T> zeta_subset_max(const std::vector<T> &v) {
    return zeta_subset<T, std::max<T>, std::numeric_limits<T>::min>(v);
}

}

namespace zeta {
// z[S] = f(v[T], T⊃S)    O(N * 2^N)
template<typename T, T (*f)(T, T), T (*e)()>
std::vector<T> zeta_superset(std::vector<T> v) {
    if (v.empty()) return {};
    int m = bit_ceil(v.size());
    v.resize(m, e());
    for (int i = 1; i < m; i <<= 1) {
        for (int j = 0; j < m; j += i << 1) {
            for (int k = j; k < (j | i); k++) {
                v[k] = f(v[k], v[k | i]);
            }
        }
    }
    return v;
}

// z[S] = sum(v[T], T⊃S)
template<typename T>
std::vector<T> zeta_superset_sum(const std::vector<T> &v) {
    return zeta_superset<T, _add_func<T>, _zero_func<T>>(v);
}

// 包除原理(sumの逆変換)
template<typename T>
std::vector<T> mobius_superset_sum(const std::vector<T> &v) {
    return zeta_superset<T, _sub_func<T>, _zero_func<T>>(v);
}

// z[S] = max(v[T], T⊃S)
template<typename T>
std::vector<T> zeta_superset_max(const std::vector<T> &v) {
    return zeta_superset<T, std::max<T>, std::numeric_limits<T>::min>(v);
}
}

namespace zeta {
// z[S] = f(v[T], (T&S)!=φ)    O(N * 2^N)
// できる演算が限定されるかも？ sum, max, minはできそう
// 最初の部分で
// v[111] = 5 ->
// A[111] = 5
// A[011] = A[101] = A[110] = -5
// A[100] = A[010] = A[001] = 5としたくなるが
// 5 + inv(5) + inv(5) + inv(5) + 5 + 5 + 5 = 5 = v[111]が成り立つ
// maxも逆元は定義できないが inv(任意の数) = 0 として
// max({5, inv(5), inv(5), inv(5), 5, 5, 5}) = 5 = v[111]が成り立つためできるはず
// つまり f(Tの立っているbitが奇数 ? A[T] : inv(A[T]), T⊂S) = v[S]ならできそう？

template<typename T, T (*f)(T, T), T (*e)(), T (*inv)(T)>
std::vector<T> zeta_intersect(std::vector<T> v) {
    if (v.empty()) return {};
    int m = bit_ceil(v.size());
    v.resize(m, e());
    for (int i = 1; i < m; i <<= 1) {
        for (int j = 0; j < m; j += i << 1) {
            for (int k = j; k < (j | i); k++) {
                v[k] = f(v[k], v[k | i]);
            }
        }
    }

    for(int j = 0; j < m; j++) {
        if (__builtin_popcount(j) % 2 == 0) {
            v[j] = inv(v[j]);
        }
    }

    v[0] = e();
    for (int i = 1; i < m; i <<= 1) {
        for (int j = 0; j < m; j += i << 1) {
            for (int k = j; k < (j | i); k++) {
                v[k | i] = f(v[k | i], v[k]);
            }
        }
    }
    return v;
}

// z[S] = sum(v[T], (T&S)!=φ)
template<typename T>
std::vector<T> zeta_intersect_sum(const std::vector<T> &v) {
    return zeta_intersect<T, _add_func<T>, _zero_func<T>, _minus_func<T>>(v);
}
    
// z[S] = max(v[T], (T&S)!=φ)
template<typename T>
std::vector<T> zeta_intersect_max(const std::vector<T> &v) {
    return zeta_intersect<T, std::max<T>, std::numeric_limits<T>::min, std::numeric_limits<T>::min>(v);
}

// z[S] = min(v[T], (T&S)!=φ)
template<typename T>
std::vector<T> zeta_intersect_min(const std::vector<T> &v) {
    return zeta_intersect<T, std::min<T>, std::numeric_limits<T>::max, std::numeric_limits<T>::max>(v);
}
}

namespace zeta {
// z[S] = f(v[T], (T&S)==φ)    O(N * 2^N)
// Z[S] = f(v[T], T⊂(~S))

template<typename T, T (*f)(T, T), T (*e)()>
std::vector<T> zeta_disjoint(const std::vector<T> &v) {
    auto z = zeta_subset<T, f, e>(v);
    if (z.empty()) return {};
    int N = z.size(); 
    for (int i = 0; i < N / 2; i++) {
        std::swap(z[i], z[(N - 1) ^ i]);
    }
    return z;
}

template<typename T, T (*f)(T, T), T (*e)()>
std::vector<T> mobius_disjoint(std::vector<T> v) {
    if (v.empty()) return {};
    int N = bit_ceil(v.size());
    v.resize(N, e());
    for (int i = 0; i < N / 2; i++) {
        std::swap(v[i], v[(N - 1) ^ i]);
    }
    return zeta_subset<T, f, e>(v);
}

// z[S] = sum(v[T], (T&S)==φ)
template<typename T>
std::vector<T> zeta_disjoint_sum(const std::vector<T> &v) {
    return zeta_disjoint<T, _add_func<T>, _zero_func<T>>(v);
}

// sumの逆変換
template<typename T>
std::vector<T> mobius_disjoint_sum(const std::vector<T> &v) {
    return zeta_disjoint<T, _sub_func<T>, _zero_func<T>>(v);
}

// z[S] = max(v[T], (T&S)==φ)
template<typename T>
std::vector<T> zeta_disjoint_max(const std::vector<T> &v) {
    return zeta_disjoint<T, std::max<T>, std::numeric_limits<T>::min>(v);
}
}

namespace zeta {
//1-indexed, z[i] = f(v[j], 1<=i<=j<=n and j%i==0) O(NlogN)
template<typename T, T (*f)(T, T)>
std::vector<T> zeta_multiple(std::vector<T> v) {
    int N = v.size();
    for (int i = 1; i <= N; i++) {
        for(int j = 2 * i; j <= N; j += i) {
            v[i - 1] = f(v[i - 1], v[j - 1]);
        }
    }
    return v;
}
template<typename T, T (*f)(T, T)>
std::vector<T> mobius_multiple(std::vector<T> v){
    int N = v.size();
    for (int i = N; i >= 1; i--) {
        for (int j = 2 * i; j <= N; j += i) {
            v[i - 1] = f(v[i - 1], v[j - 1]);
        }
    }
    return v;
}
}

namespace zeta {
//1-indexed, z[i] = f(v[j], 1<=j<=i<=n and i%j==0) O(NlogN)
template<typename T, T (*f)(T, T)>
std::vector<T> zeta_divisor(std::vector<T> v) {
    int N = v.size();
    for (int i = N; i >= 1; i--) {
        for (int j = 2 * i; j <= N; j += i) {
            v[j - 1] = f(v[j - 1], v[i - 1]);
        }
    }
    return v;
}

template<typename T, T (*f)(T, T)>
std::vector<T> mobius_divisor(std::vector<T> v) {
    int N = v.size();
    for (int i = 1; i <= N; i++) {
        for(int j = 2 * i; j <= N; j += i) {
            v[j - 1] = f(v[j - 1], v[i - 1]);
        }
    }
    return v;
}
}
using namespace zeta;
#endif
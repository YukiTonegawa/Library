#ifndef _FLOOR_SUM_H_
#define _FLOOR_SUM_H_
#include <cassert>

// ∑{i, 0 <= i < n} floor((ai + b) / c)
// n, a, b, c <= 10^9
long long floor_sum(long long n, long long a, long long b, long long c) {
    long long ans = 0;
    if (a >= c) {
        ans += (n - 1) * n / 2 * (a / c);
        a %= c;
    }
    if (b >= c) {
        ans += n * (b / c);
        b %= c;
    }
    long long y_max = a * n + b;
    if (y_max >= c) ans += floor_sum(y_max / c, c, y_max % c, a);
    return ans;
}

// ∑{i, 0 <= i < n} ceil((ai + b) / c)
// n, a, b, c <= 10^9
long long ceil_sum(long long n, long long a, long long b, long long c) {
    return floor_sum(n, a, b + c - 1, c);
}


template<typename T>
struct fs_struct {
    T f, g, h;
    fs_struct(T _f = 0, T _g = 0, T _h = 0): f(_f), g(_g), h(_h){}
};

template<typename T>
fs_struct<T> fs_query(T n, long long a, long long b, long long c) {
    if (!n) return {0, 0, 0};
    long long ac = a / c, bc = b / c;
    T nc2 = n * (n - 1) / 2;
    if (!a) {
        return {(T)bc * n, bc * nc2, (T)bc * n * bc};
    }
    T f, g, h;
    fs_struct<T> next;
    if (a >= c || b >= c) {
        T sqsum = nc2 * (2 * n - 1) / 3;
        next = fs_query<T>(n, a % c, b % c, c);
        f = next.f + nc2 * ac + n * bc;
        g = next.g + ac * sqsum + bc * nc2;
        h = next.h + ac * (2 * next.g + ac * sqsum) + bc * (2 * next.f + (T)bc * n + (T)2 * ac * nc2);
        return {f, g, h};
    }
    long long m = (a * (n - 1) + b) / c;
    next = fs_query<T>(m, c, c - b - 1, a);
    f = (T)m * (n - 1) - next.f;
    g = ((T)m * n * (n - 1) - next.h - next.f) / 2;
    h = (T)(n - 1) * m * (m + 1) - 2 * next.g - 2 * next.f - f;
    return {f, g, h};
}

// ∑{i, 0 <= i < n} i * floor((ai + b) / c)
// n, a, b, c <= 10^9
template<typename T>
T floor_sum_slope(long long n, long long a, long long b, long long c) {
    assert(0 <= n && 0 <= a && 0 <= b && 0 < c);
    return fs_query<T>(n, a, b, c).g;
}
// ∑{i, 0 <= i < n} floor((ai + b) / c) ^ 2
// n, a, b, c <= 10^9
template<typename T>
T floor_sum_square(long long n, long long a, long long b, long long c) {
    assert(0 <= n && 0 <= a && 0 <= b && 0 < c);
    return fs_query<T>(n, a, b, c).h;
}

#endif
#ifndef _QUOTIENTS_H_
#define _QUOTIENTS_H_
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

template<typename T>
struct quotients_floor {
    // floor(x / x以下の自然数)としてあり得る値の昇順
    static std::vector<T> enumerate(T x) {
        assert(x >= 0);
        if (x == 0) return {};
        T sq = sqrtl(x);
        std::vector<T> ans(sq);
        std::iota(ans.begin(), ans.end(), 1);
        if (x / sq == sq) sq--;
        for (T i = sq; i >= 1; i--) ans.push_back(x / i);
        return ans;
    }

    // floor(x / y(x以下の自然数))としてあり得る値の昇順
    // {商, yの最小値, yの最大値 + 1}
    static std::vector<std::tuple<T, T, T>> enumerate_range(T x) {
        assert(x >= 0);
        if (x == 0) return {};
        T sq = sqrtl(x);
        std::vector<std::tuple<T, T, T>> ans(sq);
        for (T i = 1, pre1 = x; i <= sq; i++){
            T pre2 = x / (i + 1);
            ans[i - 1] = {i, pre2 + 1, pre1 + 1};
            std::swap(pre1, pre2);
        }
        if (x / sq == sq) sq--;
        for (T i = sq; i >= 1; i--) ans.push_back({x / i, i, i + 1});
        return ans;
    }

    // floor(x / x以下の自然数)としてあり得る値の昇順
    // ↑に対してfを適用
    template<typename F>
    static void apply(T x, F f) {
        assert(x >= 0);
        if (x == 0) return;
        T sq = sqrtl(x);
        for (T i = 1; i <= sq; i++) f(i);
        if (x / sq == sq) sq--;
        for (T i = sq; i >= 1; i--) f(x / i);
    }

    // floor(x / y(x以下の自然数))としてあり得る値の昇順
    // {商, yの最小値, yの最大値 + 1}
    // ↑に対してfを適用
    template<typename F>
    static void apply_range(T x, F f) {
        assert(x >= 0);
        if (x == 0) return;
        T sq = sqrtl(x);
        for (T i = 1, pre1 = x; i <= sq; i++) {
            T pre2 = x / (i + 1);
            f(i, pre2 + 1, pre1 + 1);
            std::swap(pre1, pre2);
        }
        if (x / sq == sq) sq--;
        for (T i = sq; i >= 1; i--) f(x / i, i, i + 1);
    }

    // floor(x / x以下の自然数)として小さい方からk番目の要素
    // ない場合は-1
    static T kth(T x, T k) {
        assert(x >= 0);
        if (x == 0) return -1;
        T sq = sqrtl(x);
        if (k < sq) return k + 1;
        T len = 2 * sq - (x / sq == sq);
        if (len <= k) return -1;
        return x / (len - k);
    }

    // floor(x / x以下の自然数)としてあり得る数の種類
    static T num(T x) {
        assert(x >= 0);
        if (x == 0) return 0;
        T sq = sqrtl(x);
        return 2 * sq - (x / sq == sq);
    }

    // floor(x / y(x以下の自然数))として小さい方からk番目の要素
    // {商, yの最小値, yの最大値 + 1}
    // ない場合は-1
    static std::tuple<T, T, T> kth_range(T x, T k) {
        assert(x >= 0);
        if (x == 0) return -1;
        T sq = sqrtl(x);
        if (k < sq) return {k + 1, x / (k + 2) + 1, x / (k + 1) + 1};
        T len = 2 * sq - (x / sq == sq);
        if (len <= k) return {-1, -1, -1};
        len -= k;
        return {x / len, len, len + 1};
    }

    // enumerate(x)を呼んだ時にyは何番目の要素に含まれるか
    // 含まれない場合は-1
    static T index(T x, T y) {
        if (y <= 0 || y > x) return -1;
        T z = x / y;
        if (z <= y) return z;
        T sq = sqrtl(x);
        if (x / sq == sq) return 2 * sq - 1 - y; // 2sq - 1型
        return 2 * sq - y; // 2sq型
    }

    // floor(x / z(x以下の自然数)) = yとなるzの範囲[lz, rz)
    // ない場合は{-1, -1}
    static std::pair<T, T> find_range(T x, T y) {
        T idx = index(x, y);
        if (idx == -1) return {-1, -1};
        auto [a, b, c] = kth_range(x, idx);
        assert(a == y);
        return {b, c};
    }

    // 2変数enumerate
    // (x / k, y / k)ごとに{lk, rk}を列挙
    // {x / k, y / k} = {0, 0}となるようなものは列挙しない(k > max(x, y))
    static std::vector<std::tuple<T, T>> enumerate_pair(T x, T y) {
        if (x < y) std::swap(x, y);
        auto X = enumerate_range(x);
        auto Y = enumerate_range(y);
        X.push_back({-1, 0, 0});
        Y.push_back({-1, 0, 0});
        if (x != y) Y.insert(Y.begin(), {-1, y + 1, -1});
        int r = x + 1;
        std::vector<std::tuple<T, T>> res;
        int i = 0, j = 0;
        while (i + 1 < X.size() || j + 1 < Y.size()) {
            auto [_, xl, xr] = X[i];
            auto [__, yl, yr] = Y[j];
            if (xl > yl) {
                res.push_back({xl, r});
                r = xl;
                i++;
            } else if(xl < yl) {
                res.push_back({yl, r});
                r = yl;
                j++;
            } else {
                res.push_back({xl, r});
                r = xl;
                i++, j++;
            }
        }
        return res;
    }
};

template<typename T>
struct quotients_ceil {
    // ceil(x / x以下の自然数)としてあり得る値の昇順
    static std::vector<T> enumerate(T x) {
        assert(x >= 0);
        if (x == 0) return {};
        T sq = sqrtl(x);
        T y = (x + sq) / (sq + 1);
        std::vector<T> ans(y); // [1...ceil(x / (sq + 1))]を含む
        std::iota(ans.begin(), ans.end(), 1);
        if ((x + sq - 1) / sq == y) sq--;
        for (T i = sq; i >= 1; i--) ans.push_back((x + i - 1) / i); // [1...sq-1]で割った答えは全て異なる
        return ans;
    }

    // ceil(x / y(x以下の自然数))としてあり得る値の昇順
    // {商, yの最小値, yの最大値 + 1}
    static std::vector<std::tuple<T, T, T>> enumerate_range(T x) {
        assert(x >= 0);
        if (x == 0) return {};
        T sq = sqrtl(x);
        T y = (x + sq) / (sq + 1);
        std::vector<std::tuple<T, T, T>> ans(y);
        for (T i = 1, pre1 = x + 1, pre2; i <= y; i++){
            pre2 = x / i;
            if (pre2 * i != x) assert(pre1 != ++pre2);
            ans[i - 1] = {i, pre2, pre1};
            pre1 = pre2;
        }
        if ((x + sq - 1) / sq == y) sq--;
        for (T i = sq; i >= 1; i--) ans.push_back({(x + i - 1) / i, i, i + 1}); // [1...sq-1]で割った答えは全て異なる
        return ans;
    }

    // ceil(x / x以下の自然数)としてあり得る値の昇順
    // ↑に対してfを適用
    template<typename F>
    static void apply(T x, F f) {
        assert(x >= 0);
        if (x == 0) return;
        T sq = sqrtl(x);
        T y = (x + sq) / (sq + 1);
        for (T i = 1; i <= y; i++) f(i);
        if ((x + sq - 1) / sq == y) sq--;
        for (T i = sq; i >= 1; i--) f((x + i - 1) / i);
    }

    // ceil(x / y(x以下の自然数))としてあり得る値の昇順
    // {商, yの最小値, yの最大値 + 1}
    // ↑に対してfを適用
    template<typename F>
    static void apply_range(T x, F f) {
        assert(x >= 0);
        if (x == 0) return;
        T sq = sqrtl(x);
        T y = (x + sq) / (sq + 1);
        for (T i = 1, pre1 = x + 1, pre2; i <= y; i++) {
            pre2 = x / i;
            if (pre2 * i != x) assert(pre1 != ++pre2);
            f(i, pre2, pre1);
            pre1 = pre2;
        }
        if ((x + sq - 1) / sq == y) sq--;
        for (T i = sq; i >= 1; i--) f((x + i - 1) / i, i, i + 1); // [1...sq-1]で割った答えは全て異なる
    }

    // ceil(x / x以下の自然数)として小さい方からk番目の要素
    // ない場合は-1
    static T kth(T x, T k) {
        assert(x >= 0);
        if (x == 0) return -1;
        T sq = sqrtl(x);
        T y = (x + sq) / (sq + 1);
        if (k < y) return k + 1;
        T len = y + (sq - 1) + ((x + sq - 1) / sq != y);
        if (len <= k) return -1;
        return (x + len - k - 1) / (len - k);
    }

    // ceil(x / x以下の自然数)としてあり得る数の種類
    static T num(T x) {
        assert(x >= 0);
        if (x == 0) return -1;
        T sq = sqrtl(x);
        T y = (x + sq) / (sq + 1);
        T len = y + (sq - 1) + ((x + sq - 1) / sq != y);
        return len;
    }

    // ceil(x / y(x以下の自然数))として小さい方からk番目の要素
    // {商, yの最小値, yの最大値 + 1}
    // ない場合は-1
    static std::tuple<T, T, T> kth_range(T x, T k) {
        T z = kth(x, k);
        if (z == 1) return {1, x, x + 1};
        if (z == -1) return {-1, -1, -1};
        T l = x / z, r = x / (z - 1);
        return {z, l + (l * z != x), r + (r * z != x)};
    }
};
#endif
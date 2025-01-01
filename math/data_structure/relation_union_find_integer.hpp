#ifndef _RELATION_UNION_FIND_H_
#define _RELATION_UNION_FIND_H_
#include <vector>
#include <numeric>
#include <algorithm>

// x_i = x_j * {-1/0/1} + b
// 閉路ができて確定する答えは整数/2になる
template<typename T>
struct relation_union_find_integer {
  private:
    int num_cc;
    std::vector<int> sz;
    std::vector<std::pair<T, T>> diff; // その時点での親との関係式
    std::vector<std::pair<T, T>> ans; // 答えとしてあり得る値[l, r]
    static constexpr T inf = std::numeric_limits<T>::max();
    static constexpr T minf = std::numeric_limits<T>::min();

    // x_1 = x_2 * a + b (a != 0)のとき
    // x_2 = x_1 * A + Bを満たす{A, B}を返す
    std::pair<T, T> inv(const std::pair<T, T> &a) {
        if (a.first == 1) return {1, -a.second};
        return {-1, a.second};
    }
    std::pair<T, T> composite(const std::pair<T, T> &a, const std::pair<T, T> &b) {
        return {a.first * b.first, a.second * b.first + b.second};
    }
    // inf, minfをaとして使うと壊れる
    T composite(T a, const std::pair<T, T> &b) {
        if (a == inf) return b.first == 1 ? inf : minf;
        if (a == minf) return b.first == 1 ? minf : inf;
        return a * b.first + 2 * b.second;
    }
    int find(int u) {
        if (sz[u] < 0) return u;
        int p = find(sz[u]);
        if (sz[u] != p) diff[u] = composite(diff[sz[u]], diff[u]);
        return sz[u] = p;
    }
    int size(int u) {
        return -sz[find(u)];
    }
    bool same(int u, int v) {
        return find(u) == find(v);
    }
  
  public:
    relation_union_find_integer(int n): num_cc(n), sz(n, -1), diff(n, {1, 0}), ans(n, {minf, inf}) {}

    // 条件 x_u = x_v * a + b を追加できるか
    // |a| <= 1
    bool check_relation(int u, int v, T a, T b) {
        assert(abs(a) <= 1);
        if (a == 0) return check_const(u, b);
        int ur = find(u), vr = find(v);
        if (ur == vr) {
            auto [A, B] = composite(diff[v], {a, b});
            auto [X, Y] = diff[u];
            if (A - X == 0) return (Y - B) == 0;
            T x = 2 * (Y - B) / (A - X);
            return ans[ur].first <= x && x <= ans[ur].second;
        }
        std::pair<T, T> c = composite(diff[v], composite({a, b}, inv(diff[u])));
        T lur = composite(ans[vr].first, c), rur = composite(ans[vr].second, c);
        if (c.first == -1) std::swap(lur, rur);
        return std::max(ans[ur].first, lur) <= std::min(ans[ur].second, rur);
    }
    
    // 条件 x_u = x_v * a + b を追加
    bool add_relation(int u, int v, T a, T b) {
        assert(abs(a) <= 1);
        if (a == 0) return add_const(u, b);
        int ur = find(u), vr = find(v);
        if (ur == vr) {
            auto [A, B] = composite(diff[v], {a, b});
            auto [X, Y] = diff[u];
            if (A - X == 0) return (Y - B) == 0;
            T x = 2 * (Y - B) / (A - X);
            if (x < ans[ur].first || ans[ur].second < x) return false;
            ans[ur] = {x, x};
            return true;
        }
        if (sz[ur] < sz[vr]) {
            std::swap(u, v);
            std::swap(ur, vr);
            std::tie(a, b) = inv({a, b});
        }
        std::pair<T, T> c = composite(diff[v], composite({a, b}, inv(diff[u])));
        T lur = composite(ans[vr].first, c), rur = composite(ans[vr].second, c);
        if (c.first == -1) std::swap(lur, rur);
        lur = std::max(ans[ur].first, lur);
        rur = std::min(ans[ur].second, rur);
        if (lur > rur) return false;
        num_cc--;
        ans[ur] = {lur, rur};
        sz[vr] += sz[ur];
        sz[ur] = vr;
        diff[ur] = c;
        std::pair<T, T> i = inv(c);
        T lvr = composite(ans[ur].first, i), rvr = composite(ans[ur].second, i);
        if (i.first == -1) std::swap(lvr, rvr);
        ans[vr].first = std::max(ans[vr].first, lvr);
        ans[vr].second = std::min(ans[vr].second, rvr);
        return true;
    }

    // 条件 l <= a < r を追加できるか
    bool check_range(int u, T l, T r) {
        l *= 2, r = 2 * (r - 1);
        int ur = find(u);
        std::pair<T, T> f = inv(diff[u]);
        T lur = composite(l, f), rur = composite(r, f);
        if (f.first == -1) std::swap(lur, rur);
        return std::max(ans[ur].first, lur) <= std::min(ans[ur].second, rur);
    }

    // 条件 l <= a < r を追加
    bool add_range(int u, T l, T r) {
        l *= 2, r = 2 * (r - 1);
        int ur = find(u);
        std::pair<T, T> f = inv(diff[u]);
        T lur = composite(l, f), rur = composite(r, f);
        if (f.first == -1) std::swap(lur, rur);
        lur = std::max(ans[ur].first, lur), rur = std::min(ans[ur].second, rur);
        if (lur > rur) return false;
        ans[ur] = {lur, rur};
        return true;
    }

    // 条件 x_u = a を追加できるか
    bool check_const(int u, T a) {
        a *= 2;
        int ur = find(u);
        T aur = composite(a, inv(diff[u]));
        return ans[ur].first <= aur && aur <= ans[ur].second;
    }

    // 条件 x_u = a を追加
    bool add_const(int u, T a) {
        a *= 2;
        int ur = find(u);
        T aur = composite(a, inv(diff[u]));
        if (aur < ans[ur].first || ans[ur].second < aur) return false;
        ans[ur] = {aur, aur};
        return true;
    }

    // あり得るx_uの区間が[l, r)の場合{2l, 2r}を返す
    std::pair<T, T> get2x(int u) {
        int ur = find(u);
        T lu = composite(ans[ur].first, diff[u]);
        T ru = composite(ans[ur].second, diff[u]);
        if (diff[u].first == -1) std::swap(lu, ru);
        return {lu, (ru == inf ? inf : ru + 1)};
    }

    // x_u = x_v * a + bという式が存在するか, 存在する場合その値
    std::tuple<bool, T, T> relation(int u, int v) {
        int ur = find(u), vr = find(v);
        if (ur != vr) return {false, 0, 0};
        auto [x, y] = composite(inv(diff[v]), diff[u]);
        return {true, x, y};
    }

    // 連結成分の数
    int count_cc() {
        return num_cc;
    }
};
#endif
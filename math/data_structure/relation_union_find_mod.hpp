#ifndef _RELATION_UNION_FIND_MOD_H_
#define _RELATION_UNION_FIND_MOD_H_
#include <vector>
#include <algorithm>
#include <numeric>

template<typename mint>
struct relation_union_find_mod {
  private:
    int num_cc;
    std::vector<int> sz;
    std::vector<std::pair<mint, mint>> diff;
    std::vector<bool> decided;
    std::vector<mint> ans;

    // x_1 = x_2 * a + b (a != 0)のとき
    // x_2 = x_1 * A + Bを満たす{A, B}を返す
    std::pair<mint, mint> inv(const std::pair<mint, mint> &a) {
        mint A = a.first.inv();
        return {A, -a.second * A};
    }
    std::pair<mint, mint> composite(const std::pair<mint, mint> &a, const std::pair<mint, mint> &b) {
        return {a.first * b.first, a.second * b.first + b.second};
    }
    int find(int u) {
        if (sz[u] < 0) return u;
        int p = find(sz[u]);
        if (sz[u] != p) {
            diff[u] = composite(diff[sz[u]], diff[u]);
        }
        return sz[u] = p;;
    }
    int size(int u) {
        return -sz[find(u)];
    }
    bool same(int u, int v) {
        return find(u) == find(v);
    }
  
  public:
    relation_union_find_mod(int n): num_cc(n), sz(n, -1), diff(n, {1, 0}), decided(n, false), ans(n, 0) {}

    // 条件 x_u = x_v * a + b を追加できるか
    bool check_relation(int u, int v, mint a, mint b) {
        if (a == 0) return check_const(u, b);
        int ur = find(u), vr = find(v);
        if (ur == vr) {
            auto [A, B] = composite(diff[v], {a, b});
            auto [X, Y] = diff[u];
            if (decided[ur]) return ans[ur] * (A - X) == Y - B;
            if (A - X == 0) return (Y - B) == 0;
            return true;
        }
        if (!decided[ur] || !decided[vr]) return true;
        auto [c, d] = composite(diff[v], composite({a, b}, inv(diff[u])));
        return ans[vr] * c + d == ans[ur];
    }

    // 条件 x_u = x_v * a + b を追加
    bool add_relation(int u, int v, mint a, mint b) {
        if (a == 0) return add_const(u, b);
        int ur = find(u), vr = find(v);
        if (ur == vr) {
            auto [A, B] = composite(diff[v], {a, b});
            auto [X, Y] = diff[u];
            if (decided[ur]) return ans[ur] * (A - X) == Y - B;
            if (A - X == 0) return (Y - B) == 0;
            decided[ur] = true;
            ans[ur] = (Y - B) / (A - X);
            return true;
        }
        if (sz[ur] < sz[vr]) {
            std::swap(u, v);
            std::swap(ur, vr);
            std::tie(a, b) = inv({a, b});
        }
        // ur = vr * c + d
        auto [c, d] = composite(diff[v], composite({a, b}, inv(diff[u])));
        if (decided[ur] && decided[vr] && ans[vr] * c + d != ans[ur]) return false;
        num_cc--;
        sz[vr] += sz[ur];
        sz[ur] = vr;
        diff[ur] = {c, d};
        if (!decided[vr] && decided[ur]) {
            decided[vr] = true;
            auto [e, f] = inv({c, d});
            ans[vr] = ans[ur] * e + f;
        }
        return true;
    }
    
    // 条件 x_u = aを追加できるか
    bool check_const(int u, mint a) {
        int ur = find(u);
        if (!decided[ur]) return true;
        return (ans[ur] * diff[u].first + diff[u].second) == a;
    }

    // 条件 x_u = aを追加
    bool add_const(int u, mint a) {
        int ur = find(u);
        if (!decided[ur]) {
            auto [c, d] = inv(diff[u]);
            ans[ur] = a * c + d;
            decided[ur] = true;
        }
        return (ans[ur] * diff[u].first + diff[u].second) == a;
    }

    // {確定しているか, その値}
    std::pair<bool, mint> get(int u) {
        int ur = find(u);
        if (!decided[ur]) return {false, 0};
        return {true, (ans[ur] * diff[u].first + diff[u].second)};
    }
    
    // x_u = x_v * a + bという式が存在するか, 存在する場合その値
    std::tuple<bool, mint, mint> relation(int u, int v) {
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
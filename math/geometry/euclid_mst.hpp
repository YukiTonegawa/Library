/*
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <functional>
#include <cassert>
#include <climits>
#include <iomanip>
#include <numeric>
#include <memory>
#include <random>
#include <thread>
#include <chrono>
#define allof(obj) (obj).begin(), (obj).end()
#define range(i, l, r) for(int i=l;i<r;i++)
#define unique_elem(obj) obj.erase(std::unique(allof(obj)), obj.end())
#define bit_subset(i, S) for(int i=S, zero_cnt=0;(zero_cnt+=i==S)<2;i=(i-1)&S)
#define bit_kpop(i, n, k) for(int i=(1<<k)-1,x_bit,y_bit;i<(1<<n);x_bit=(i&-i),y_bit=i+x_bit,i=(!i?(1<<n):((i&~y_bit)/x_bit>>1)|y_bit))
#define bit_kth(i, k) ((i >> k)&1)
#define bit_highest(i) (i?63-__builtin_clzll(i):-1)
#define bit_lowest(i) (i?__builtin_ctzll(i):-1)
#define sleepms(t) std::this_thread::sleep_for(std::chrono::milliseconds(t))
using ll = long long;
using ld = long double;
using ul = uint64_t;
using pi = std::pair<int, int>;
using pl = std::pair<ll, ll>;
using namespace std;

template<typename F, typename S>
std::ostream &operator << (std::ostream &dest, const std::pair<F, S> &p) {
    dest << p.first << ' ' << p.second;
    return dest;
}

template<typename A, typename B>
std::ostream &operator << (std::ostream &dest, const std::tuple<A, B> &t) {
    dest << std::get<0>(t) << ' ' << std::get<1>(t);
    return dest;
}

template<typename A, typename B, typename C>
std::ostream &operator << (std::ostream &dest, const std::tuple<A, B, C> &t) {
    dest << std::get<0>(t) << ' ' << std::get<1>(t) << ' ' << std::get<2>(t);
    return dest;
}

template<typename A, typename B, typename C, typename D>
std::ostream &operator << (std::ostream &dest, const std::tuple<A, B, C, D> &t) {
    dest << std::get<0>(t) << ' ' << std::get<1>(t) << ' ' << std::get<2>(t) << ' ' << std::get<3>(t);
    return dest;
}

template<typename T>
std::ostream &operator << (std::ostream &dest, const std::vector<std::vector<T>> &v) {
    int sz = v.size();
    if (!sz) return dest;
    for (int i = 0; i < sz; i++) {
        int m = v[i].size();
        for (int j = 0; j < m; j++) dest << v[i][j] << (i != sz - 1 && j == m - 1 ? '\n' : ' ');
    }
    return dest;
}

template<typename T>
std::ostream &operator << (std::ostream &dest, const std::vector<T> &v) {
    int sz = v.size();
    if (!sz) return dest;
    for (int i = 0; i < sz - 1; i++) dest << v[i] << ' ';
    dest << v[sz - 1];
    return dest;
}

template<typename T, size_t sz>
std::ostream &operator << (std::ostream &dest, const std::array<T, sz> &v) {
    if (!sz) return dest;
    for (int i = 0; i < sz - 1; i++) dest << v[i] << ' ';
    dest << v[sz - 1];
    return dest;
}

template<typename T>
std::ostream &operator << (std::ostream &dest, const std::set<T> &v) {
    for (auto itr = v.begin(); itr != v.end();) {
        dest << *itr;
        itr++;
        if (itr != v.end()) dest << ' ';
    }
    return dest;
}

template<typename T, typename E>
std::ostream &operator << (std::ostream &dest, const std::map<T, E> &v) {
    for (auto itr = v.begin(); itr != v.end(); ) {
        dest << '(' << itr->first << ", " << itr->second << ')';
        itr++;
        if (itr != v.end()) dest << '\n';
    }
    return dest;
}

template<typename T>
vector<T> make_vec(size_t sz, T val) { return std::vector<T>(sz, val); }

template<typename T, typename... Tail>
auto make_vec(size_t sz, Tail ...tail) {
    return std::vector<decltype(make_vec<T>(tail...))>(sz, make_vec<T>(tail...));
}

template<typename T>
vector<T> read_vec(size_t sz) {
    std::vector<T> v(sz);
    for (int i = 0; i < (int)sz; i++) std::cin >> v[i];
    return v;
}

template<typename T, typename... Tail>
auto read_vec(size_t sz, Tail ...tail) {
    auto v = std::vector<decltype(read_vec<T>(tail...))>(sz);
    for (int i = 0; i < (int)sz; i++) v[i] = read_vec<T>(tail...);
    return v;
}

// x / y以上の最小の整数
ll ceil_div(ll x, ll y) {
    assert(y > 0);
    return (x + (x > 0 ? y - 1 : 0)) / y;
}

// x / y以下の最大の整数
ll floor_div(ll x, ll y) {
    assert(y > 0);
    return (x + (x > 0 ? 0 : -y + 1)) / y;
}

void io_init() {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
}

//#include ".lib/base/fast_io.hpp"



template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
struct kd_tree {
  public:
    struct point {
        Idx x, y;
        S z;
        point() {}
        point(Idx x, Idx y, S z) : x(x), y(y), z(z) {}
    };

  private:
    static constexpr Idx inf = std::numeric_limits<Idx>::max() / 2;
    static constexpr Idx minf = -inf;

    struct node {
        Idx lx, rx, ly, ry;
        int r;
        S sum;
        node () : lx(inf), rx(minf), ly(inf), ry(minf), r(0), sum(e()) {}
        node (point p) : lx(p.x), rx(p.x), ly(p.y), ry(p.y), r(0), sum(p.z) {}
    };
  
    std::vector<node> V{node()};
    std::vector<int> Leaf, Par{0}, Flag{0};

    void update_coord(int v) {
        V[v].lx = V[v].ly = inf;
        V[v].rx = V[v].ry = minf;
        if (Flag[v + 1] > 0) {
            node &u = V[v + 1];
            V[v].lx = u.lx;
            V[v].rx = u.rx;
            V[v].ly = u.ly;
            V[v].ry = u.ry;
        }
        if (Flag[V[v].r] > 0) {
            node &u = V[V[v].r];
            V[v].lx = std::min(V[v].lx, u.lx);
            V[v].rx = std::max(V[v].rx, u.rx);
            V[v].ly = std::min(V[v].ly, u.ly);
            V[v].ry = std::max(V[v].ry, u.ry);
        }
    }

    void update_val(int v) {
        S lsum = Flag[v + 1] > 0 ? V[v + 1].sum : e();
        S rsum = Flag[V[v].r] > 0 ? V[V[v].r].sum : e();
        V[v].sum = op(lsum, rsum);
    }

    int make_node() {
        int res = V.size();
        V.push_back(node());
        Par.push_back(0);
        Flag.push_back(2);
        return res;
    }

    int make_node(point p) {
        int res = V.size();
        V.push_back(node(p));
        Par.push_back(0);
        Flag.push_back(1);
        return res;
    }

    int build(std::vector<std::pair<point, int>> &v, int l, int r, bool dim) {
        if(r - l == 1) {
            int u = make_node(v[l].first);
            Leaf[v[l].second] = u;
            return u;
        }
        int m = (l + r) / 2;
        if constexpr (true) {
            if (!dim) {
                std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.x < b.first.x; });
            } else {
                std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.y < b.first.y; });
            }
        } else {
            // ヒューリスティック
            Idx lx = inf, rx = minf, ly = inf, ry = minf;
            for (int i = l; i < r; i++) {
                lx = std::min(lx, v[i].first.x);
                rx = std::max(rx, v[i].first.x);
                ly = std::min(ly, v[i].first.y);
                ry = std::max(ry, v[i].first.y);
            }
            if ((long long)rx - lx > (long long)ry - ly) {
                std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.x < b.first.x; });
            } else {
                std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.y < b.first.y; });
            }
        }
        int u = make_node();
        build(v, l, m, !dim);
        Par[u + 1] = u;
        V[u].r = build(v, m, r, !dim);
        Par[V[u].r] = u;
        update_val(u);
        update_coord(u);
        return u;
    }
    int root = 1;
    static std::array<int, 100000> st;

  public:
    kd_tree() {}
    kd_tree(const std::vector<point> &v) : Leaf(v.size()) { 
        if(!v.empty()) {
            std::vector<std::pair<point, int>> P(v.size());
            for (int i = 0; i < v.size(); i++) P[i] = {v[i], i};
            root = build(P, 0, v.size(), 0);
        }
    }

    // offにした点番号iを復活
    void on(int i) {
        i = Leaf[i];
        if (Flag[i] == 1) return;
        Flag[i] = 1;
        while (i) {
            i = Par[i];
            update_val(i);
            update_coord(i);
        }
    }

    // 点番号iを一時的に削除
    void off(int i) {
        i = Leaf[i];
        if (Flag[i] == 0) return;
        Flag[i] = 0;
        while (i) {
            i = Par[i];
            update_val(i);
            update_coord(i);
        }
    }

    // 点番号iの値をzにする
    void set(int i, S z) {
        i = Leaf[i];
        V[i].sum = z;
        while (i) {
            i = Par[i];
            update_val(i);
        }
    }

    // 点番号iの値
    point get(int i) {
        i = Leaf[i];
        return {V[i].lx, V[i].ly, V[i].sum};
    }

    template<typename Idx2>
    std::pair<bool, point> closest_euclid(Idx x, Idx y) {
        Idx2 mindist = std::numeric_limits<Idx2>::max();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (Flag[v] < 2) {
                Idx2 dx = V[v].lx - x, dy = V[v].ly - y;
                Idx2 dist = dx * dx + dy * dy;
                if (Flag[v] == 1 && dist < mindist) {
                    mindist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx2 dx1 = std::max({x - V[a].rx, V[a].lx - x, 0}), dy1 = std::max({y - V[a].ry, V[a].ly - y, 0});
            Idx2 dx2 = std::max({x - V[b].rx, V[b].lx - x, 0}), dy2 = std::max({y - V[b].ry, V[b].ly - y, 0});
            Idx2 dist1 = dx1 * dx1 + dy1 * dy1, dist2 = dx2 * dx2 + dy2 * dy2;
            if (dist1 < dist2) {
                if (mindist > dist1) dfs(dfs, a);
                if (mindist > dist2) dfs(dfs, b);
            } else {
                if (mindist > dist2) dfs(dfs, b);
                if (mindist > dist1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return {mindist != std::numeric_limits<Idx2>::max(), res};
    }

    // onの点のうち(x, y)と最も近い点 
    template<typename Idx2>
    std::pair<bool, point> closest_euclid(Idx x, Idx y, Idx lx, Idx rx, Idx ly, Idx ry) {
        Idx2 mindist = std::numeric_limits<Idx2>::max();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (V[v].rx < lx || rx < V[v].lx || V[v].ry < ly || ry < V[v].ly) return;
            if (Flag[v] < 2) {
                Idx2 dx = V[v].lx - x, dy = V[v].ly - y;
                Idx2 dist = dx * dx + dy * dy;
                if (Flag[v] == 1 && dist < mindist) {
                    mindist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx2 dx1 = std::max({x - std::min(V[a].rx, rx), std::max(V[a].lx, lx) - x, 0});
            Idx2 dy1 = std::max({y - std::min(V[a].ry, ry), std::max(V[a].ly, ly) - y, 0});
            Idx2 dx2 = std::max({x - std::min(V[b].rx, rx), std::max(V[b].lx, lx) - x, 0});
            Idx2 dy2 = std::max({y - std::min(V[b].ry, ry), std::max(V[b].ly, ly) - y, 0});
            Idx2 dist1 = dx1 * dx1 + dy1 * dy1, dist2 = dx2 * dx2 + dy2 * dy2;
            if (dist1 < dist2) {
                if (mindist > dist1) dfs(dfs, a);
                if (mindist > dist2) dfs(dfs, b);
            } else {
                if (mindist > dist2) dfs(dfs, b);
                if (mindist > dist1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return {mindist != std::numeric_limits<Idx2>::max(), res};
    }

    template<typename Idx2>
    std::pair<bool, point> closest_euclid1(Idx x, Idx y) {
        Idx2 mindist = std::numeric_limits<Idx2>::max();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (V[v].rx < x || V[v].ry < y) return;
            Idx X = std::max(x, V[v].lx) - x, Y = V[v].ry - y;
            if (X >= Y) return;
            if (Flag[v] < 2) {
                Idx2 dx = V[v].lx - x, dy = V[v].ly - y;
                Idx2 dist = dx * dx + dy * dy;
                if (Flag[v] == 1 && dist < mindist) {
                    mindist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx2 dx1 = std::max(std::max(V[a].lx, x) - x, 0);
            Idx2 dy1 = std::max(std::max(V[a].ly, y) - y, 0);
            Idx2 dx2 = std::max(std::max(V[b].lx, x) - x, 0);
            Idx2 dy2 = std::max(std::max(V[b].ly, y) - y, 0);
            Idx2 dist1 = dx1 * dx1 + dy1 * dy1, dist2 = dx2 * dx2 + dy2 * dy2;
            if (dist1 < dist2) {
                if (mindist > dist1) dfs(dfs, a);
                if (mindist > dist2) dfs(dfs, b);
            } else {
                if (mindist > dist2) dfs(dfs, b);
                if (mindist > dist1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return {mindist != std::numeric_limits<Idx2>::max(), res};
    }

    template<typename Idx2>
    std::pair<bool, point> closest_euclid2(Idx x, Idx y) {
        Idx2 mindist = std::numeric_limits<Idx2>::max();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (V[v].rx < x || V[v].ry < y) return;
            Idx X = V[v].rx - x, Y = std::max(y, V[v].ly) - y;
            if (X < Y) return;
            if (Flag[v] < 2) {
                Idx2 dx = V[v].lx - x, dy = V[v].ly - y;
                Idx2 dist = dx * dx + dy * dy;
                if (Flag[v] == 1 && dist < mindist) {
                    mindist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx2 dx1 = std::max(std::max(V[a].lx, x) - x, 0);
            Idx2 dy1 = std::max(std::max(V[a].ly, y) - y, 0);
            Idx2 dx2 = std::max(std::max(V[b].lx, x) - x, 0);
            Idx2 dy2 = std::max(std::max(V[b].ly, y) - y, 0);
            Idx2 dist1 = dx1 * dx1 + dy1 * dy1, dist2 = dx2 * dx2 + dy2 * dy2;
            if (dist1 < dist2) {
                if (mindist > dist1) dfs(dfs, a);
                if (mindist > dist2) dfs(dfs, b);
            } else {
                if (mindist > dist2) dfs(dfs, b);
                if (mindist > dist1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return {mindist != std::numeric_limits<Idx2>::max(), res};
    }

    std::pair<bool, point> closest_manhattan1(Idx x, Idx y) {
        Idx mindist = std::numeric_limits<Idx>::max();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (V[v].rx < x || V[v].ry < y) return;
            Idx X = std::max(x, V[v].lx) - x, Y = V[v].ry - y;
            if (X >= Y) return;
            if (Flag[v] < 2) {
                Idx dist = abs(V[v].lx - x) + abs(V[v].ly - y);
                if (Flag[v] == 1 && dist < mindist) {
                    mindist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx dx1 = std::max(V[a].lx - x, 0);
            Idx dy1 = std::max(V[a].ly - y, 0);
            Idx dx2 = std::max(V[b].lx - x, 0);
            Idx dy2 = std::max(V[b].ly - y, 0);
            if (dx1 + dy1 < dx2 + dy2) {
                if (mindist > dx1 + dy1) dfs(dfs, a);
                if (mindist > dx2 + dy2) dfs(dfs, b);
            } else {
                if (mindist > dx2 + dy2) dfs(dfs, b);
                if (mindist > dx1 + dy1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return {mindist != std::numeric_limits<Idx>::max(), res};
    }

    std::pair<bool, point> closest_manhattan2(Idx x, Idx y) {
        Idx mindist = std::numeric_limits<Idx>::max();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (V[v].rx < x || V[v].ry < y) return;
            Idx X = V[v].rx - x, Y = std::max(y, V[v].ly) - y;
            if (X < Y) return;
            if (Flag[v] < 2) {
                Idx dist = abs(V[v].lx - x) + abs(V[v].ly - y);
                if (Flag[v] == 1 && dist < mindist) {
                    mindist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx dx1 = std::max(V[a].lx - x, 0);
            Idx dy1 = std::max(V[a].ly - y, 0);
            Idx dx2 = std::max(V[b].lx - x, 0);
            Idx dy2 = std::max(V[b].ly - y, 0);
            if (dx1 + dy1 < dx2 + dy2) {
                if (mindist > dx1 + dy1) dfs(dfs, a);
                if (mindist > dx2 + dy2) dfs(dfs, b);
            } else {
                if (mindist > dx2 + dy2) dfs(dfs, b);
                if (mindist > dx1 + dy1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return {mindist != std::numeric_limits<Idx>::max(), res};
    }

};

template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
std::array<int, 100000> kd_tree<Idx, S, op, e>::st;


using S = int;
static constexpr S op(S a, S b) {
    return a + b;
}
static constexpr S e() {
    return 0;
}

#include ".lib/graph/general/kruskal.hpp"
#include ".lib/graph/base/edge.hpp"

int main() {
    using kdt = kd_tree<int, S, op, e>;
    using point = typename kdt::point;
    io_init();
    
    int N;
    std::cin >> N;

    std::vector<point> P1(N), P2(N), P3(N), P4(N);
    for (int i = 0; i < N; i++) {
        int x, y;
        std::cin >> x >> y;
        P1[i] = {x, y, i};
        P2[i] = {-y, x, i};
        P3[i] = {-x, -y, i};
        P4[i] = {y, -x, i};
    }

    kdt tree1(P1), tree2(P2), tree3(P3), tree4(P4);

    std::vector<std::pair<int, int>> E;

    auto dist = [&](int _i, int _j) -> double {
        double dx = P1[_i].x - P1[_j].x;
        double dy = P1[_i].y - P1[_j].y;
        return sqrt(dx * dx + dy * dy);
    };

    for (int i = 0; i < N; i++) {
        tree1.off(i);
        tree2.off(i);
        tree3.off(i);
        tree4.off(i);
        {
            int x = P1[i].x, y = P1[i].y;
            {
                auto [f, p] = tree1.closest_euclid1<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
            {
                auto [f, p] = tree1.closest_euclid2<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
            {
                auto [f, p] = tree1.closest_euclid<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
        }

        {
            int x = P2[i].x, y = P2[i].y;
            {
                auto [f, p] = tree2.closest_euclid1<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
            {
                auto [f, p] = tree2.closest_euclid2<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
        }

        {
            int x = P3[i].x, y = P3[i].y;
            {
                auto [f, p] = tree3.closest_euclid1<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
            {
                auto [f, p] = tree3.closest_euclid2<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
        }

        {
            int x = P4[i].x, y = P4[i].y;
            {
                auto [f, p] = tree4.closest_euclid1<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
            {
                auto [f, p] = tree4.closest_euclid2<long long>(x, y);
                if (f) E.push_back({i, p.z});
            }
        }
        tree1.off(i);
        tree2.off(i);
        tree3.off(i);
        tree4.off(i);
    }


    using e = weighted_edge<double>;
    std::vector<std::pair<int, e>> E2;
    for (auto [i, j] : E) {
        E2.push_back({i, {j, dist(i, j)}});
    }

    auto V = kruskal<e>(N, E2);
    for (auto [i, j] : V) {
        std::cout << i << " " << j.to() << '\n';
    }
}
*/

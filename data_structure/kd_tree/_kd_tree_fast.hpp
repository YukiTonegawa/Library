
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




constexpr unsigned int bit_ceil(unsigned int n) {
    unsigned int x = 1;
    while (x < (unsigned int)(n)) x *= 2;
    return x;
}

constexpr int bit_ceil_log(unsigned int n) {
    int x = 0;
    while ((1 << x) < (unsigned int)(n)) x++;
    return x;
}



#include <unistd.h>

// サイズは空白や改行も含めた文字数
template<int size_in = 1 << 25, int size_out = 1 << 25>
struct fast_io {
    char ibuf[size_in], obuf[size_out];
    char *ip, *op;

    fast_io() : ip(ibuf), op(obuf) {
        int t = 0, k = 0;
        while ((k = read(STDIN_FILENO, ibuf + t, sizeof(ibuf) - t)) > 0) {
            t += k;
        }
    }
    
    ~fast_io() {
        int t = 0, k = 0;
        while ((k = write(STDOUT_FILENO, obuf + t, op - obuf - t)) > 0) {
            t += k;
        }
    }
  
    long long in() {
        long long x = 0;
        bool neg = false;
        for (; *ip < '+'; ip++) ;
        if (*ip == '-'){ neg = true; ip++;}
        else if (*ip == '+') ip++;
        for (; *ip >= '0'; ip++) x = 10 * x + *ip - '0';
        if (neg) x = -x;
        return x;
    }

    unsigned long long inu64() {
        unsigned long long x = 0;
        for (; *ip < '+'; ip++) ;
        if (*ip == '+') ip++;
        for (; *ip >= '0'; ip++) x = 10 * x + *ip - '0';
        return x;
    }

    char in_char() {
        for (; *ip < '!'; ip++) ;
        return *ip++;
    }
  
    void out(long long x, char c = 0) {
        static char tmp[20];
        if (!x) {
            *op++ = '0';
        } else {
            int i;
            if (x < 0) {
                *op++ = '-';
                x = -x;
            }
            for (i = 0; x; i++) {
                tmp[i] = x % 10;
                x /= 10;
            }
            for (i--; i >= 0; i--) *op++ = tmp[i] + '0';
        }
        if (c) *op++ = c;
    }

    void outu64(unsigned long long x, char c = 0) {
        static char tmp[20];
        if (!x) {
            *op++ = '0';
        } else {
            int i;
            for (i = 0; x; i++) {
                tmp[i] = x % 10;
                x /= 10;
            }
            for (i--; i >= 0; i--) *op++ = tmp[i] + '0';
        }
        if (c) *op++ = c;
    }

    void out_char(char x, char c = 0){
        *op++ = x;
        if (c) *op++ = c;
    }

    long long memory_size() {
        return (long long)(size_in + size_out) * sizeof(char);
    }
};



template<typename Idx, typename S, S (*op)(S, S), S (*e)(), typename F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
struct kd_tree_lazy {
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
        int size;
        S sum;
        F lz;
        node () : lx(inf), rx(minf), ly(inf), ry(minf), size(0), sum(e()), lz(id()){}
        node (point p) : lx(p.x), rx(p.x), ly(p.y), ry(p.y), size(1), sum(p.z), lz(id()) {}
    };
    int log, sz;
    std::vector<node> V{node()};

    void update(int v) {
        V[v].size = V[v * 2].size + V[v * 2 + 1].size;
        V[v].lx = std::min(V[v * 2].lx, V[v * 2 + 1].lx);
        V[v].rx = std::max(V[v * 2].rx, V[v * 2 + 1].rx);
        V[v].ly = std::min(V[v * 2].ly, V[v * 2 + 1].ly);
        V[v].ry = std::max(V[v * 2].ry, V[v * 2 + 1].ry);
        update_val(v);
    }
    void update_val(int v) {
        V[v].sum = op(V[v * 2].sum, V[v * 2 + 1].sum);
    }

    void all_apply(int v, F lz) {
        V[v].sum = mapping(lz, V[v].sum, V[v].size);
        V[v].lz = composition(lz, V[v].lz);
    }

    void push_down(int v) {
        if (V[v].lz != id()) {
            all_apply(v * 2, V[v].lz);
            all_apply(v * 2 + 1, V[v].lz);
            V[v].lz = id();
        }
    }

    void build(std::vector<std::pair<point, int>> &v, int l, int r, bool dim) {
        int len = r - l;
        if (len == 1) {
            V[l + sz] = node(v[l].first);
            if (v[l].second != sz) info[v[l].second].second = l;
            return;
        }
        int m = (l + r) / 2;
        if (!dim) {
            std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.x < b.first.x; });
        } else {
            std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.y < b.first.y; });
        }
        build(v, l, m, !dim);
        build(v, m, r, !dim);
    }
    static std::array<int, 100000> que;
    static std::array<int, 100000> que_upd;
    std::vector<std::pair<S, int>> info; // {値, 内部インデックス}

  public:
    kd_tree_lazy() {}

    kd_tree_lazy(const std::vector<point> &v) {
        log = bit_ceil_log(std::max(1, int(v.size())));
        sz = 1 << log;
        std::vector<std::pair<point, int>> P(sz, {point{inf, inf, e()}, sz});
        for (int i = 0; i < int(v.size()); i++) P[i] = {v[i], i};
        info.resize(sz, {e(), -1});
        V.resize(2 * sz);
        build(P, 0, sz, 0);
        for (int i = sz - 1; i > 0; i--) update(i);
    }

    void on(int k) {
        int t = info[k].second + sz;
        for (int i = log; i > 0; i--) push_down(t >> i);
        if (V[t].size == 1) return;
        V[t].size = 1;
        V[t].sum = info[k].first;
        while (t > 1) {
            t /= 2;
            update(t);
        }
    }

    void off(int k) {
        int t = info[k].second + sz;
        for (int i = log; i > 0; i--) push_down(t >> i);
        info[k].first = V[t].sum;
        V[t].size = 0;
        V[t].sum = e();
        while (t > 1) {
            t /= 2;
            update(t);
        }
    }

    void set(int k, S x) {
        int t = info[k].second + sz;
        for (int i = log; i > 0; i--) push_down(t >> i);
        assert(V[t].size == 1);
        V[t].sum = x;
        while (t > 1) {
            t /= 2;
            update(t);
        }
    }

    // O(√N)
    void apply_rectangle(Idx LX, Idx RX, Idx LY, Idx RY, F lz) {
        RX--, RY--;
        if (LX > RX || LY > RY) return;
        int l = 0, r = 0;
        int k = 0;
        que[r++] = 1;
        while (l < r) {
            int v = que[l++];
            if (V[v].size == 0 || V[v].rx < LX || RX < V[v].lx || V[v].ry < LY || RY < V[v].ly) continue;
            if (LX <= V[v].lx && V[v].rx <= RX && LY <= V[v].ly && V[v].ry <= RY) {
                all_apply(v, lz);
            } else {
                push_down(v);
                que[r++] = v * 2;
                que[r++] = v * 2 + 1;
                que_upd[k++] = v;
            } 
        }
        while (k--) update_val(que_upd[k]);
    }

    // O(√N)
    S prod_rectangle(Idx LX, Idx RX, Idx LY, Idx RY) {
        RX--, RY--;
        if (LX > RX || LY > RY) return e();
        int l = 0, r = 0;
        que[r++] = 1;
        S res = e();
        while (l < r) {
            int v = que[l++];
            if (V[v].size == 0 || V[v].rx < LX || RX < V[v].lx || V[v].ry < LY || RY < V[v].ly) continue;
            if (LX <= V[v].lx && V[v].rx <= RX && LY <= V[v].ly && V[v].ry <= RY) {
                res = op(res, V[v].sum);
            } else {
                push_down(v);
                que[r++] = v * 2;
                que[r++] = v * 2 + 1;
            }
        }
        return res;
    }
};

template<typename Idx, typename S, S (*op)(S, S), S (*e)(), typename F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
std::array<int, 100000> kd_tree_lazy<Idx, S, op, e, F, mapping, composition, id>::que;
template<typename Idx, typename S, S (*op)(S, S), S (*e)(), typename F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
std::array<int, 100000> kd_tree_lazy<Idx, S, op, e, F, mapping, composition, id>::que_upd;


static constexpr int mod = 998244353;
using S = int;
using F = std::pair<int, int>;

static constexpr S op(S a, S b) {
    return (a + b >= mod ? a + b - mod : a + b);
}
static constexpr S e() {
    return 0;
}
static constexpr S mapping(F a, S b, int c) {
    return ((long long)a.first * b + (long long)a.second * c) % mod;
}
static constexpr F composition(F a, F b) {
    return {(long long)a.first * b.first % mod, ((long long)b.second * a.first + a.second) % mod};
}
static constexpr F id() {
    return {1, 0};
}

using kdt = kd_tree_lazy<int, S, op, e, F, mapping, composition, id>;
using point = typename kdt::point;

int main() {
    fast_io<1 << 25, 1 << 24> io;
    int N = io.in();
    int Q = io.in();
    
    std::vector<point> P;
    for (int i = 0; i < N; i++) {
        int x, y, z;
        x = io.in();
        y = io.in();
        z = io.in();
        P.push_back(point{x, y, z});
    }

    std::vector<std::array<int, 7>> QU(Q);

    for (int i = 0; i < Q; i++) {
        int t = io.in();
        QU[i][0] = t;
        if (t == 0) {
            for (int j = 1; j < 4; j++) QU[i][j] = io.in();
            P.push_back(point{QU[i][1], QU[i][2], QU[i][3]});
        } else if (t == 1) {
            for (int j = 1; j < 3; j++) QU[i][j] = io.in();
        } else if (t == 2) {
            for (int j = 1; j < 5; j++) QU[i][j] = io.in();
        } else {
            for (int j = 1; j < 7; j++) QU[i][j] = io.in();
        }
    }

    kdt tree(P);
    for (int i = N; i < P.size(); i++) tree.off(i);

    int k = N;
    for (int i = 0; i < Q; i++) {
        int t = QU[i][0];
        if (t == 0) {
            tree.on(k++);
        } else if (t == 1) {
            int x = QU[i][1];
            int w = QU[i][2];
            tree.set(x, w);
        } else if (t == 2) {
            int lx = QU[i][1];
            int rx = QU[i][3];
            int ly = QU[i][2];
            int ry = QU[i][4];
            io.out(tree.prod_rectangle(lx, rx, ly, ry), '\n');
        } else {
            int lx = QU[i][1];
            int rx = QU[i][3];
            int ly = QU[i][2];
            int ry = QU[i][4];
            int a = QU[i][5];
            int b = QU[i][6];
            tree.apply_rectangle(lx, rx, ly, ry, {a, b});
        }
    }
}

*/
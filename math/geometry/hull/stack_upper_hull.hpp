#ifndef _STACK_UPPER_HULL_H_
#define _STACK_UPPER_HULL_H_
#include <cassert>
#include <vector>
#include <array>
#include "../point.hpp"

// T : 点の値が納まる型
// T2 : (点の値)^2が納まる型
template<typename T, typename T2>
struct stack_upper_hull {
    using point = Point<T, T2>;
  private:
    static constexpr int log_max_n = 20;
    std::vector<std::array<int, log_max_n>> par; // 凸包上の頂点を2^i個遡った時のインデックス
    std::vector<point> P;
    std::vector<int> sz;
    bool cw(int a, int b, int c) { return point::cross(P[a], P[b], P[c]) > 0; }
    // n <= 2^kとなるような最小のk
    int ceillog2(int n) {
        if (n == 0) return 0;
        int k = 31 - __builtin_clz(n);
        return n > (1 << k) ? k + 1 : k;
    }
  public:
    // 昇順にi番目の点
    point get(int i) { return P[i]; }

    // 点pを追加 O(logN)
    void push(point p) {
        assert(P.empty() || P.back() <= p);
        int k = P.size() - 1;
        P.push_back(p);
        if (k != -1 && k != 0) {
            if (!cw(par[k][0], k, P.size() - 1)) {
                for (int i = ceillog2(sz.back()); i >= 0; i--) {
                    int b = par[k][i];
                    if (b == -1 || par[b][0] == -1) continue;
                    if (!cw(par[b][0], b, P.size() - 1)) k = b;
                }
                k = par[k][0];
            }
        }
        sz.push_back(k == -1 ? 1 : sz[k] + 1);
        par.push_back({});
        par.back().fill(-1);
        par.back()[0] = k;
        for (int i = 1; i < log_max_n; i++) {
            if (par.back()[i - 1] == -1) break;
            par.back()[i] = par[par.back()[i - 1]][i - 1];
        }
    }
    // 末尾を削除 O(1)
    void pop() {
        P.pop_back();
        par.pop_back();
        sz.pop_back();
    }
    // 点全体の数 O(1)
    int size_points() {
        return P.size();
    }
    // 下側凸包のサイズ O(1)
    int size_hull() {
        if (sz.empty()) return 0;
        return sz.back();
    }
    // 全体の上側凸包で小さい順にk個目, O(logN)
    int kth_point(int k) {
        assert(0 <= k && k < size_hull());
        k = size_hull() - 1 - k;
        int pos = (int)P.size() - 1;
        for (int i = ceillog2(sz.back()); i >= 0; i--) {
            if ((1 << i) > k) continue;
            pos = par[pos][i];
            k -= 1 << i;
        }
        return pos;
    }
    // 全体の上側凸包にi番目の点が含まれるか(含まれない場合は-1, 含まれる場合はその順番)
    // O(logN)
    int order(int i) {
        aassert(0 <= i && i < P.size());
        int pos = P.size() - 1;
        int res = sz.back() - 1;
        for (int j = ceillog2(sz.back()); j >= 0; j--) {
            if (par[pos][j] < i) continue;
            res -= 1 << j;
            if (par[pos][j] == i) return res;
            pos = par[pos][j];
        }
        return (pos == i ? res : -1);
    }
    // [0, i]の点だけで作った上側凸包でiの左の点(ない場合は-1)
    // iが全体の上側凸包に含まれる場合この左の点も含まれ, 隣り合っている
    // O(1)
    int left(int i) {
        return par[i][0];
    }
    // [0, i]の点だけで作った上側凸包でiのk個左の点(ない場合は-1)
    // O(logN)
    int left_k(int i, int k) {
        for (int j = ceillog2(sz[i]); j >= 0; j--) {
            if ((1 << j) > k) continue;
            k -= (1 << j);
            i = par[i][j];
            if (i == -1 || k == 0) return i;
        }
        return -1;
    }
    // [0, r)の点にax+bを足したと仮定した時の[0, r)の{最大値を取る点, 最大値}
    // O(logN)
    std::pair<int, T> linear_add_max(int r, T a, T b) {
        assert(0 < r);
        int pos = r - 1;
        for (int i = ceillog2(sz[r - 1]); i >= 0; i--) {
            int mid = par[pos][i];
            if (mid == -1) continue;
            if ((P[mid].x - P[pos].x) * a > (P[pos].y - P[mid].y)) {
                pos = mid;
            }
        }
        return {pos, P[pos].x * a + P[pos].y};
    }
    // [0, i]の点だけで作った上側凸包のうちp以上の最小, ない場合は-1
    int lower_bound(int i, point p) {
        assert(0 <= i && i < size_points());
        if(P[i] < p) return -1;
        for (int j = ceillog2(sz[i]); j >= 0; j--) {
            if (par[i][j] == -1 || P[par[i][j]] < p) continue;
            i = par[i][j];
        }
        return i;
    }
    // [0, i]の点だけで作った上側凸包のうちp以下の最大, ない場合は-1
    int lower_bound_rev(int i, point p) {
        assert(0 <= i && i < size_points());
        if(p < P[0]) return -1;
        for (int j = ceillog2(sz[i]); j >= 0; j--) {
            if (par[i][j] == -1 || P[par[i][j]] <= p) continue;
            i = par[i][j];
        }
        return par[i][0];
    }
    // [0, i]の点だけで作った上側凸包の点を列挙
    std::vector<int> enumerate(int i) {
        assert(0 <= i && i < size_points());
        std::vector<int> res(sz[i]);
        int j = sz[i] - 1;
        while (i != -1) {
            res[j--] = i;
            i = par[i][0];
        }
        return res;
    }
};
#endif
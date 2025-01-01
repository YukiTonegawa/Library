#ifndef _LI_CHAO_TREE_COMPRESSED_H_
#define _LI_CHAO_TREE_COMPRESSED_H_
#include <algorithm>
#include <vector>
#include "../base/bit_ceil.hpp"

template<typename T>
struct li_chao_tree_compressed {
    static constexpr T inf = std::numeric_limits<T>::max();
    struct line {
        T a, b;
        line(T a, T b): a(a), b(b) {}
        inline T get(T x) { return a * x + b; }
    };
  private:
    int N, M;
    T xmax;
    std::vector<line> L;
    std::vector<T> sample_points;

    void add_line(int k, line &x, int lx, int rx, T x_l, T x_r) {
        if (k >= 2 * M - 1) return;
        T l = sample_points[lx], r = sample_points[rx];
        T t_l = L[k].get(l), t_r = L[k].get(r);
        if (M - 1 <= k) {
            if (x_l < t_l) L[k] = x;
            return;
        } else if (t_l <= x_l && t_r <= x_r) {
            return;
        } else if (t_l >= x_l && t_r >= x_r) {
            L[k] = x;
            return;
        } else {
            int mx = (lx + rx) / 2;
            T m = sample_points[mx];
            T t_m = L[k].get(m), x_m = x.get(m);
            if (t_m > x_m) {
                std::swap(L[k], x);
                if (x_l >= t_l) add_line(k * 2 + 1, x, lx, mx, t_l, t_m);
                else add_line(k * 2 + 2, x, mx, rx, t_m, t_r);
            } else {
                if (t_l >= x_l) add_line(k * 2 + 1, x, lx, mx, x_l, x_m);
                else add_line(k * 2 + 2, x, mx, rx, x_m, x_r);
            }
        }
    }
    void add_segment(int k, line &x, int ax, int bx, int lx, int rx, T x_l, T x_r) {
        if (2 * M - 1 <= k || rx < ax || bx < lx) return;
        if (ax <= lx && rx <= bx) {
            line y(x);
            return add_line(k, y, lx, rx, x_l, x_r);
        }
        if (L[k].get(sample_points[lx]) <= x_l && L[k].get(sample_points[rx]) <= x_r) return;
        int mx = (lx + rx) / 2;
        T m = sample_points[mx];
        T x_m = x.get(m);
        add_segment(k * 2 + 1, x, ax, bx, lx, mx, x_l, x_m);
        add_segment(k * 2 + 2, x, ax, bx, mx, rx, x_m, x_r);
    }
    T min(int k, int lx, int rx, T x) {
        if (M - 1 <= k) return L[k].get(x);
        int mx = (lx + rx) / 2;
        T m = sample_points[mx];
        if (x < m) return std::min(L[k].get(x), min(k * 2 + 1, lx, mx, x));
        else return std::min(L[k].get(x), min(k * 2 + 2, mx, rx, x));
    }
    line min_line(int k, T lx, T rx, T x) {
        if (M - 1 <= k) return L[k];
        int mx = (lx + rx) / 2;
        T m = sample_points[mx];
        if (x < m) {
            line res = min_line(k * 2 + 1, lx, mx, x);
            return L[k].get(x) <= res.get(x) ? L[k] : res;
        } else {
            line res = min_line(k * 2 + 2, mx, rx, x);
            return L[k].get(x) <= res.get(x) ? L[k] : res;
        }
    }

  public:
    // x: クエリを飛ばす点の集合(線分の端点は追加しなくていい)
    // xはソート済みかつユニーク
    li_chao_tree_compressed(const std::vector<T> &x) : N(x.size()), M(bit_ceil(N)), L(2 * M - 1, line(0, inf)), sample_points(x) {
        xmax = (x.empty() ? 0 : x.back() + 1);
        sample_points.resize(M + 1, xmax);
    }

    // 直線ax + bを追加
    void add_line(T a, T b) {
        line x(a, b);
        add_line(0, x, 0, M, x.get(sample_points[0]), x.get(xmax));
    }
    
    // 線分ax + b, [l, r)を追加
    void add_segment(T l, T r, T a, T b) {
        line x(a, b);
        int lx = std::lower_bound(sample_points.begin(), sample_points.end(), l) - sample_points.begin();
        int rx = std::lower_bound(sample_points.begin(), sample_points.end(), r) - sample_points.begin();
        add_segment(0, x, lx, rx, 0, M, x.get(sample_points[0]), x.get(xmax));
    }

    // f(x)の最小値
    T min(T x) {
        return min(0, 0, M, x);
    }

    // f(x)の最小値をとる線
    line min_line(T x) {
        return min_line(0, 0, M, x);
    }
};

#endif

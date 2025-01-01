#ifndef _FENWICK_TREE2D_H_
#define _FENWICK_TREE2D_H_
#include <vector>
#include <limits>
#include <cassert>

template<typename T>
struct fenwick_tree2d {
  private:
    int H, W;
    std::vector<std::vector<T>> V;

  public:
    fenwick_tree2d() : H(0), W(0) {}
    fenwick_tree2d(int _h, int _w): H(_h), W(_w), V(H + 1, std::vector<T>(W + 1, 0)) {}
    fenwick_tree2d(const std::vector<std::vector<T>> &v): H(v.size()) {
        if (v.empty()) return;
        W = v[0].size();
        V.resize(H + 1, std::vector<T>(W + 1, 0));
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                V[i + 1][j + 1] = v[i][j];
            }
        }
        for (int i = 1; i <= H; i++) {
            for (int j = 1; j <= W; j++) {
                int next_j = j + (j & (-j));
                if (next_j <= W) V[i][next_j] += V[i][j];
            }
        }

        for (int i = 1; i <= H; i++) {
            int next_i = i + (i & (-i));
            if (next_i <= H) {
                for (int j = 1; j <= W; j++) {
                    V[next_i][j] += V[i][j];
                }
            }
        }
    }
        
    // 点(x, y)にzを足す
    void apply(int x, int y, T z) {
        for (int i = x + 1; i <= H; i += (i & (-i))) {
            for (int j = y + 1; j <= W; j += (j & (-j))) {
                V[i][j] += z;
            }
        }
    }

    T get(int x, int y) {
        x++, y++;
        auto get_y = [&](int i) -> T {
            T res = V[y];
            int lsb = 1;
            while ((y & lsb) == 0) {
                res = op(res, inv(V[i][y - lsb]));
                lsb <<= 1;
            }
            return res;
        };
        T res = get_y(x);
        int lsb = 1;
        while ((x & lsb) == 0) {
            res = op(res, inv(get_y(x - lsb)));
            lsb <<= 1;
        }
        return res;
    }

    // [0, rx) × [0, ry)の和
    T prod(int rx, int ry) {
        T res = 0;
        for (int i = rx; i > 0; i -= (i & (-i))) {
            for (int j = ry; j > 0; j -= (j & (-j))) {
                res += V[i][j];
            }
        }
        return res;
    }

    // [lx, rx)　× [ly, ry)の和
    T prod_naive(int lx, int rx, int ly, int ry) {
        if (lx >= rx || ly >= ry) return 0;
        return prod(rx, ry) - prod(rx, ly) - prod(lx, ry) + prod(lx, ly);
    }

    // [lx, rx)　× [ly, ry)の和
    T prod(int lx, int rx, int ly, int ry) {
        auto get_y = [&](int x) {
            int _ly = ly, _ry = ry;
            T sum = 0;
            while ((_ry | _ly) != _ly) {
                sum += V[x][_ry];
                _ry -= (_ry & (-_ry));
            }
            while (_ly > _ry) {
                sum -= V[x][_ly];
                _ly -= (_ly & (-_ly));
            }
            return sum;
        };
        T sum = 0;
        while ((rx | lx) != lx) {
            sum += get_y(rx);
            rx -= (rx & (-rx));
        }
        while (lx > rx) {
            sum -= get_y(lx);
            lx -= (lx & (-lx));
        }
        return sum;
    }
};
#endif
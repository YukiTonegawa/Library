#ifndef _RECT_ADD_RECT_SUM_DYNAMIC_H_
#define _RECT_ADD_RECT_SUM_DYNAMIC_H_
#include <array>
#include <vector>

template<typename Idx, typename Val>
struct rectangle_add_rectangle_sum {
  private:
    struct bit4 {
        int N;
        std::vector<std::array<Val, 4>> sum;
        bit4(int N): N(N), sum(N + 1, {0, 0, 0, 0}) {}

        void apply(Idx l, Idx r, int lc, int rc, Val z1, Val z2) {
            Val a = l * z1, b = r * z1, c = l * z2, d = r * z2;
            for (int i = lc + 1; i <= N; i += (i & (-i))) {
                sum[i][0] -= a;
                sum[i][1] += z1;
                sum[i][2] -= c;
                sum[i][3] += z2;
            }
            for (int i = rc + 1; i <= N; i += (i & (-i))) {
                sum[i][0] += b;
                sum[i][1] -= z1;
                sum[i][2] += d;
                sum[i][3] -= z2;
            }
        }

        std::pair<Val, Val> prod(Idx r, int rc) {
            Val a = 0, b = 0, c = 0, d = 0;
            for (int i = rc; i > 0; i -= (i & (-i))) {
                a += sum[i][0];
                b += sum[i][1];
                c += sum[i][2];
                d += sum[i][3];
            }
            return {a + (b * r), c + (d * r)};
        }

        std::pair<Val, Val> prod(Idx l, Idx r, int lc, int rc) {
            auto [cr, dxr] = prod(r, rc);
            auto [cl, dxl] = prod(l, lc);
            return {cr - cl, dxr - dxl};
        }
    };

    struct Event {
        Idx lx, rx, ly, ry;
        Val z;
        int id, lyc, ryc;
        Event(Idx lx, Idx rx, Idx ly, Idx ry, Val z, int id, int lyc = 0, int ryc = 0): lx(lx), rx(rx), ly(ly), ry(ry), z(z), id(id), lyc(lyc), ryc(ryc) {}
    };

    std::vector<int> qcnt{0};
    std::vector<Event> E;

    void solve(int l, int r, std::vector<Val> &ans) {
        
        static constexpr int DO_NAIVE = 20;
        if (r - l < 2) return;
        if (qcnt[r] - qcnt[l] == 0 || qcnt[r] - qcnt[l] == r - l) return;
        if (r - l <= DO_NAIVE) {
            for (int i = l; i < r; i++) {
                if (E[i].id == -1) continue;
                for (int j = l; j < i; j++) {
                    if (E[j].id != -1) continue;
                    Idx lx = std::max(E[i].lx, E[j].lx);
                    Idx rx = std::min(E[i].rx, E[j].rx);
                    Idx ly = std::max(E[i].ly, E[j].ly);
                    Idx ry = std::min(E[i].ry, E[j].ry);
                    if (lx < rx && ly < ry) {
                        ans[E[i].id] += Val(rx - lx) * Val(ry - ly) * E[j].z;
                    }
                }
            }
            return;
        }

        int m = (l + r) / 2;
        solve(l, m, ans);
        solve(m, r, ans);

        std::vector<Idx> Y;
        for (int i = l; i < m; i++) {
            if (E[i].id == -1) {
                Y.push_back(E[i].ly);
                Y.push_back(E[i].ry);
            }
        }
        std::sort(Y.begin(), Y.end());
        Y.erase(std::unique(Y.begin(), Y.end()), Y.end());
        
        auto lb = [&](Idx y) -> int { return std::lower_bound(Y.begin(), Y.end(), y) - Y.begin();};

        std::vector<Event> tmp;

        for (int i = l; i < r; i++) { 
            int lyc = lb(E[i].ly), ryc = lb(E[i].ry);
            if (i < m && E[i].id == -1) {
                tmp.push_back(Event(E[i].lx, 0, E[i].ly, E[i].ry, E[i].z, -1, lyc, ryc));
                tmp.push_back(Event(E[i].rx, 0, E[i].ly, E[i].ry, -E[i].z, -1, lyc, ryc));
            }
            if (i >= m && E[i].id != -1) {
                tmp.push_back(Event(E[i].lx, 0, E[i].ly, E[i].ry, 0, E[i].id + qcnt.back(), lyc, ryc));
                tmp.push_back(Event(E[i].rx, 0, E[i].ly, E[i].ry, 0, E[i].id, lyc, ryc));
            }
        }

        std::sort(tmp.begin(), tmp.end(), [](const Event&a, const Event &b) {
            if (a.lx != b.lx) return a.lx < b.lx;
            return a.id > b.id;
        });

        bit4 bit(Y.size());
        int qidx = 0;
        for (Event &e : tmp) {
            if (e.id == -1) {
                bit.apply(e.ly, e.ry, e.lyc, e.ryc, -e.z * e.lx, e.z);
            } else {
                auto [a, b] = bit.prod(e.ly, e.ry, e.lyc, e.ryc);
                if (e.id >= qcnt.back()) {
                    ans[e.id - qcnt.back()] -= a + e.lx * b;
                } else {
                    ans[e.id] += a + e.lx * b;
                }
                if (++qidx == 2 * (qcnt[r] - qcnt[l])) return;
            }
        }
    }

  public:
    rectangle_add_rectangle_sum() {}
    
    void apply(Idx lx, Idx rx, Idx ly, Idx ry, Val z) {
        E.push_back(Event(lx, rx, ly, ry, z, -1));
        qcnt.push_back(qcnt.back());
    }

    void prod(Idx lx, Idx rx, Idx ly, Idx ry) {
        E.push_back(Event(lx, rx, ly, ry, 0, qcnt.back()));
        qcnt.push_back(qcnt.back() + 1);
    }

    std::vector<Val> solve() {
        std::vector<Val> ans(qcnt.back(), 0);
        solve(0, E.size(), ans);
        return ans;
    }
};
#endif
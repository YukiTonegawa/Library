#ifndef _RECTANGLE_ADD_POINT_GET_H_
#define _RECTANGLE_ADD_POINT_GET_H_

#include "../fenwick_tree/fenwick_tree.hpp"

template<typename Idx, typename Val>
struct rectangle_add_point_get {
  private:
    static constexpr int qlim = 1e8;
    struct Query {
        Idx x, y;
        int id;
    };
    struct Update {
        Idx lx, rx, ly, ry;
        Val z;
    };
    struct Event {
        Idx x;
        int lyc, ryc;
        Val z;
    };
    std::vector<Update> U;
    std::vector<Query> Q;
    std::vector<std::pair<bool, int>> T;
    
    void solve(int l, int r, std::vector<Val> &ans) {
        static constexpr int DO_NAIVE = 10;
        if (r - l < 2) return;
        if (r - l <= DO_NAIVE) {
            for (int i = l; i < r; i++) {
                if (T[i].first == 1) continue;
                Idx x = Q[T[i].second].x;
                Idx y = Q[T[i].second].y;
                for (int j = l; j < i; j++) {
                    int k = T[j].second;
                    if (T[j].first == 1 && U[k].lx <= x && x < U[k].rx && U[k].ly <= y && y < U[k].ry) {
                       ans[T[i].second] += U[k].z;
                    }
                }
            }
            return;
        }
        
        int mid = (l + r) / 2;
        solve(l, mid, ans);
        solve(mid, r, ans);
        std::vector<Idx> Y;
        for (int i = mid; i < r; i++) {
            if (T[i].first) continue;
            int id = T[i].second;
            Y.push_back(Q[id].y);
        }

        if (Y.empty()) return;
        std::sort(Y.begin(), Y.end());
        Y.erase(std::unique(Y.begin(), Y.end()), Y.end());
        std::vector<Event> E;
        for (int i = l; i < mid; i++) {
            if (!T[i].first) continue;
            int id = T[i].second;
            int lyc = std::lower_bound(Y.begin(), Y.end(), U[id].ly) - Y.begin();
            int ryc = std::lower_bound(Y.begin(), Y.end(), U[id].ry) - Y.begin();
            E.push_back(Event{U[id].lx, lyc, ryc, U[id].z});
            E.push_back(Event{U[id].rx, lyc, ryc, -U[id].z});
        }

        for (int i = mid; i < r; i++) {
            if (T[i].first) continue;
            int id = T[i].second;
            int y = std::lower_bound(Y.begin(), Y.end(), Q[id].y) - Y.begin();
            E.push_back(Event{Q[id].x, y, Q[id].id + qlim, 0});
        }

        std::sort(E.begin(), E.end(), [](const Event &a, const Event &b) {
            if (a.x == b.x) return a.ryc < b.ryc;
            return a.x < b.x;
        });

        fenwick_tree<Val> ft(Y.size());
        for (const Event &e : E) {
            if (e.ryc < qlim) {
                if (e.lyc < Y.size()) ft.apply(e.lyc, e.z);
                if (e.ryc < Y.size()) ft.apply(e.ryc, -e.z);
            } else {
                int id = e.ryc - qlim;
                ans[id] += ft.prod(e.lyc + 1);
            }
        }
    }

  public:
    // [lx, rx) × [ly, ry)にzを足す
    void apply(Idx lx, Idx rx, Idx ly, Idx ry, Val z) {
        T.push_back({1, U.size()});
        U.push_back(Update{lx, rx, ly, ry, z});
    }

    // get(x, y)
    void prod(Idx x, Idx y) {
        T.push_back({0, Q.size()});
        Q.push_back(Query{x, y, (int)Q.size()});
    }

    std::vector<Val> solve() {
        std::vector<Val> ans(Q.size(), 0);
        solve(0, T.size(), ans);
        return ans;
    }
};
#endif
#ifndef _DYNAMIC_MST_H_
#define _DYNAMIC_MST_H_
#include "mst_incremental.hpp"
#include <vector>
#include <unordered_map>
#include <functional>

// F: (mst, query_id) -> void
template<typename W, typename F = std::function<void(mst_incremental<W>&, int)>>
struct dynamic_mst_offline {
  public:
    using MST = mst_incremental<W>;
    MST mst;
  private:
    static constexpr int inf_time = 1 << 30;
    int n;
    std::unordered_multimap<long long, int> E; // {a, b, inesrt_time}
    std::vector<std::pair<int, F>> Q;
    struct Update { int a, b, link_time, cut_time; W w; };
    std::vector<Update> U{Update{0, 0, 0, 0, 0}};

    void solve(int l, int r, int &j, int &k) {
        assert(l != r);
        if (r - l == 1) {
            while (k < Q.size() && Q[k].first == l) {
                Q[k++].second(mst, j++);
            }
            return;
        }
        int mid = (l + r) / 2;
        int cnt = 0;
        for (int i = mid; i < r; i++) {
            if (U[i].link_time <= l) {
                cnt++;
                mst.add_edge(U[i].a, U[i].b, U[i].w);
            }
        }
        solve(l, mid, j, k);
        while (cnt) {
            cnt--;
            mst.erase_last_edge();
        }
        for (int i = l; i <= mid; i++) {
            if (r <= U[i].cut_time) {
                cnt++;
                mst.add_edge(U[i].a, U[i].b, U[i].w);
            }
        }
        solve(mid, r, j, k);
        while(cnt) {
            cnt--;
            mst.erase_last_edge();
        }
    }
  public:
    dynamic_mst_offline(int n) : n(n), E(n) {
        for (int i = 0; i < n; i++) mst.add_vertex();
    }

    // 辺a-bを追加する, すでにある場合は多重辺が追加される
    void link(int a, int b, W c) {
        if (a > b) std::swap(a, b);
        if (a == b) return;
        int link_time = U.size();
        E.emplace(((long long)a << 30) + b, link_time);
        U.push_back(Update{a, b, link_time, inf_time, c});
    }

    // 辺a-bを消す, ない場合は何もしない
    void cut(int a, int b) {
        if (a > b) std::swap(a, b);
        if (a == b) return;
        auto itr = E.find(((long long)a << 30) + b);
        if (itr != E.end()) {
            int link_time = itr->second;
            int cut_time = U.size();
            U[link_time].cut_time = cut_time;
            U.push_back({a, b, link_time, cut_time});
            E.erase(itr);
        }
    }

    void query(F f) {
        int query_time = (int)U.size() - 1;
        Q.push_back(std::make_pair(query_time, f));
    }

    void solve() { 
        int j = 0, k = 0;
        solve(0, U.size(), j, k);
    }
};
#endif
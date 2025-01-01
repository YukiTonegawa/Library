#ifndef _OFFLINE_DYNAMIC_QUERY_H_
#define _OFFLINE_DYNAMIC_QUERY_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include <limits>
#include <functional>
#include <map>

// F: (query_id) -> void  取得
// G: 更新用の構造体 operator <, == が必要
// コンストラクタ (function<void(G&)>, function<void()>)  Gを入力とする更新用関数, rollback用関数
template<typename F, typename G>
struct offline_dynamic_query {
  private:
    static constexpr int inf_time = 1 << 30;
    const std::function<void(G&)> update_func;
    const std::function<void()> rollback_func;

    std::multimap<G, int> E; // {g, inesrt_time}
    std::vector<std::pair<int, F>> Q;
    struct Update{ G g; int link_time, cut_time;};
    std::vector<Update> U{Update{G(), 0, 0}};

    void solve(int l, int r, int &j, int &k) {
        assert(l != r);
        if (r - l == 1) {
            while (k < Q.size() && Q[k].first == l) Q[k++].second(j++);
            return;
        }
        int mid = (l + r) / 2;
        int cnt = 0;
        for (int i = mid; i < r; i++) if(U[i].link_time <= l) cnt++, update_func(U[i].g);
        solve(l, mid, j, k);
        while (cnt) cnt--, rollback_func();
        for (int i = l; i <= mid; i++) if (r <= U[i].cut_time) cnt++, update_func(U[i].g);
        solve(mid, r, j, k);
        while (cnt) cnt--, rollback_func();
    }

  public:
    offline_dynamic_query(std::function<void(G&)> ufunc, std::function<void()> rfunc): update_func(ufunc), rollback_func(rfunc) {}

    void update(G g) {
        int insert_time = U.size();
        E.emplace(g, insert_time);
        U.push_back(Update{g, insert_time, inf_time});
    }

    void update_unique(G g) {
        int insert_time = U.size();
        auto itr = E.find(g);
        if (itr != E.end()) return;
        E.emplace(g, insert_time);
        U.push_back(Update{g, insert_time, inf_time});
    }

    // hを消す, ない場合は何もしない
    void erase(G g) {
        auto itr = E.find(g);
        if (itr == E.end()) return;
        int link_time = itr->second;
        int cut_time = U.size();
        U[link_time].cut_time = cut_time;
        U.push_back({g, link_time, cut_time});
        E.erase(itr);
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
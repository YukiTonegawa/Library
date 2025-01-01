#ifndef _RANGE_TYPES_H_
#define _RANGE_TYPES_H_
#include "../rectangle/rectangle_sum.hpp"
#include "../rectangle/point_add_rectangle_sum.hpp"
#include "../ordered_set/ordered_set.hpp"
#include <unordered_map>

// O((N + Q)logN)
template<typename T>
struct offline_static_range_types {
    int n;
    std::vector<std::pair<int, int>> E;
    std::vector<std::tuple<int, int, int>> Q;
    
    offline_static_range_types(const std::vector<T> &val) {
        std::unordered_map<T, int> mp; // {値, 最後に現れたインデックス}
        n = val.size();
        for (int i = 0; i < n; i++) {
            auto itr = mp.find(val[i]);
            int prev;
            if (itr == mp.end()) {
                prev = -1;
                mp.emplace(val[i], i);
            } else {
                prev = itr->second;
                itr->second = i;
            }
            // lがprev+1以上になったら追加する
            E.push_back({prev + 1, i});
        }
    }
  
    // A[l, r)の種類数
    void query(int l, int r) {
        int id = Q.size();
        Q.push_back({l, r, id});
    }

    std::vector<int> solve() {
        std::vector<int> ans(Q.size());
        std::sort(Q.begin(), Q.end());
        std::sort(E.begin(), E.end());
        fenwick_tree<int> bit(n);
        int j = 0;
        for (auto [l, r, id] : Q) {
            while (j < E.size() && E[j].first <= l) {
                bit.apply(E[j].second, 1);
                if (E[j].first != 0) bit.apply(E[j].first - 1, -1);
                j++;
            }
            ans[id] = bit.prod(l, r);
        }
        return ans;
    }
};

// 十分大きな値INFとして, 初期値は A = {INF, INF + 1, INF + 2, INF + 3...}, (初期状態はN種類)
template<typename T>
struct offline_point_set_range_types {
  private:
    struct query_type {
        int t, id, l, r;
        T val;
        query_type(int a, int b, int c, int d, T e): t(a), id(b), l(c), r(d), val(e) {}
    };
    std::vector<query_type> q;
    int qs = 0;

    std::vector<int> solve_inner() {
        std::vector<int> ans(qs, 0);
        point_add_rectangle_sum<int, int> rect;
        std::vector<ordered_set<int>> idx_table;
        int max_idx = -1;
        // 追加される値で座圧
        std::vector<T> zrev;
        for (int i = 0; i < q.size(); i++) {
            if (q[i].t == 0) {
                zrev.push_back(q[i].val);
                max_idx = std::max(max_idx, q[i].id);
            }
        }
        std::sort(zrev.begin(), zrev.end());
        zrev.erase(std::unique(zrev.begin(), zrev.end()), zrev.end());
        std::vector<int> v(max_idx + 1, -1);
        idx_table.resize(zrev.size());
        for (auto qi : q) {
            if (qi.t == 0) {
                int i = qi.id, y = v[i];
                int z = std::lower_bound(zrev.begin(), zrev.end(), qi.val) - zrev.begin();
                if (z == y) continue;
                if (y != -1) {
                    auto [f, prev] = idx_table[y].lt(i);
                    auto [g, next] = idx_table[y].gt(i);
                    if (f && g) rect.apply(prev, next, 1);
                    if (f) rect.apply(prev, i, -1);
                    if (g) rect.apply(qi.id, i, -1);
                    idx_table[y].erase(i);
                }
                auto [f, prev] = idx_table[z].lt(i);
                auto [g, next] = idx_table[z].gt(i);
                if (f && g) rect.apply(prev, next, -1);
                if (f) rect.apply(prev, i, 1);
                if (g) rect.apply(i, next, 1);
                idx_table[z].insert(i);
                v[i] = z;
            } else {
                rect.prod(qi.l, qi.r, qi.l, qi.r);
            }
        }
        auto rect_sum = rect.solve();
        int j = 0;
        for (auto qi : q) {
            if (qi.t == 1) {
                ans[qi.id] = qi.r - qi.l - rect_sum[j++];
            }
        }
        return ans;
    }

  public:
    offline_point_set_range_types() {}
    
    // A[i] <- val
    void update(int i, T val) {
        q.push_back(query_type(0, i, 0, 0, val));
    }
  
    // A[l, r)の種類数
    void query(int l, int r) {
        q.push_back(query_type(1, qs++, l, r, 0));
    }

    std::vector<int> solve() {
        return solve_inner();
    }
};
#endif
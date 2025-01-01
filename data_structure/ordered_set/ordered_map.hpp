#ifndef _ORDERED_MAP_H_
#define _ORDERED_MAP_H_
#include <vector>
#include <iostream>
#include <bitset>
#include <cassert>
#include <algorithm>
#include "base/ordered_map_small.hpp"

template<typename T, typename U>
struct ordered_map {
  private:
    struct node {
        int h, _size;
        ordered_map_small<T, U> x;
        node *l, *r;
        node() : h(1), _size(0), l(nullptr), r(nullptr) {}
        node(ordered_map_small<T, U> x) : h(1), _size(x.size), x(x), l(nullptr), r(nullptr) {}
        int balanace_factor() {
            return (l ? l->h : 0) - (r ? r->h : 0);
        }
    };

    static int size(node *v) { return v ? v->_size : 0; }

    static void update(node *v) {
        v->h = std::max(v->l ? v->l->h : 0, v->r ? v->r->h : 0) + 1;
        v->_size = v->x.size;
        if (v->l) {
            v->_size += v->l->_size;
        }
        if (v->r) {
            v->_size += v->r->_size;
        }
    }

    node *rotate_right(node *v) {
        node *l = v->l;
        v->l = l->r;
        l->r = v;
        update(v);
        update(l);
        return l;
    }
    
    node *rotate_left(node *v) {
        node *r = v->r;
        v->r = r->l;
        r->l = v;
        update(v);
        update(r);
        return r;
    }

    node *balance(node *v) {
        int bf = v->balanace_factor();
        assert(-2 <= bf && bf <= 2);
        if (bf == 2) {
            if (v->l->balanace_factor() == -1) {
                v->l = rotate_left(v->l);
                update(v);
            }
            return rotate_right(v);
        } else if(bf == -2) {
            if (v->r->balanace_factor() == 1) {
                v->r = rotate_right(v->r);
                update(v);
            }
            return rotate_left(v);
        }
        return v;
    }

    node *insert_leftmost(node *v, node *u) {
        if (!v) return u;
        v->l = insert_leftmost(v->l, u);
        update(v);
        return balance(v);
    }

    node *cut_leftmost(node *v, node* &u) {
        if (!v->l) {
            u = v;
            return v->r;
        }
        v->l = cut_leftmost(v->l, u);
        update(v);
        return balance(v);
    }

    template<bool REPLACE = false>
    bool _emplace(T x, U y) {
        bool res = false;
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v) {
                v = new node();
                v->x.val[0] = x;
                v->x.second[0] = y;
                v->x.size = 1;
                res = true;
                update(v);
                return v;
            }
            if (v->l && x < v->x.min()) {
                v->l = dfs(dfs, v->l);
            } else if (v->r && x > v->x.max()) {
                v->r = dfs(dfs, v->r);
            } else {
                if constexpr (REPLACE) {
                    res = v->x.emplace_replace(x, y);
                } else {
                    res = v->x.emplace(x, y);
                }
                if (v->x.size == ordered_map_small<T, U>::max_size) {
                    node *u = new node(v->x.split_half());
                    update(u);
                    v->r = insert_leftmost(v->r, u);   
                }
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
        return res;
    }


  public:
    node *root;
    ordered_map() : root(nullptr) {}
    // ソート済かつキーがユニーク
    ordered_map(const std::vector<std::pair<T, U>> &val) : root(nullptr) {
        if (val.empty()) return;
        std::vector<node*> nodes;
        int N = val.size();
        int B = ordered_map_small<T, U>::max_size / 2;
        int M = (N + B - 1) / B;
        for (int i = 0; i < M; i++) {
            int l = B * i, r = std::min(N, l + B);
            ordered_map_small<T, U> x;
            for (int j = l; j < r; j++) {
                assert(!j || val[j - 1].first < val[j].first);
                x.val[j - l] = val[j].first;
                x.second[j - l] = val[j].second;
            }
            x.size = r - l;
            nodes.push_back(new node(x));
        }
        auto dfs = [&](auto &&dfs, int l, int r) -> node* {
            int m = (l + r) / 2;
            if (l < m) nodes[m]->l = dfs(dfs, l, m);
            if (m + 1 < r) nodes[m]->r = dfs(dfs, m + 1, r);
            update(nodes[m]);
            return nodes[m];
        };
        root = dfs(dfs, 0, nodes.size());
    }

    int size() {
        return size(root);
    }

    // 追加できたか
    bool emplace(T x, U y) {
        return _emplace<false>(x, y);
    }

    // 要素が増えたか
    bool emplace_replace(T x, U y) {
        return _emplace<true>(x, y);
    }

    // 削除できたか
    bool erase(T x) {
        bool res = false;
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v) return nullptr;
            if (x < v->x.min()) {
                v->l = dfs(dfs, v->l);
            } else if (x > v->x.max()) {
                v->r = dfs(dfs, v->r);
            } else {
                res = v->x.erase(x);
                if (v->x.size == 0) {
                    if (!v->r || !v->l) {
                        return (!v->r ? v->l : v->r);
                    } else {
                        node *u = nullptr;
                        node *r = cut_leftmost(v->r, u);
                        u->l = v->l;
                        u->r = r;
                        update(u);
                        return balance(u);
                    }
                }
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
        return res;
    }

    std::pair<bool, U> find(T x) {
        node *v = root;
        while (v) {
            if (x < v->x.min()) {
                v = v->l;
            } else if(x > v->x.max()) {
                v = v->r;
            } else {
                return v->x.find(x);
            }
        }
        return {false, U()};
    }
    // 見つけたら値を返す前に f(値)を実行する
    template<typename F>
    std::pair<bool, U> find(T x, F f) {
        node *v = root;
        while (v) {
            if (x < v->x.min()) {
                v = v->l;
            } else if(x > v->x.max()) {
                v = v->r;
            } else {
                return v->x.find(x, f);
            }
        }
        return {false, U()};
    }

    // x未満の最大要素 (ない場合はfalse)
    std::tuple<bool, T> lt(T x) {
        T res = x;
        node *v = root;
        while (v) {
            if (x <= v->x.min()) {
                v = v->l;
            } else if(x > v->x.max()) {
                res = v->x.max();
                v = v->r;
            } else {
                int idx = v->x.lb(x);
                assert(idx);
                return {true, v->x.get(idx - 1).first};
            }
        }
        if (res == x) return {false, x};
        return {true, res};
    }

    // x以下の最大要素 (ない場合はfalse)
    std::pair<bool, T> le(T x) {
        return lt(x + 1);
    }

    // xより大きい最小要素 (ない場合はfalse)
    std::pair<bool, T> gt(T x) {
        T res = x;
        node *v = root;
        while (v) {
            if (x < v->x.min()) {
                res = v->x.min();
                v = v->l;
            } else if(x >= v->x.max()) {
                v = v->r;
            } else {
                int idx = v->x.ub(x);
                assert(idx != v->x.size);
                return {true, v->x.get(idx).first};
            }
        }
        if (res == x) return {false, x};
        return {true, res};
    }

    // x以上の最小要素 (ない場合はfalse)
    std::pair<bool, T> ge(T x) {
        return gt(x - 1);
    }

    // k番目に小さい要素 (ない場合はfalse)
    std::pair<bool, T> kth_smallest(int k) {
        if (size() <= k) return {false, T()};
        node *v = root;
        while (true) {
            int lsize = size(v->l);
            if (k < lsize) {
                v = v->l;
            } else if(k < lsize + v->x.size) {
                return {true, v->x.get(k - lsize).first};
            } else {
                k -= lsize + v->x.size;
                v = v->r;
            }
        }
    }
    
    // r未満の要素の数
    int rank(T r) {
        node *v = root;
        int res = 0;
        while (v) {
            if (r <= v->x.min()) {
                v = v->l;
            } else if(r <= v->x.max() + 1) {
                res += size(v->l);
                return res + v->x.lb(r);
            } else {
                res += size(v->l) + v->x.size;
                v = v->r;
            }
        }
        return res;
    }

    std::vector<std::pair<T, U>> to_list() {
        if (!root) return {};
        std::vector<std::pair<T, U>> res;
        auto dfs = [&](auto &&dfs, node *v) -> void {
            if (v->l) dfs(dfs, v->l);
            for (int i = 0; i < v->x.size; i++) {
                res.push_back(v->x.get(i));
            }
            if (v->r) dfs(dfs, v->r);
        };
        dfs(dfs, root);
        return res;
    }
};
#endif

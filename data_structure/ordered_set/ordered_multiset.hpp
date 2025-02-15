#ifndef _ORDERED_MULTISET_H_
#define _ORDERED_MULTISET_H_
#include <vector>
#include <iostream>
#include <bitset>
#include <cassert>
#include <algorithm>
#include "base/ordered_multiset_small.hpp"

template<typename T>
struct ordered_multiset {
  private:
    struct node {
        int h, _size;
        ordered_multiset_small<T> x;
        node *l, *r;
        node() : h(1), _size(0), l(nullptr), r(nullptr) {}
        node(ordered_multiset_small<T> x) : h(1), _size(x.size), x(x), l(nullptr), r(nullptr) {}
        int balanace_factor() {
            return (l ? l->h : 0) - (r ? r->h : 0);
        }
    };

    static int size(node *v) { return v ? v->_size : 0; }

    static void update(node *v) {
        v->h = std::max(v->l ? v->l->h : 0, v->r ? v->r->h : 0) + 1;
        v->_size = v->x.size;
        if (v->l) v->_size += v->l->_size;
        if (v->r) v->_size += v->r->_size;
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

  public:
    node *root;
    ordered_multiset() : root(nullptr) {}
    // ソート済
    ordered_multiset(const std::vector<T> &val) : root(nullptr) {
        if (val.empty()) return;
        std::vector<node*> nodes;
        int N = val.size();
        int B = ordered_multiset_small<T>::max_size / 2;
        int M = (N + B - 1) / B;
        for (int i = 0; i < M; i++) {
            int l = B * i, r = std::min(N, l + B);
            ordered_multiset_small<T> x;
            for (int j = l; j < r; j++) {
                assert(!j || val[j - 1] <= val[j]);
                x.val[j - l] = val[j];
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

    void insert(T x) {
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v) {
                v = new node();
                v->x.val[0] = x;
                v->x.size = 1;
                update(v);
                return v;
            }
            if (v->l && x < v->x.min()) {
                v->l = dfs(dfs, v->l);
            } else if (v->r && x > v->x.max()) {
                v->r = dfs(dfs, v->r);
            } else {
                v->x.insert(x);
                if (v->x.size == ordered_multiset_small<T>::max_size) {
                    node *u = new node(v->x.split_half());
                    update(u);
                    v->r = insert_leftmost(v->r, u);   
                }
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
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

    bool find(T x) {
        node *v = root;
        while (v) {
            if (x < v->x.min()) {
                v = v->l;
            } else if(x > v->x.max()) {
                v = v->r;
            } else {
                int idx = v->x.lb(x);
                return idx < v->x.size && v->x.get(idx) == x;
            }
        }
        return false;
    }

    // x未満の最大要素 (ない場合はfalse)
    std::pair<bool, T> lt(T x) {
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
                return {true, v->x.get(idx - 1)};
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
                return {true, v->x.get(idx)};
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
                return {true, v->x.get(k - lsize)};
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
            } else if(r > v->x.max()) {
                res += size(v->l) + v->x.size;
                v = v->r;
            }else {
                return res + size(v->l) + v->x.lb(r);
            }
        }
        return res;
    }

    std::vector<T> to_list() {
        if (!root) return {};
        std::vector<T> res;
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

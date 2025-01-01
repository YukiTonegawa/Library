#ifndef _DYNAMIC_BITVECTOR_H_
#define _DYNAMIC_BITVECTOR_H_
#include <vector>
#include <iostream>
#include <bitset>
#include <cassert>
#include <algorithm>
#include "dynamic_bitvector_small.hpp"

struct dynamic_bitvector {
  private:
    struct node {
        int h, _size, _pop;
        dynamic_bitvector_small bit;
        node *l, *r;
        node() : h(1), _size(0), _pop(0), bit(), l(nullptr), r(nullptr) {}
        node(dynamic_bitvector_small x) : h(1), _size(x.size), _pop(x.pop), bit(x), l(nullptr), r(nullptr) {}
        int balanace_factor() {
            return (l ? l->h : 0) - (r ? r->h : 0);
        }
    };

    static int size(node *v) { return v ? v->_size : 0; }
    static int pop(node *v) { return v ? v->_pop : 0; }

    static void update(node *v) {
        v->h = std::max(v->l ? v->l->h : 0, v->r ? v->r->h : 0) + 1;
        v->_size = v->bit.size;
        v->_pop = v->bit.pop;
        if (v->l) {
            v->_size += v->l->_size;
            v->_pop += v->l->_pop;
        }
        if (v->r) {
            v->_size += v->r->_size;
            v->_pop += v->r->_pop;
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

  public:
    node *root;
    dynamic_bitvector(): root(nullptr) {}
    dynamic_bitvector(const std::vector<bool> &bits) : root(nullptr) {
        if (bits.empty()) return;
        std::vector<node*> nodes;
        int N = bits.size();
        int B = dynamic_bitvector_small::max_size / 2;
        int M = (N + B - 1) / B;
        for (int i = 0; i < M; i++) {
            int l = B * i, r = std::min(N, l + B);
            dynamic_bitvector_small bit;
            for (int j = l; j < r; j++) bit.insert(j - l, bits[j]);
            nodes.push_back(new node(bit));
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

    int popcount() {
        return pop(root);
    }

    // [0, r)の1の数
    int rank(int r) {
        if (!root) return 0;
        node *v = root;
        int res = 0;
        while (v) {
            int lsz = size(v->l);
            if (r < lsz) {
                v = v->l;
            } else {
                int msz = lsz + v->bit.size;
                res += pop(v->l);
                if (r < msz) return res + v->bit.rank(r - lsz);
                res += v->bit.pop;
                r -= msz;
                v = v->r;
            }
        }
        return res;
    }
    
    int select(int k) {
        assert(0 <= k);
        if (k >= popcount()) return -1;
        node *v = root;
        int ans = 0;
        while (true) {
            int lpop = pop(v->l);
            int mpop = lpop + v->bit.pop;
            if (k < lpop) {
                v = v->l;
            } else if (mpop <= k) {
                ans += size(v->l) + v->bit.size;
                k -= mpop;
                v = v->r;
            } else {
                ans += size(v->l) + v->bit.select(k - lpop);
                return ans;
            }
        }
    }
    
    int select_zero(int k){
        assert(0 <= k);
        if (k >= size() - popcount()) return -1;
        node *v = root;
        int ans = 0;
        while (true) {
            int lpop = size(v->l) - pop(v->l);
            int mpop = lpop + v->bit.size - v->bit.pop;
            if (k < lpop) {
                v = v->l;
            } else if (mpop <= k) {
                ans += size(v->l) + v->bit.size;
                k -= mpop;
                v = v->r;
            } else {
                ans += size(v->l) + v->bit.select_zero(k - lpop);
                return ans;
            }
        }
    }
  
    void set(int k, bool f) {
        assert(0 <= k && k < size());
        auto dfs = [&](auto &&dfs, node *v) -> void {
            int lsz = size(v->l);
            int msz = lsz + v->bit.size;
            if (k < lsz) {
                dfs(dfs, v->l);
            } else if (k >= msz) {
                k -= msz;
                dfs(dfs, v->r);
            } else {
                v->bit.set(k - lsz, f);
            }
            update(v);
        };
        dfs(dfs, root);
    }

    bool get(int k) {
        assert(0 <= k && k < size());
        node *v = root;
        while (true) {
            int lsz = size(v->l);
            if (k < lsz) {
                v = v->l;
            } else {
                int msz = lsz + v->bit.size;
                if (k < msz) return v->bit.get(k - lsz);
                k -= msz;
                v = v->r;
            }
        }
    }

    void insert(int k, bool f) {
        assert(0 <= k && k <= size());
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v) {
                v = new node();
                v->bit.insert(0, f);
                update(v);
                return v;
            }
            int lsz = size(v->l);
            int msz = lsz + v->bit.size;
            if (k < lsz) {
                v->l = dfs(dfs, v->l);
            } else if (k > msz) {
                k -= msz;
                v->r = dfs(dfs, v->r);
            } else {
                v->bit.insert(k - lsz, f);
                if (v->bit.size == dynamic_bitvector_small::max_size) {
                    node *u = new node(v->bit.split_half());
                    update(u);
                    v->r = insert_leftmost(v->r, u);
                }
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
    }

    bool erase(int k) {
        assert(0 <= k && k < size());
        bool res;
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            int lsz = size(v->l);
            int msz = lsz + v->bit.size;
            if (k < lsz) {
                v->l = dfs(dfs, v->l);
            } else if (k >= msz) {
                k -= msz;
                v->r = dfs(dfs, v->r);
            } else {
                res = v->bit.erase(k - lsz);
                if (v->bit.size == 0) {
                    if (!v->r) {
                        return v->l;
                    } else {
                        node *u;
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

    std::vector<bool> to_list() {
        if (!root) return {};
        std::vector<bool> res;
        auto dfs = [&](auto &&dfs, node *v) -> void {
            if (v->l) dfs(dfs, v->l);
            for (int i = 0; i < v->bit.size; i++) {
                res.push_back(v->bit.get(i));
            }
            if (v->r) dfs(dfs, v->r);
        };
        dfs(dfs, root);
        return res;
    }
};
#endif

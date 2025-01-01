#ifndef _DS_RBT_H_
#define _DS_RBT_H_
#include "../base/stovector.hpp"
#include "../base/rolling_hash.hpp"

template<int MAX_LEN = 2000000>
struct ds_rbt {
  private:
    using rh = rolling_hash_base<MAX_LEN>;
    using ull = uint64_t;
    using self_t = ds_rbt<MAX_LEN>;
    struct node {
        node *l, *r;
        bool red;
        int ra, sz;
        ull sum;
        node(ull x) : l(nullptr), r(nullptr), red(false), ra(0), sz(1), sum(x) {}
        node(node *l, node *r, bool red) : l(l), r(r), red(red), ra(l->ra + !(l->red)), sz(l->sz + r->sz), sum(rh::concat(l->sz, l->sum, r->sum)) {}
    };
    static std::vector<node*> stock;
    static node *reuse(node *a, node *l, node *r, bool red) {
        *a = node(l, r, red);
        return a;
    }
    node *root;
    using pn = std::pair<node*, node*>;
    static node *merge_sub(node *a, node *b) {
        if (a->ra < b->ra) {
            node *c = merge_sub(a, b->l);
            if (!b->red && c->red && c->l->red) {
                if (!b->r->red) {
                    return reuse(c, c->l, reuse(b, c->r, b->r, 1), 0);
                } else {
                    b->r->red = 0;
                    c->red = 0;
                    return reuse(b, c, b->r, 1);
                }
            } else {
                return reuse(b, c, b->r, b->red);
            }
        } else if (a->ra > b->ra) {
            node *c = merge_sub(a->r, b);
            if (!a->red && c->red && c->r->red) {
                if (!a->l->red) {
                    return reuse(c, reuse(a, a->l, c->l, 1), c->r, 0);
                } else {
                    a->l->red = 0;
                    c->red = 0;
                    return reuse(a, a->l, c, 1);
                }
            } else {
                return reuse(a, a->l, c, a->red);
            }
        } else {
            if (stock.empty()) return new node(a, b, 1);
            node *u = stock.back();
            stock.pop_back();
            return reuse(u, a, b, 1);
        }
    }
    static node *merge(node *a, node *b) {
        if (!a || !b) return !a ? b : a;
        node *c = merge_sub(a, b);
        c->red = 0;
        return c;
    }
    static node *as_root(node *a) {
        if (!a) return nullptr;
        a->red = 0;
        return a;
    }
    static pn split(node *a, int k) {
        int sz = a->sz, szl = (a->l ? a->l->sz : 0);
        if (k == 0 || k == sz) return (!k ? pn{nullptr, a} : pn{a, nullptr});
        pn res;
        if (k < szl) {
            auto [l, r] = split(a->l, k);
            res = pn{l, merge(r, as_root(a->r))};
        } else if (k > szl) {
            auto [l, r] = split(a->r, k - szl);
            res = pn{merge(as_root(a->l), l), r};
        } else { 
            res = pn{as_root(a->l), as_root(a->r)};
        }
        if (a) stock.push_back(a);
        return res;
    }
    static void set(node *a, int k, ull x) {
        if (!a->l && !a->r) {
            a->sum = x;
            return;
        }
        int szl = a->l ? a->l->sz : 0;
        if (k < szl) set(a->l, k, x);
        else set(a->r, k - szl, x);
        a->sum = rh::concat(szl, a->l->sum, a->r->sum);
    }
    static ull get(node *a, int k) {
        if (!a->l && !a->r) {
            assert(k == 0);
            return a->sum;
        }
        int szl = a->l ? a->l->sz : 0;
        if (k < szl) return get(a->l, k);
        else return get(a->r, k - szl);
    }
    static ull hash_range(node *a, int l, int r) {
        if (!a || l >= r || a->sz <= l || r <= 0) return 0;
        if (l <= 0 && a->sz <= r) return a->sum;
        if (!a->l && !a->r) return a->sum;
        int szl = a->l->sz;
        if (r <= szl) return hash_range(a->l, l, r);
        if (szl <= l) return hash_range(a->r, l - szl, r - szl);
        return rh::concat(szl - l, hash_range(a->l, l, szl), hash_range(a->r, 0, r - szl));
    }
    template<typename T>
    static node *build(const std::vector<T> &v, int l, int r) {
        if (l == r) return nullptr;
        if (r - l == 1) return new node(rh::to_hash(v[l]));
        int mid = (l + r) / 2;
        node *L = build(v, l, mid);
        node *R = build(v, mid, r);
        return merge(L, R);
    }
    static int lcp(node *v, node *u, int l1, int l2) {
        if (!v || l2 >= (u ? u->sz : 0)) return 0;
        if (l1 == 0 && l2 + v->sz <= (u ? u->sz : 0)) {
            ull uh = hash_range(u, l2, l2 + v->sz);
            if (v->sum == uh) return v->sz;
        }
        int szl = v->l ? v->l->sz : 0;
        if (szl <= l1) return lcp(v->r, u, l1 - szl, l2);
        int left_cross = szl - l1, L = lcp(v->l, u, l1, l2);
        if (L != left_cross) return L;
        return L + lcp(v->r, u, 0, l2 + left_cross);
    }
    ds_rbt(node *a) : root(a) {}

  public:
    ds_rbt() : root(nullptr) {}
    template<typename T>
    ds_rbt(const std::vector<T> &v) : root(build(v, 0, v.size())) {}
    ds_rbt(const std::string &S) : ds_rbt(stovector(S)) {}

    int size() const {
        return root ? root->sz : 0;
    }

    template<typename T>
    void set(int k, T x) {
        assert(k < size());
        set(root, k, rh::to_hash(x));
    }
    
    ull get(int k) const {
        assert(k < size());
        return get(root, k);
    }

    // k番目にxを挿入
    template<typename T>
    void insert(int k, T x) {
        auto [a, b] = split(root, k);
        root = merge(a, merge(new node(rh::to_hash(x)), b));
    }
  
    void erase(int k) {
        assert(root->sz > k);
        auto [a, b] = split(root, k);
        assert(b);
        auto [b2, c] = split(b, 1);
        root = merge(a, c);
        if (b2) stock.push_back(b2);
    }

    ull hash_range(int l, int r) const {
        assert(0 <= l && r <= size());
        return hash_range(root, l, r);
    }
    
    // [0, k), [l, N)に分割 永続でないためaのrootはnullptrになる
    static std::pair<self_t, self_t> split(self_t &a, int k) {
        assert(k <= a.size());
        auto [l, r] = split(a.root, k);
        a.root = nullptr;
        return {self_t(l), self_t(r)};
    }
    
    // aージ 永続でないためa, bのrootはnullptrになる
    static self_t merge(self_t &a, self_t &b) {
        self_t res(merge(a.root, b.root));
        a.root = b.root = nullptr;
        return res;
    }
    
    static int lcp(const self_t &a, const self_t &b, int l1, int l2) {
        return lcp(a.root, b.root, l1, l2);
    }
};
template<int MAX_LEN>
std::vector<typename ds_rbt<MAX_LEN>::node*> ds_rbt<MAX_LEN>::stock;
#endif
#ifndef _DS_AVL_H_
#define _DS_AVL_H_
#include "../base/rolling_hash.hpp"
#include "../base/stovector.hpp"

template<size_t MAX_LEN = 2000000>
struct ds_avl {
  private:
    using rh = rolling_hash_base<MAX_LEN>;
    using ull = uint64_t;
    using self_t = ds_avl<MAX_LEN>;
    struct node {
        int h, sz;
        ull x, sum;
        node *l, *r;
        template<typename T>
        node(T _x) : h(1), sz(1), x(rh::to_hash(_x)), sum(x), l(nullptr), r(nullptr) {}
        int balanace_factor() const { return (l ? l->h : 0) - (r ? r->h : 0); }
    };
    node *root, *tmp_node;
    static int size(node *v) { return v ? v->sz : 0; }
    static void update(node *v) {
        v->h = std::max(v->l ? v->l->h : 0,  v->r ? v->r->h : 0) + 1;
        v->sz = 1;
        v->sum = v->x;
        if (v->l) {
            v->sum = rh::concat(v->l->sz, v->l->sum, v->sum);
            v->sz += v->l->sz;
        }
        if (v->r) {
            v->sum = rh::concat(v->sz, v->sum, v->r->sum);
            v->sz += v->r->sz;
        }
    }
    static node *rotate_right(node *v) {
        node *l = v->l;
        v->l = l->r;
        l->r = v;
        update(v);
        update(l);
        return l;
    }
    static node *rotate_left(node *v) {
        node *r = v->r;
        v->r = r->l;
        r->l = v;
        update(v);
        update(r);
        return r;
    }
    static node *build(const std::vector<node*> &nodes, int l, int r) {
        int m = (l + r) >> 1;
        node *v = nodes[m];
        if (m > l) v->l = build(nodes, l, m);
        if (r > m + 1) v->r = build(nodes, m + 1, r);
        update(v);
        return v;
    }
    static node *balance(node *v) {
        int bf = v->balanace_factor();
        assert(-2 <= bf && bf <= 2);
        if (bf == 2) {
            if (v->l->balanace_factor() == -1) v->l = rotate_left(v->l);
            return rotate_right(v);
        } else if (bf == -2) {
            if (v->r->balanace_factor() == 1) v->r = rotate_right(v->r);
            return rotate_left(v);
        }
        return v;
    }
    node *cut_leftmost(node *v) {
        if (v->l) {
            v->l = cut_leftmost(v->l);
            update(v);
            return balance(v);
        }
        tmp_node = v;
        return v->r;
    }
    node *cut_rightmost(node *v) {
        if (v->r) {
            v->r = cut_rightmost(v->r);
            update(v);
            return balance(v);
        }
        tmp_node = v;
        return v->l;
    }
    void set_inner(node *v, int k, ull x) {
        int szl = v->l ? v->l->sz : 0;
        if (k < szl) set_inner(v->l, k, x);
        else if (k > szl) set_inner(v->r, k - szl - 1, x);
        else v->x = x;
        update(v);
    }
    static ull get_inner(node *v, int k) {
        assert(0 <= k && k < v->sz);
        while (true) {
            int szl = v->l ? v->l->sz : 0;
            int szr = v->r ? v->r->sz : 0;
            assert(v->sz = szl + szr + 1);
            if (k < szl) {
                v = v->l;
            } else if (k > szl) {
                v = v->r;
                k -= szl + 1;
            } else return v->x;
        }
    }
    static ull hash_inner(node *v, int l, int r) {
        if (!v) return 0;
        if (l == 0 && r == v->sz) return v->sum;
        int szl = size(v->l), szv = szl + 1;
        if (r <= szl) return hash_inner(v->l, l, r);
        if (szv <= l) return hash_inner(v->r, l - szv, r - szv);
        ull ql = hash_inner(v->l, l, szl), qr = hash_inner(v->r, 0, r - szv);
        ull sum = rh::concat(szl - l, ql, v->x);
        sum = rh::concat(szl - l + 1, sum, qr);
        return sum;
    }
    node *insert_inner(node *v, int k, ull x) {
        assert(size() >= k);
        if (!v) return new node(x);
        int szl = v->l ? v->l->sz : 0;
        if (k <= szl) v->l = insert_inner(v->l, k, x);
        else if (k > szl) v->r = insert_inner(v->r, k - szl - 1, x);
        update(v);
        return balance(v);
    }
    node *erase_inner(node *v, int k) {
        assert(0 <= k && k < size());
        int szl = v->l ? v->l->sz : 0;
        if (k < szl) v->l = erase_inner(v->l, k);
        else if (k > szl) v->r = erase_inner(v->r, k - szl - 1);
        else {
            if (!v->r) return v->l;
            node *u = cut_leftmost(v->r);
            tmp_node->l = v->l;
            tmp_node->r = u;
            v = tmp_node;
        }
        update(v);
        return balance(v);
    }
  
    static int lcp_inner(node *v, node *u, int l1, int l2) {
        if (!v || l2 >= u->sz) return 0;
        while (u) {
            int szu = (u->l ? u->l->sz : 0) + 1;
            if (szu <= l2) {
                u = u->r;
                l2 -= szu;
            } else break;
        }
        if (!u) return 0;
        if (l1 == 0 && l2 + v->sz <= u->sz && v->sum == hash_inner(u, l2, l2 + v->sz)) return v->sz;
        int szl = v->l ? v->l->sz : 0;
        if (szl + 1 <= l1) return lcp_inner(v->r, u, l1 - (szl + 1), l2);
        int left_cross = szl - l1, L = lcp_inner(v->l, u, l1, l2);
        if (L != left_cross) return L;
        l2 += left_cross;
        while (u) {
            int szu = (u->l ? u->l->sz : 0) + 1;
            if (szu <= l2) {
                u = u->r;
                l2 -= szu;
            }else break;
        }
        if (!u || l2 >= u->sz || v->x != get_inner(u, l2)) return L;
        return L + 1 + lcp_inner(v->r, u, 0, l2 + 1);
    }

  public:
    ds_avl() : root(nullptr) {}
    ds_avl(const std::string &S) : ds_avl(stovector(S)){}
    template<typename T>
    ds_avl(const std::vector<T> &v) {
        if (v.empty()) {
            root = nullptr;
            return;
        }
        int n = v.size();
        std::vector<node*> nodes(n);
        for (int i = 0; i < n; i++) nodes[i] = new node(v[i]);
        root = build(nodes, 0, n);
    }

    
    int size() const {
        return size(root);
    }

    template<typename T>
    void set(int k, T x) {
        assert(0 <= k && k < size());
        set_inner(root, k, rh::to_hash(x));
    }
    
    ull get(int k) const {
        assert(0 <= k && k < size());
        return get_inner(root, k);
    }

    void insert(int k, ull x) {
        assert(0 <= k && k <= size());
        root = insert_inner(root, k, x);
    }

    void erase(int k) {
        assert(0 <= k && k < size());
        root = erase_inner(root, k);
    }

    ull hash_range(int l, int r) const {
        assert(0 <= l && r <= size());
        if (l >= r) return 0;
        return hash_inner(root, l, r);
    }

    ull hash_all() const {
        if (!root) return 0;
        return root->sum;
    }

    static int lcp(const self_t &a, const self_t &b, int l1, int l2) {
        return lcp_inner(a.root, b.root, l1, l2);
    }
};
#endif
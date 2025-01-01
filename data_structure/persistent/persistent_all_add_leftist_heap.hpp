#ifndef _PERSISTENT_ALL_ADD_LEFTIST_HEAP_H_
#define _PERSISTENT_ALL_ADD_LEFTIST_HEAP_H_
#include <algorithm>
#include <vector>
#include <cassert>

// std::less 最小値
// std::greater 最大値
template<typename T, typename cmp = std::less<T>>
struct persistent_all_add_leftist_heap {
    using self_t = persistent_all_add_leftist_heap<T, cmp>;
    struct node {
        int s;
        node *l, *r;
        T x, lz;
        node(T _x) : s(1), l(nullptr), r(nullptr), x(_x), lz(0) {}
    };
  
  private:
    static node *meld(node *a, node *b, T lza, T lzb) {
        if (!a || !b) {
            if (!a) {
                std::swap(a, b);
                std::swap(lza, lzb);
            }
            if (lza == 0) return a;
            a = new node(*a);
            a->lz += lza;
            a->x += lza;
            return a;
        }
        if (!cmp()(a->x + lza, b->x + lzb)) {
            std::swap(a, b);
            std::swap(lza, lzb);
        }
        a = new node(*a);
        a->x += lza;
        a->lz += lza;
        a->r = meld(a->r, b, 0, lzb - a->lz);
        int ls = (!a->l ? 0 : a->l->s);
        int rs = a->r->s;
        if (ls < rs) std::swap(a->l, a->r);
        a->s = std::max(ls, rs) + 1;
        return a;
    }
    
    node *root;
    int sz;

    persistent_all_add_leftist_heap(node *_root, int _sz) : root(_root), sz(_sz) {}
  public:
    persistent_all_add_leftist_heap() : root(nullptr), sz(0) {}

    int size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    self_t clear() const {
        return self_t();
    }

    self_t all_add(T x) const {
        if (!root) return self_t();
        node *v = new node(*root);
        v->x += x;
        v->lz += x;
        return self_t(v, sz);
    }

    self_t push(T x) const {
        node *v = meld(root, new node(x), 0, 0);
        return self_t(v, sz + 1);
    }

    T top() const {
        assert(!empty());
        return root->x;
    }
    
    self_t pop() const {
        assert(!empty());
        node *v = meld(root->l, root->r, root->lz, root->lz);
        return self_t(v, sz - 1);
    }

    // rをマージ
    self_t meld(const self_t &r) const {
        int s = sz + r.size();
        node *v = meld(root, r.root, 0, 0);
        return self_t(v, s);
    }
};
#endif
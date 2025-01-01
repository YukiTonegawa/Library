#ifndef _ALL_ADD_LEFTIST_HEAP_H_
#define _ALL_ADD_LEFTIST_HEAP_H_
#include <vector>
#include <cassert>
#include <functional>
#include <algorithm>

// std::less 最小値
// std::greater 最大値
template<typename T, typename cmp = std::less<T>>
struct all_add_leftist_heap {
  private:
    using self_t = all_add_leftist_heap<T, cmp>;
    struct node {
        int s;
        node *l, *r;
        T x, lz;
        node(T _x) : s(1), l(nullptr), r(nullptr), x(_x), lz(0) {}
    };
    static void push_down(node *v) {
        if (v->lz) {
            T lz = v->lz;
            if (v->l) {
                v->l->x += lz;
                v->l->lz += lz;
            }
            if (v->r) {
                v->r->x += lz;
                v->r->lz += lz;
            }
            v->lz = 0;
        }
    }

    static node *meld(node *a, node *b) {
        if (!a) return b;
        if (!b) return a;
        if (!cmp()(a->x, b->x)) std::swap(a, b);
        push_down(a);
        a->r = meld(a->r, b);
        int ls = (!a->l ? 0 : a->l->s);
        int rs = a->r->s;
        if (ls < rs) std::swap(a->l, a->r);
        a->s = std::max(ls, rs) + 1;
        return a;
    }
    
    node *root;
    int sz;
  
  public:
    all_add_leftist_heap() : root(nullptr), sz(0) {}

    int size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    void clear() {
        root = nullptr;
        sz = 0;
    }

    void all_add(T x) {
        if (root) {
            root->x += x;
            root->lz += x;
        }
    }

    void push(T x) {
        root = meld(root, new node(x));
        sz++;
    }

    T top() const {
        assert(!empty());
        return root->x;
    }

    void pop() {
        assert(!empty());
        push_down(root);
        root = meld(root->l, root->r);
        sz--;
    }

    // rをマージ(rは空になる)
    void meld(self_t &r) {
        assert(root == nullptr || root != r.root);
        sz += r.size();
        root = meld(root, r.root);
        r.clear();
    }
};
#endif
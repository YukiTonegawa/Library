#ifndef _ALL_ADD_LEFTIST_HEAP_MAP_H_
#define _ALL_ADD_LEFTIST_HEAP_MAP_H_
#include <vector>
#include <cassert>
#include <functional>
#include <algorithm>

// std::less 最小値
// std::greater 最大値
template<typename T, typename V, typename cmp = std::less<T>>
struct all_add_leftist_heap_map {
  private:
    using self_t = all_add_leftist_heap_map<T, V, cmp>;
    struct node {
        int s;
        node *l, *r;
        T x, lz;
        V y;
        node(T _x, V _y) : s(1), l(nullptr), r(nullptr), x(_x), lz(0), y(_y) {}
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
    all_add_leftist_heap_map() : root(nullptr), sz(0) {}

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

    void push(T x, V y) {
        root = meld(root, new node(x, y));
        sz++;
    }

    std::pair<T, V> top() const {
        assert(!empty());
        return {root->x, root->y};
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
#ifndef _MULTISET_SUM_H_
#define _MULTISET_SUM_H_
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>

// キーに対して加算できる
// 値の範囲の絶対値がA以内, 一回の操作で追加される要素数がB以内の場合, Q回の操作で総和はABQ^2以内になる
template<typename S>
struct multiset_sum {
    static constexpr S inf = std::numeric_limits<S>::max();
  private:
    struct node {
        S sz_sum, cnt;
        S key, sum, lazy;
        node *l, *r, *p;
        node(S _key, S _cnt) : sz_sum(_cnt), cnt(_cnt), key(_key), sum(_key * cnt), lazy(0), l(nullptr), r(nullptr), p(nullptr) {}
    };
    S size(node *v) { return v ? v->sz_sum : 0; }
    node *root;

    void push_down(node *v) {
        if (v && v->lazy) {
            if (v->l) propagate(v->l, v->lazy);
            if (v->r) propagate(v->r, v->lazy);
            v->lazy = 0;
        }
    }
    void propagate(node *v, S x) {
        v->lazy += x;
        v->key += x;
        v->sum += v->sz_sum * x;
    }
    void update(node *v) {
        v->sz_sum = v->cnt;
        v->sum = v->key * v->cnt;
        if (v->l) {
            v->sz_sum += v->l->sz_sum;
            v->sum += v->l->sum;
        }
        if (v->r) {
            v->sz_sum += v->r->sz_sum;
            v->sum += v->r->sum;
        }
    }
    void rotate_right(node *v) {
        node *p = v->p, *pp = p->p;
        if ((p->l = v->r)) v->r->p = p;
        v->r = p, p->p = v;
        update(p), update(v);
        if ((v->p = pp)) {
            if (pp->l == p) pp->l = v;
            if (pp->r == p) pp->r = v;
            update(pp);
        }
    }
    void rotate_left(node *v) {
        node *p = v->p, *pp = p->p;
        if ((p->r = v->l)) v->l->p = p;
        v->l = p, p->p = v;
        update(p), update(v);
        if ((v->p = pp)) {
            if (pp->l == p) pp->l = v;
            if (pp->r == p) pp->r = v;
            update(pp);
        }
    }
    node *splay(node *v) {
        if (!v) return v;
        push_down(v);
        while (v->p) {
            node *p = v->p;
            if (!p->p) {
                push_down(p), push_down(v);
                if (p->l == v) rotate_right(v);
                else rotate_left(v);
            } else {
                node *pp = p->p;
                push_down(pp), push_down(p), push_down(v);
                if (pp->l == p) {
                    if (p->l == v) rotate_right(p);
                    else rotate_left(v);
                    rotate_right(v);
                } else {
                    if (p->r == v) rotate_left(p);
                    else rotate_right(v);
                    rotate_left(v);
                }
            }
        }
        return v;
    }
    node *leftmost(node *v) {
        push_down(v);
        while (v->l) {
            push_down(v);
            v = v->l;
        }
        return splay(v);
    }
    node *rightmost(node *v) {
        push_down(v);
        while (v->r) {
            push_down(v);
            v = v->r;
        }
        return splay(v);
    }
    node *lower_bound_inner(S k) {
        node *v = root, *ret = nullptr;
        while (v) {
            push_down(v);
            if (k < v->key) {
                ret = v;
                v = v->l;
            } else if (k > v->key) {
                if (!ret && !v->r) return splay(v); // 全てのキーがk未満の場合
                v = v->r;
            } else return splay(v);
        }
        return splay(ret);
    }
    node *kth_key_inner(S k) {
        node *v = root;
        while (v) {
            push_down(v);
            S szl = size(v->l);
            if (k < szl) {
                v = v->l;
            } else if (k >= szl + v->cnt) {
                k -= szl + v->cnt;
                v = v->r;
            } else return splay(v);
        }
        return nullptr;
    }
    node *insert_inner(S k, S num) {
        if (!root) return new node(k, num);
        node *v = lower_bound_inner(k);
        push_down(v);
        if (v->key < k) { // 全てのキーがk未満の場合
            node *u = new node(k, num);
            u->l = v, v->p = u;
            update(u);
            return u;
        } else if (v->key == k) {
            v->cnt += num;
            update(v);
        } else {
            node *u = new node(k, num), *l = v->l;;
            v->l = u, u->p = v;
            if ((u->l = l)) l->p = u;
            update(u);
            update(v);
        }
        return v;
    }
    std::pair<node*, node*> split(S k) {
        if (k == size(root)) return {root, nullptr};
        node *v = kth_key_inner(k);
        push_down(v);
        S szl = size(v->l);
        node *l = v->l;
        if (l) l->p = nullptr;
        v->l = nullptr;
        if (szl == k) {
            update(v);
            return {l, v};
        }
        // vを分割する
        S sp = k - size(l);
        v->cnt -= sp;
        assert(v->cnt > 0);
        update(v);
        node *u = new node(v->key, sp);
        if ((u->l = l)) l->p = u;
        update(u);
        return {u, v};
    }
    node *merge(node *l, node *r) {
        if (!l || !r) return !l ? r : l;
        l = rightmost(l);
        push_down(l);
        l->r = r, r->p = l;
        update(l);
        return l;
    }
    void __enumerate(node *v, std::vector<std::pair<S, S>> &res) {
        if (!v) return;
        push_down(v);
        if (v->l) __enumerate(v->l, res);
        res.push_back({v->key, v->cnt});
        if (v->r) __enumerate(v->r, res);
    }
  public:
    multiset_sum() : root(nullptr) {}
    // 要素数
    S size() {
        return size(root);
    }
    bool empty() {
        return size(root) == 0;
    }
    void clear() {
        root = nullptr;
    }
    S min() {
        assert(size());
        root = leftmost(root);
        return root->key;
    }
    S max() {
        assert(size());
        root = rightmost(root);
        return root->key;
    }
    // [-∞, r)
    S sum_left(S r) {
        if (!root) return 0;
        root = lower_bound_inner(r);
        push_down(root);
        if (root->key < r) return root->sum; // 全てのキーがr未満
        return root->l ? root->l->sum : 0;
    }
    // [l, ∞)
    S sum_right(S l) {
        if (!root) return 0;
        root = lower_bound_inner(l);
        push_down(root);
        if (root->key < l) return 0; // 全てのキーがl未満
        return root->key * root->cnt + (root->r ? root->r->sum : 0);
    }
    // r未満の値の数(同じ要素を重複して数える)
    S low_count(S r) {
        if (!root) return 0;
        root = lower_bound_inner(r);
        push_down(root);
        if (root->key < r) return root->sz_sum; // 全てのキーがr未満
        return root->l ? root->l->sz_sum : 0;
    }
    // l以上の要素の数(同じ要素を重複して数える)
    S up_count(S l) {
        return size() - low_count(l);
    }
    void insert(S k, S num) {
        root = insert_inner(k, num);
    }
    // 左のk個を切り離す
    node *split_left(S k) {
        assert(k <= size());
        auto [a, b] = split(k);
        root = b;
        return a;
    }
    // 右のk個を切り離す
    node *split_right(S k) {
        assert(k <= size());
        auto [a, b] = split(size() - k);
        root = a;
        return b;
    }
    // 左側にvをマージする
    void merge_left(node *v) {
        root = merge(v, root);
    }
    // 右側にvをマージする
    void merge_right(node *v) {
        root = merge(root, v);
    }
    // 総和がs以上になる初のノード
    node *lower_bound_sum(S s) {
        if (!root) return nullptr;
        node *v = root, *ret = nullptr;
        S lsum = 0;
        while (v) {
            push_down(v);
            S cur_sum = lsum + (v->l ? v->l->sum : 0) + v->cnt * v->key;
            if (s <= cur_sum) {
                ret = v;
                v = v->l;
            } else if (cur_sum < s) {
                if (!ret && !v->r) return root = splay(v); // 全てのキーがk未満の場合
                lsum = cur_sum; 
                v = v->r;
            }
        }
        return root = splay(ret);
    }
    void add_all(S x) {
        if (root) propagate(root, x);
    }
    std::vector<std::pair<S, S>> enumerate() {
        std::vector<std::pair<S, S>> res;
        __enumerate(root, res);
        return res;
    }
};
#endif
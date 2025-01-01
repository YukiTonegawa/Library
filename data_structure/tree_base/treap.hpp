#ifndef _TREAP_BASE_H_
#define _TREAP_BASE_H_
#include <cassert>
#include <vector>

template<typename T>
struct treap_node {
    unsigned priority;
    treap_node<T> *lch, *rch;
    T _val;

    static unsigned xor128() {
        static unsigned x = 123456789, y = 362436069, z = 521288629, w = 86675123;
        unsigned t = (x ^ (x << 11));
        x = y, y = z, z = w;
        return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
    }

    treap_node(T x) : priority(xor128()), lch(nullptr), rch(nullptr), _val(x) {}
};

template<typename Node, void (*update)(Node*), void (*push)(Node*)>
struct treap {
    static void set_lch(Node *p, Node *c) {
        push(p);
        p->lch = c;
        update(p);
    }

    static void set_rch(Node *p, Node *c) {
        push(p);
        p->rch = c;
        update(p);
    }

    static void cut_lch(Node *p) {
        push(p);
        p->lch = nullptr;
        update(p);
    }

    static void cut_rch(Node *p) {
        push(p);
        p->rch = nullptr;
        update(p);
    }

    static Node *get_leftmost(Node *v) {
        if (!v) return nullptr;
        while (v->lch) {
            push(v);
            v = v->lch;
        }
        return v;
    }

    static Node *get_rightmost(Node *v) {
        if (!v) return nullptr;
        while (v->rch) {
            push(v);
            v = v->rch;
        }
        return v;
    }

    static Node *build(const std::vector<Node*> &v) {
        if (v.empty()) return nullptr;
        int N = v.size();
        std::vector<Node*> st;
        for (int i = 0; i < N; i++) {
            Node *last = nullptr;
            while (!st.empty() && v[i]->priority >= st.back()->priority) {
                last = st.back();
                update(last);
                st.pop_back();
            }
            v[i]->lch = last;
            if (!st.empty()) st.back()->rch = v[i];
            st.push_back(v[i]);
        }
        for (int i = (int)st.size() - 1; i >= 0; i--) update(st[i]);
        return st[0];
    }

    template<typename F>
    static void enumerate(Node *a, F f) {
        if (!a) return;
        push(a);
        if (a->lch) enumerate(a->lch, f);
        f(a);
        if (a->rch) enumerate(a->rch, f);
    }

    // 左がサイズr以下かつ最大になるように分割
    template<int (*get_size)(Node*), int (*get_sumsize)(Node*)>
    static std::pair<Node*, Node*> split_size(Node *v, int r) {
        if (!v) return {nullptr, nullptr};
        if (r <= 0) return {nullptr, v};
        if (get_sumsize(v) <= r) return {v, nullptr};
        push(v);
        int lsz = v->lch ? get_sumsize(v->lch) : 0;
        if (r <= lsz) {
            auto [a, b] = split_size<get_size, get_sumsize>(v->lch, r);
            v->lch = b;
            update(v);
            return {a, v};
        } else {
            int vsz = get_size(v);
            if (r < vsz + lsz) {
                Node *L = v->lch;
                v->lch = nullptr;
                update(v);
                return {L, v};
            }
            auto [a, b] = split_size<get_size, get_sumsize>(v->rch, r - lsz - vsz);
            v->rch = a;
            update(v);
            return {v, b};
        }
    }
    
    // r未満, r以上で分割
    template<typename Key, Key (*get_key)(Node*), Key (*get_minkey)(Node*), Key (*get_maxkey)(Node*)>
    static std::pair<Node*, Node*> split_key(Node *v, Key r) {
        if (!v) return {nullptr, nullptr};
        if (r <= get_minkey(v)) return {nullptr, v};
        if (get_maxkey(v) < r) return {v, nullptr};
        push(v);
        if (get_key(v) < r) {
            auto [a, b] = split_key<Key, get_key, get_minkey, get_maxkey>(v->rch, r);
            v->rch = a;
            update(v);
            return {v, b};
        } else {
            auto [a, b] = split_key<Key, get_key, get_minkey, get_maxkey>(v->lch, r);
            v->lch = b;
            update(v);
            return {a, v};
        }
    }

    static Node *merge(Node *a, Node *b) {
        if (!a) return b;
        if (!b) return a;
        if (a->priority > b->priority) {
            push(a);
            a->rch = merge(a->rch, b);
            update(a);
            return a;
        } else {
            push(b);
            b->lch = merge(a, b->lch);
            update(b);
            return b;
        }
    }

    template<int (*get_sumsize)(Node*)>
    static Node *merge3(Node *a, Node *b, Node *c) {
        if (!a) return merge(b, c);
        if (!c) return merge(a, b);
        if (get_sumsize(a) > get_sumsize(c)) {
            return merge(a, merge(b, c));
        } else {
            return merge(merge(a, b), c);
        }
    }

    static constexpr int _one(Node *v) { return 1; };
    // 全ノードサイズ1
    template<int (*get_sumsize)(Node*)>
    static Node *insert_size(Node *a, int k, Node *b) {
        if (!a) return b;
        push(a);
        int lsz = a->lch ? get_sumsize(a->lch) : 0;
        if (a->priority > b->priority) {
            if (k <= lsz) {
                a->lch = insert_size<get_sumsize>(a->lch, k, b);
            } else {
                a->rch = insert_size<get_sumsize>(a->rch, k - lsz - 1, b);
            }
            update(a);
            return a;
        } else {
            auto [l, r] = split_size<_one, get_sumsize>(a, k);
            b->lch = l;
            b->rch = r;
            update(b);
            return b;
        }
    }

    // 全ノードサイズ1
    template<int (*get_sumsize)(Node*)>
    static Node *erase_size(Node *a, int k) {
        push(a);
        int lsz = a->lch ? get_sumsize(a->lch) : 0;
        if (k < lsz) {
            a->lch = erase_size<get_sumsize>(a->lch, k);
        } else if (lsz == k) {
            return merge(a->lch, a->rch);
        } else {
            a->rch = erase_size<get_sumsize>(a->rch, k - lsz - 1);
        }
        update(a);
        return a;
    }

    // [l, r)が
    // ・ある頂点の部分木を完全に含む
    // ・ある頂点と交差する(全頂点のサイズが1の場合は完全に含む)
    // 上記のイベントがO(logN)回起こる
    // 前者ではf(v), 後者ではg(v, _l, _r)を呼ぶ(_l, _rはv中でのインデックス)
    template<int (*get_size)(Node*), int (*get_sumsize)(Node*), typename F, typename G>
    static void intersection(Node *v, int l, int r, F f, G g) {
        if (!v || l >= r || get_sumsize(v) <= l) return;
        if (l == 0 && get_sumsize(v) <= r) {
            f(v);
            update(v);
            return;
        }
        push(v);
        int lsz = v->lch ? get_sumsize(v->lch) : 0;
        intersection<get_size, get_sumsize>(v->lch, l, r, f, g);
        if (r <= lsz) {
            update(v);
            return;
        }
        int vsz = get_size(v); 
        {
            int _l = std::max(l - lsz, 0);
            int _r = std::min(r - lsz, vsz);
            if (_l < _r) g(v, _l, _r);
        }
        l = std::max(0, l - lsz - vsz);
        r = r - lsz - vsz;
        intersection<get_size, get_sumsize>(v->rch, l, r, f, g);
        update(v);
    }

    // 両方とも昇順でないと壊れる
    template<typename Key, Key (*get_key)(Node*), Key (*get_minkey)(Node*), Key (*get_maxkey)(Node*)>
    static Node *meld(Node *a, Node *b) {
        if (!a) return b;
        if (!b) return a;
        if (get_maxkey(a) <= get_minkey(b)) return merge(a, b);
        if (get_maxkey(b) <= get_minkey(a)) return merge(b, a);
        if (a->priority > b->priority) std::swap(a, b);
        push(b);
        auto [l, r] = split_key<Key, get_key, get_minkey, get_maxkey>(a, get_key(b));
        b->lch = meld<Key, get_key, get_minkey, get_maxkey>(l, b->lch);
        b->rch = meld<Key, get_key, get_minkey, get_maxkey>(r, b->rch);
        update(b);
        return b;
    }
};

#endif
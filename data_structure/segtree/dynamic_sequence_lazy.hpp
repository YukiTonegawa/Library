#ifndef _DYNAMIC_SEQUENCE_LAZY_H_
#define _DYNAMIC_SEQUENCE_LAZY_H_
#include "../tree_base/treap.hpp"

template<typename S, S (*op)(S, S), S (*e)(), S (*flip_val)(S), typename F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
struct dynamic_sequence_lazy {
  private:
    using self_t = dynamic_sequence_lazy<S, op, e, flip_val, F, mapping, composition, id>;
    struct V {
        S val, sum;
        F lazy;
        int size;
        bool _flip;
        V(S s) : val(s), sum(s), lazy(id()), size(1), _flip(false) {}
    };

    using node = treap_node<V>;

    static constexpr void update(node *v) {
        v->_val.sum = v->_val.val;
        v->_val.size = 1;
        if (v->lch) {
            v->_val.sum = op(v->lch->_val.sum, v->_val.sum);
            v->_val.size += v->lch->_val.size;
        }
        if (v->rch) {
            v->_val.sum = op(v->_val.sum, v->rch->_val.sum);
            v->_val.size += v->rch->_val.size;
        }
    }
    static constexpr void flip(node *v) {
        v->_val._flip = !v->_val._flip;
        std::swap(v->lch, v->rch);
        v->_val.sum = flip_val(v->_val.sum);
    }

    static constexpr void all_apply(node *v, F lz) {
        v->_val.sum = mapping(lz, v->_val.sum, v->_val.size);
        v->_val.val = mapping(lz, v->_val.val, 1);
        v->_val.lazy = composition(lz, v->_val.lazy);
    }

    static constexpr void push_down(node *v) {
        if (v->_val._flip) {
            if (v->lch) flip(v->lch);
            if (v->rch) flip(v->rch);
            v->_val._flip = false;
        }
        if (v->_val.lazy != id()) {
            if (v->lch) all_apply(v->lch, v->_val.lazy);
            if (v->rch) all_apply(v->rch, v->_val.lazy);
            v->_val.lazy = id();
        }
    }

    using T = treap<node, update, push_down>;

    node *root;
    dynamic_sequence_lazy(node *v) : root(v) {}
  public:
    static constexpr int get_size(node *v) { return 1; }
    static constexpr int get_sumsize(node *v) { return v->_val.size; }
    dynamic_sequence_lazy() : root(nullptr) {}
    dynamic_sequence_lazy(const std::vector<S> &_v) : root(nullptr) {
        int N = _v.size();
        std::vector<node*> tmp(N);
        for (int i = 0; i < N; i++) tmp[i] = new node(V(_v[i]));
        root = T::build(tmp);
    } 

    int size() {
        return root ? root->_val.size : 0;
    }

    void insert(int k, S x) {
        node *b = new node(V(x));
        root = T::template insert_size<get_sumsize>(root, k, b);
    }

    void erase(int k) {
        root = T::template erase_size<get_sumsize>(root, k);
    }
    
    void reverse(int l, int r) {
        auto [ab, c] = T::template split_size<get_size, get_sumsize>(root, r);
        auto [a, b] = T::template split_size<get_size, get_sumsize>(ab, l);
        if (b) flip(b);
        root = T::template merge3<get_sumsize>(a, b, c);
    }

    S prod(int l, int r) {
        S res = e();
        T::template intersection<get_size, get_sumsize>(root, l, r, 
            [&](node *v) { res = op(res, v->_val.sum); },
            [&](node *v, int _l, int _r) { res = op(res, v->_val.val); }
        );
        return res;
    }

    void apply(int l, int r, F f) {
        T::template intersection<get_size, get_sumsize>(root, l, r, 
            [&](node *v) { all_apply(v, f); },
            [&](node *v, int _l, int _r) { v->_val.val = mapping(f, v->_val.val, 1); }
        );
    }

    // [l, r)がtrueであるような最大のr (<= N)
    template<typename G>
    int max_right(int l, G f) {
        auto [L, R] = T::template split_size<get_size, get_sumsize>(root, l);
        node *v = R;
        assert(f(e()));
        if (!v || f(v->_val.sum)) {
            int res = (v ? v->_val.size : 0) + l;
            root = T::merge(L, R);
            return res;
        }
        int res = l;
        S lsum = e();
        while (v) {
            push_down(v);
            S lval = (v->lch ? v->lch->_val.sum : e());
            S a = op(lsum, lval);
            if (f(a)) {
                res += (v->lch ? v->lch->_val.size : 0);
                S b = op(a, v->_val.val);
                if (!f(b)) break;
                res++;
                lsum = b;
                v = v->rch;
            } else {
                v = v->lch;
            }
        }
        root = T::merge(L, R);
        return res;
    }


    // [l, r)がtrueになる最小のl (>=0)
    template<typename G>
    int min_left(int r, G f) {
        auto [L, R] = T::template split_size<get_size, get_sumsize>(root, r);
        node *v = L;
        assert(f(e()));
        if (!v || f(v->_val.sum)) {
            root = T::merge(L, R);
            return 0;
        }
        int res = 0;
        S rsum = e();
        while (v) {
            push_down(v);
            S rval = (v->rch ? v->rch->_val.sum : e());
            S a = op(rval, rsum);
            if (f(a)) {
                S b = op(v->_val.val, a);
                if (!f(b)) {
                    res += (v->lch ? v->lch->_val.size : 0) + 1;
                    break;
                }
                rsum = b;
                v = v->lch;
            } else {
                res += (v->lch ? v->lch->_val.size : 0) + 1;
                v = v->rch;
            }
        }
        root = T::merge(L, R);
        return res;
    }

    // [0, r), [r, N)に分割する(永続でないためAは空になる)
    static std::pair<self_t, self_t> split(self_t &A, int r) {
        auto [a, b] = T::template split_size<get_size, get_sumsize>(A.root, r);
        A.root = nullptr;
        return {self_t(a), self_t(b)};
    }

    // 結合(永続でないためA, Bは空になる)
    static self_t merge(self_t &A, self_t &B) {
        assert(!A.root || A.root != B.root);
        self_t res(T::merge(A.root, B.root));
        A.root = B.root = nullptr;
        return res;
    }
};
#endif
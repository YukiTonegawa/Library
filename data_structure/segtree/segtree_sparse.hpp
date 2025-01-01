#ifndef _SEGTREE_SPARSE_H_
#define _SEGTREE_SPARSE_H_
#include <cassert>
#include <numeric>

template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
struct segtree_sparse {
  private:
    struct node {
        Idx idx;
        node *l, *r;
        S val, sum;
        node(Idx idx, S val) : idx(idx), l(nullptr), r(nullptr), val(val), sum(val) {}
    };

    void update(node *v) {
        v->sum = v->val;
        if (v->l) v->sum = op(v->l->sum, v->sum);
        if (v->r) v->sum = op(v->sum, v->r->sum);
    }

    Idx mid_point(Idx l, Idx r) {
        return (l + r) / 2;
    }

    template<bool APPLY>
    node *set(node* v, Idx k, S x, Idx l, Idx r) {
        if (!v) return new node(k, x);
        if (v->idx == k) {
            if constexpr (APPLY) {
                v->val = op(v->val, x);
            } else {
                v->val = x;
            }
            update(v);
            return v;
        }

        Idx mid = mid_point(l, r);
        if (k < mid) {
            if (v->idx < k) {
                std::swap(v->idx, k);
                std::swap(v->val, x);
            }
            v->l = set<APPLY>(v->l, k, x, l, mid);
        } else {
            if (v->idx > k) {
                std::swap(v->idx, k);
                std::swap(v->val, x);
            }
            v->r = set<APPLY>(v->r, k, x, mid, r);
        }
        update(v);
        return v;
    }
  
    node *root;
    Idx L, R;

  public:
    // 可能なインデックスの範囲[L, R)
    std::pair<Idx, Idx> LR() {
        return {L, R};
    }

    segtree_sparse() : root(nullptr), L(std::numeric_limits<Idx>::min()), R(std::numeric_limits<Idx>::max()) {}
    segtree_sparse(Idx L, Idx R) : root(nullptr), L(L), R(R) {}

    void set(Idx k, S x) {
        assert(L <= k && k < R);
        root = set<false>(root, k, x, L, R);
    }

    void apply(Idx k, S x) {
        assert(L <= k && k < R);
        root = set<true>(root, k, x, L, R);
    }
    
    S get(Idx k) {
        assert(L <= k && k < R);
        node *v = root;
        Idx l = L, r = R;
        while (v) {
            if (k == v->idx) return v->val;
            Idx mid = mid_point(l, r);
            if (k < mid) {
                v = v->l;
            } else {
                v = v->r;
            }
        }
        return e();
    }

    S prod(Idx l, Idx r) {
        auto dfs = [&](auto &&dfs, node *v, Idx s, Idx t) -> S {
            if (!v || r <= s || t <= l) return e();
            if (l <= s && t <= r) return v->sum;
            Idx mid = mid_point(s, t);
            S res = dfs(dfs, v->l, s, mid);
            if (l <= v->idx && v->idx < r) res = op(res, v->val);
            return op(res, dfs(dfs, v->r, mid, t));
        };
        return dfs(dfs, root, L, R);
    }

    S all_prod() {
        return root ? root->sum : e();
    }

    // f(op(A_l, A_l+1, ... A_r-1)) = trueになる最大のrとf(op(A_l, A_l+1, ... A_r-1))
    template<typename F>
    std::pair<Idx, S> max_right(Idx l, F f) {
        assert(f(e()));
        node *res = nullptr;
        auto dfs = [&](auto &&dfs, node *v, S sum, Idx _l, Idx _r) -> S {
            if (!v || _r <= l) return sum;
            if (l <= _l && f(op(sum, v->sum))) return op(sum, v->sum);
            Idx mid = mid_point(_l, _r);
            if (v->idx < l) return dfs(dfs, v->r, sum, mid, _r);
            S lsum = dfs(dfs, v->l, sum, _l, mid);
            if (res) return lsum;
            if (!f(op(lsum, v->val))) {
                res = v;
                return lsum;
            }
            lsum = op(lsum, v->val);
            return dfs(dfs, v->r, lsum, mid, _r);
        };
        S val = dfs(dfs, root, e(), L, R);
        return {res ? res->idx : R, val};
    }
    
    // f(op(A_l, A_l+1, ... A_r-1)) = trueになる最小のlとf(op(A_l, A_l+1, ... A_r-1))
    template<typename F>
    std::pair<Idx, S> min_left(Idx r, F f) {
        assert(f(e()));
        node *res = nullptr;
        auto dfs = [&](auto &&dfs, node *v, S sum, Idx _l, Idx _r) -> S {
            if (!v || r <= _l) return sum;
            if (_r <= r && f(op(v->sum, sum))) return op(v->sum, sum);
            Idx mid = mid_point(_l, _r);
            if (v->idx >= r) return dfs(dfs, v->l, sum, _l, mid);
            S rsum = dfs(dfs, v->r, sum, mid, _r);
            if (res) return rsum;
            if (!f(op(v->val, rsum))) {
                res = v;
                return rsum;
            }
            rsum = op(v->val, rsum);
            return dfs(dfs, v->l, rsum, _l, mid);
        };
        S val = dfs(dfs, root, e(), L, R);
        return {res ? res->idx + 1 : L, val};
    }
};

template<typename Idx, typename monoid>
using segtree_sparse_monoid = segtree_sparse<Idx, typename monoid::S, monoid::op, monoid::e>;
#endif
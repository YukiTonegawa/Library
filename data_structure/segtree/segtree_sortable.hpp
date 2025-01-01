#ifndef _SEGTREE_SORTABLE_H_
#define _SEGTREE_SORTABLE_H_
#include <queue>
#include "../tree_base/treap.hpp"

template<typename Key, typename S, S (*op)(S, S), S (*e)(), S (*flip_val)(S)>
struct segtree_sortable {
  private:
    struct Vin {
        bool flip;
        Key key, minkey, maxkey;
        S val, sum;
        int size;
        Vin(Key k, S x) : flip(false), key(k), minkey(k), maxkey(k), val(x), sum(x), size(1) {}
    };

    using Nin = treap_node<Vin>;

    struct Vout {
        S sum;
        int size;
        Nin *top;
        Vout(Nin *top) : sum(top->_val.sum), size(top->_val.size), top(top) {}
    };

    using Nout = treap_node<Vout>;

    static constexpr void update_in(Nin *a) {
        Vin &v = a->_val;
        v.minkey = v.maxkey = v.key;
        v.size = 1;
        v.sum = v.val;

        if (a->lch) {
            Vin &u = a->lch->_val;
            v.minkey = std::min(v.minkey, u.minkey);
            v.maxkey = std::max(v.maxkey, u.maxkey);
            v.size += u.size;
            v.sum = op(u.sum, v.sum);
        }

        if (a->rch) {
            Vin &u = a->rch->_val;
            v.minkey = std::min(v.minkey, u.minkey);
            v.maxkey = std::max(v.maxkey, u.maxkey);
            v.size += u.size;
            v.sum = op(v.sum, u.sum);
        }
    }

    static constexpr void flip(Nin *a) {
        a->_val.flip = !a->_val.flip;
        std::swap(a->lch, a->rch);
        a->_val.sum = flip_val(a->_val.sum);
    }

    static constexpr void push_in(Nin *a) {
        if (a->_val.flip) {
            if (a->lch) flip(a->lch);
            if (a->rch) flip(a->rch);
            a->_val.flip = false;
        }
    }

    static constexpr void update_out(Nout *a) {
        assert(a);
        assert(a->_val.top);
        Vout &v = a->_val;
        v.size = v.top->_val.size;
        v.sum = v.top->_val.sum;

        if (a->lch) {
            Vout &u = a->lch->_val;
            v.size += u.size;
            v.sum = op(u.sum, v.sum);
        }

        if (a->rch) {
            Vout &u = a->rch->_val;
            v.size += u.size;
            v.sum = op(v.sum, u.sum);
        }
    }
    static constexpr void push_out(Nout *a) {}

    using Tin = treap<Nin, update_in, push_in>;
    using Tout = treap<Nout, update_out, push_out>;

  public:
    Nout* root;
    static constexpr int get_size(Nin *v) { return 1; }
    static constexpr int get_sumsize(Nin *v) { return v->_val.size; }
    static constexpr int get_size(Nout *v) { return v->_val.top->_val.size; }
    static constexpr int get_sumsize(Nout *v) { return v->_val.size; }
    static constexpr Key get_key(Nin *v) { return v->_val.key; }
    static constexpr Key get_maxkey(Nin *v) { return v->_val.maxkey; }
    static constexpr Key get_minkey(Nin *v) { return v->_val.minkey; }
    static constexpr bool is_ascending(Nin *u) {
        if (u->lch && u->_val.key < u->lch->_val.maxkey) return false;
        if (u->rch && u->_val.key > u->rch->_val.minkey) return false;
        return true;
    };

    segtree_sortable() : root(nullptr) {}
    segtree_sortable(Nout *_root) : root(_root) {}
    segtree_sortable(const std::vector<std::pair<Key, S>> &v) : root(nullptr) {
        if (v.empty()) return;
        int N = v.size();
        std::vector<Nin*> rootsin;
        std::vector<Nout*> rootsout;
        for (int i = 0; i < N; i++) {
            rootsin.push_back(new Nin(Vin(v[i].first, v[i].second)));
            bool cut = (i == N - 1);
            if (!cut && rootsin[0]->_val.key != rootsin.back()->_val.key) {
                bool f = rootsin[0]->_val.key < rootsin.back()->_val.key;
                bool g = rootsin.back()->_val.key < v[i + 1].first;
                if (f != g) cut = true;
            }
            if (cut) {
                Nout *r = new Nout(Vout(Tin::build(rootsin)));
                rootsout.push_back(r);
                rootsin.clear();
            }
        }
        root = Tout::build(rootsout);
    }
    
    int size() {
        return root ? root->_val.size : 0;
    }

    std::pair<Nout*, Nout*> split(Nout *v, int r) {
        Nout *w = nullptr;
        auto dfs = [&](auto &&dfs, Nout *u, int _r) -> std::pair<Nout*, Nout*> {
            if (!u) return {nullptr, nullptr};
            if (_r <= 0) return {nullptr, u};
            if (u->_val.size <= _r) return {u, nullptr};
            int lsz = u->lch ? u->lch->_val.size : 0;
            if (_r <= lsz) {
                auto [a, b] = dfs(dfs, u->lch, _r);
                Tout::set_lch(u, b);
                return {a, u};
            }
            int vsz = u->_val.top->_val.size;
            if (lsz + vsz <= _r) {
                auto [a, b] = dfs(dfs, u->rch, _r - lsz - vsz);
                Tout::set_rch(u, a);
                return {u, b};
            }
            auto [x, y] = Tin::template split_size<get_size, get_sumsize>(u->_val.top, _r - lsz);
            Nout *R = u->rch;
            Tout::cut_rch(u);
            u->_val.top = x;
            update_out(u);
            w = new Nout(Vout(y));
            return {u, R};
        };
        auto [L, R] = dfs(dfs, v, r);
        return {L, Tout::merge(w, R)};
    }

    std::tuple<Nout*, Nout*, Nout*> split3(Nout *v, int l, int r) {
        int N = v ? v->_val.size : 0;
        if (l > N - r) {
            auto [a, bc] = split(v, l);
            auto [b, c] = split(bc, r - l);
            return {a, b, c};
        } else {
            auto [ab, c] = split(v, r);
            auto [a, b] = split(ab, l);
            return {a, b, c};
        }
    }

    Nout *merge(Nout *a, Nout *b) { return Tout::merge(a, b); }
    Nout *merge3(Nout *a, Nout *b, Nout *c) { return Tout::template merge3<get_sumsize>(a, b, c); }

    Nout *make_node(Key key, S val) {
        Nin *a = new Nin(Vin(key, val));
        return new Nout(Vout(a));
    }

    // vの部分木中の木をキーの昇順にmeld
    Nout *meld(Nout *v) {
        if (!v) return nullptr;
        struct cmp {
            constexpr bool operator ()(const Nin *a, const Nin *b) const {
                return a->_val.size > b->_val.size;
            }
        };
        // 定数倍高速化のためにサイズの小さいやつからマージする
        std::priority_queue<Nin*, std::vector<Nin*>, cmp> pq;
        Tout::enumerate(v, [&](Nout *u) { 
            Nin *w = u->_val.top;
            if (!is_ascending(w)) flip(w);
            pq.push(w);
        });
        while (pq.size() > 1) {
            Nin *a = pq.top();
            pq.pop();
            Nin *b = pq.top();
            pq.pop();
            pq.push(Tin::template meld<Key, get_key, get_minkey, get_maxkey>(a, b));
        }
        return new Nout(pq.top());
    }
    
    // キー、値をset
    void set(int k, Key key, S val) {
        bool asc;
        auto dfs_in = [&](auto &&dfs_in, Nin *u, int t) -> std::pair<Nin*, Nin*> {
            push_in(u);
            int lsz = u->lch ? u->lch->_val.size : 0;
            if (t < lsz) {
                auto [l, r] = dfs_in(dfs_in, u->lch, t);
                Tin::set_lch(u, r);
                return {l, u};
            } else if(t == lsz) {
                bool is_l = (asc == (u->_val.key <= key));
                u->_val.key = key;
                u->_val.val = val;
                if (is_l) {
                    Nin *r = u->rch;
                    Tin::cut_rch(u);
                    return {u, r};
                } else {
                    Nin *l = u->lch;
                    Tin::cut_lch(u);
                    return {l, u};
                }
            } else {
                auto [l, r] = dfs_in(dfs_in, u->rch, t - lsz - 1);
                Tin::set_rch(u, l);
                return {u, r};
            }
        };

        auto [a, c] = Tout::template split_size<get_size, get_sumsize>(root, k);
        int rem = k - (a ? a->_val.size : 0);
        Nout *b = nullptr;
        auto dfs_out = [&](auto dfs_out, Nout *v) -> Nout* {
            assert(v);
            if (v->lch) {
                Nout *u = dfs_out(dfs_out, v->lch);
                Tout::set_lch(v, u);
                return v;
            }
            asc = is_ascending(v->_val.top);
            auto [x, y] = dfs_in(dfs_in, v->_val.top, rem);
            assert(x || y);
            if (!x || !y) {
                v->_val.top = (!x ? y : x);
            } else {
                v->_val.top = y;
                b = new Nout(Vout(x));
            }
            update_out(v);
            return v;
        };
        c = dfs_out(dfs_out, c);
        root = Tout::template merge3<get_sumsize>(a, b, c);
    }

    void set2(int k, Key key, S val) {
        auto [a, b, c] = split3(root, k, k + 1);
        Vin &x = b->_val.top->_val;
        x.key = key;
        x.val = val;
        update_in(b->_val.top);
        update_out(b);
        root = merge3(a, b, c);
    }

    // 値だけ変更 (do_applyならop(元の値, val)にする)
    void set_val(int k, S val, bool do_apply = false) {
        auto set_val_in = [&](Nin *v, int _k) -> void {
            Tin::template intersection<get_size, get_sumsize>(v, _k, _k + 1, 
                [&](Nin *u) { u->_val.val = (do_apply ? op(u->_val.val, val) : val); },
                [&](Nin *u, int _l, int _r) { u->_val.val = (do_apply ? op(u->_val.val, val) : val); }
            );
        };
        Tout::template intersection<get_size, get_sumsize>(root, k, k + 1, 
            [&](Nout *u) { set_val_in(u->_val.top, 0); },
            [&](Nout *u, int _l, int _r) { set_val_in(u->_val.top, _l); }
        );
    }

    S get(int k) {
        return prod(k, k + 1);
    }

    S prod(int l, int r) {
        S res = e();
        auto prod_in = [&](Nin *v, int L, int R) -> void {
            Tin::template intersection<get_size, get_sumsize>(
                v, L, R, [&](Nin *u) { res = op(res, u->_val.sum); },
                [&](Nin *u, int _l, int _r) { res = op(res, u->_val.val); }
            );
        };
        Tout::template intersection<get_size, get_sumsize>(
            root, l, r, [&](Nout *u) { res = op(res, u->_val.sum); }, 
            [&](Nout *u, int _l, int _r) { prod_in(u->_val.top, _l, _r); }
        );
        return res;
    }

    void sort(int l, int r, bool ascending = true) {
        if (l >= r) return;
        auto [a, b, c] = split3(root, l, r);
        if (b) {
            b = meld(b);
            if (!ascending) {
                flip(b->_val.top);
                update_out(b);
            }
        }
        root = merge3(a, b, c);
    }

    std::vector<std::pair<Key, S>> to_vector() {
        using P = std::pair<Key, S>;
        std::vector<P> res;
        Tout::enumerate(root, [&](Nout *a) -> void {
            Tin::enumerate(a->_val.top, [&](Nin* b) -> void { 
                res.push_back(P{b->_val.key, b->_val.val});
            });
        });
        return res;
    }
};
template<typename Key, typename monoid>
using segtree_sortable_monoid = segtree_sortable<Key, typename monoid::S, monoid::op, monoid::e, monoid::flip>;
#endif
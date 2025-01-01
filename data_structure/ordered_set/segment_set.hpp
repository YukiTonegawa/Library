#ifndef _SET_SEGMENTS_H_
#define _SET_SEGMENTS_H_
#include <numeric>
#include <algorithm>
#include <cassert>

// merge_adjacent: [1, 2), [2, 5)のような隣接する区間をマージするか
template<typename Idx, bool merge_adjacent = true>
struct segment_set {
    static constexpr Idx minf = std::numeric_limits<Idx>::min();
    static constexpr Idx inf = std::numeric_limits<Idx>::max();
  private:
    struct node {
        int h, sz;
        Idx L, R, lensum;
        node *l, *r;
        node(Idx _L, Idx _R): h(1), sz(1), L(_L), R(_R), lensum(R - L), l(nullptr), r(nullptr){}
        int balanace_factor() { return (l ? l->h : 0) - (r ? r->h : 0); }
    };
    node *root, *tmp_node;

    int size(node *v) { return v ? v->sz : 0; }

    void update(node *v) {
        v->h = std::max(v->l ? v->l->h : 0,  v->r ? v->r->h : 0) + 1;
        v->sz = (v->l ? v->l->sz : 0) + (v->r ? v->r->sz : 0) + 1;
        v->lensum = (v->R - v->L) + (v->l ? v->l->lensum : 0) + (v->r ? v->r->lensum : 0);
    }
    
    node *rotate_right(node *v) { 
        node *l = v->l;
        v->l = l->r;
        l->r = v;
        update(v);
        update(l);
        return l;
    }

    node *rotate_left(node *v) {
        node *r = v->r;
        v->r = r->l;
        r->l = v;
        update(v);
        update(r);
        return r;
    }

    node *balance(node *v) { 
        int bf = v->balanace_factor();
        assert(-2 <= bf && bf <= 2);
        if (bf == 2) {
            if (v->l->balanace_factor() == -1) {
                v->l = rotate_left(v->l);
                update(v);
            }
            return rotate_right(v);
        } else if(bf == -2) {
            if (v->r->balanace_factor() == 1) {
                v->r = rotate_right(v->r);
                update(v);
            }
            return rotate_left(v);
        }
        return v;
    }
    
    node *leftmost(node *v) {
        while (v->l) v = v->l;
        return v;
    }
    
    node *rightmost(node *v) {
        while (v->r) v = v->r;
        return v;
    }

    std::tuple<node*, Idx, Idx> _find(node *v, Idx k) {
        Idx Lmax = minf, Rmin = inf;
        while (v) {
            if (v->L <= k && k < v->R) {
                return {v, v->L, v->R};
            } else if (k < v->L) {
                Rmin = v->L;
                v = v->l;
            } else {
                Lmax = v->R;
                v = v->r;
            }
        }
        return {nullptr, Lmax, Rmin};
    }

    Idx _kth_point(node *v, Idx k) {
        while (true) {
            Idx lenl = (v->l ? v->l->lensum : 0);
            Idx lenv = lenl + (v->R - v->L);
            if (lenl <= k) {
                if (k < lenv) return v->L + (k - lenl);
                k -= lenv;
                v = v->r;
            } else v = v->l;
        }
        return inf;
    }

    node *_kth_segment(node *v, int k) {
        while (true) {
            int szl = size(v->l);
            if (szl <= k) {
                if (szl == k) return v;
                k -= szl + 1;
                v = v->r;
            } else v = v->l;
        }
    }
  
    int _low_count(node *v, Idx x) {
        int res = 0;
        while (v) {
            int szl = size(v->l);
            if (x < v->R) v = v->l;
            else v = v->r, res += szl + 1;
        }
        return res;
    }

    Idx _low_count_sum(node *v, Idx x) {
        Idx res = 0;
        while (v) {
            if (x <= v->L) {
                v = v->l;
            } else if(v->R <= x) {
                res += (v->l ? v->l->lensum : 0) + (v->R - v->L);
                v = v->r;
            } else {
                return res + (v->l ? v->l->lensum : 0) + (x - v->L);
            }
        }
        return res;
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

    node *_insert(node *v, Idx l, Idx r) {
        if (!v) return new node(l, r);
        if (l < v->L) {
            v->l = _insert(v->l, l, r);
        } else {
            v->r = _insert(v->r, l, r);    
        }
        update(v);
        return balance(v);
    }

    node *_erase(node *v, Idx l) {
        if (!v) return nullptr;
        if (l < v->L) {
            v->l = _erase(v->l, l);
        } else if (l > v->L) {
            v->r = _erase(v->r, l);
        } else {
            if (v->r) {
                v->r = cut_leftmost(v->r);
                tmp_node->l = v->l;
                tmp_node->r = v->r;
                free(v);
                update(tmp_node);
                return balance(tmp_node);
            }
            return v->l;
        }
        update(v);
        return balance(v);
    }

    void _merge(Idx l, Idx r) {
        auto [L, R] = _erase_intersect(l - merge_adjacent, r + merge_adjacent);
        root = _insert(root, std::min(L, l), std::max(R, r));
    }

    // 消された中で{最小, 最大}
    std::pair<Idx, Idx> _erase_include(Idx l, Idx r) {
        Idx emin = inf, emax = minf;
        for (auto [L, R] : enumerate_include(l, r)) {
            root = _erase(root, L);
            emin = std::min(emin, L);
            emax = std::max(emax, R);
        }
        return {emin, emax};
    }

    // 消された中で{最小, 最大}
    std::pair<Idx, Idx> _erase_intersect(Idx l, Idx r) {
        Idx emin = inf, emax = minf;
        for (auto [L, R] : enumerate_intersect(l, r)) {
            root = _erase(root, L);
            emin = std::min(emin, L);
            emax = std::max(emax, R);
        }
        return {emin, emax};
    }
  
    void _enumerate_include(node *v, Idx l, Idx r, std::vector<std::pair<Idx, Idx>> &res) {
        if (!v) return;
        if (v->l && l < v->L) _enumerate_include(v->l, l, r, res);
        if (l <= v->L && v->R <= r) res.push_back({v->L, v->R});
        if (v->r && v->R < r) _enumerate_include(v->r, l, r, res);
    }

    void _enumerate_intersect(node *v, Idx l, Idx r, std::vector<std::pair<Idx, Idx>> &res) {
        if (!v) return;
        if (v->l && l < v->L) _enumerate_intersect(v->l, l, r, res);
        if (std::max(l, v->L) < std::min(r, v->R)) res.push_back({v->L, v->R});
        if (v->r && v->R < r) _enumerate_intersect(v->r, l, r, res);
    }

  public:
    segment_set() : root(nullptr) {}
    
    // 区間の数
    int size() {
        return size(root);
    }

    bool empty() {
        return size_sum(root) == 0;
    }

    // a, bが同じ区間に含まれるか
    bool same(Idx a, Idx b) {
        auto [v, l, r] = find(a);
        return v && (l == std::get<1>(find(b)));
    }

    // kがいずれかの区間に含まれる場合 {1, L, R}
    // 含まれない場合 {0, L, R} (L, Rはkからいずれの区間にも含まれない座標だけを通って移動できる範囲, 何もない場合は{minf, inf})
    std::tuple<bool, Idx, Idx> find(Idx k) {
        auto [v, L, R] = _find(root, k);
        return v ? std::make_tuple(true, L, R) : std::make_tuple(false, L, R);
    }

    std::pair<Idx, Idx> min() {
        assert(size());
        node *v = leftmost(root);
        return {v->L, v->R};
    }

    std::pair<Idx, Idx> max() {
        assert(size());
        node *v = rightmost(root);
        return {v->L, v->R};
    }
  
    // 任意の区間に含まれないかつa以上の最小要素
    Idx mex(Idx a = 0) {
        assert(merge_adjacent);
        auto [v, L, R] = find(a);
        return v ? R : a;
    }

    // いずれかの区間に含まれるk番目(0-indexed)に小さい点. ない場合はinf
    Idx kth_point(Idx k){
        return _kth_point(root, k);
    }

    // k番目(0-indexed)に小さい区間. ない場合は{inf, inf}
    std::pair<Idx, Idx> kth_segment(int k) {
        if (size() <= k) return {inf, inf};
        node *v = _kth_segment(root, k);
        return {v->L, v->R};
    }

    // [l, r)がr <= xであるような区間の数
    int low_count(Idx x){
        return _low_count(root, x);
    }

    // いずれかの区間に含まれ, かつx未満の整数座標の数
    Idx low_count_sum(Idx x){
        return _low_count_sum(root, x);
    }

    // [l, r)をマージ
    void merge(Idx l, Idx r){
        _merge(l, r);
    }

    // [l, r)に含まれる区間を削除
    void erase_include(Idx l, Idx r) {
        _erase_include(l, r);
    }

    // [l, r)と少しでも交差する区間を削除
    void erase_intersect(Idx l, Idx r) {
        _erase_intersect(l, r);
    }
    
    // [l, r)が完全に含む区間を列挙
    std::vector<std::pair<Idx, Idx>> enumerate_include(Idx l, Idx r) {
        std::vector<std::pair<Idx, Idx>> res;
        _enumerate_include(root, l, r, res);
        return res;
    }

    // [l, r)と交差する区間を列挙
    std::vector<std::pair<Idx, Idx>> enumerate_intersect(Idx l, Idx r) {
        std::vector<std::pair<Idx, Idx>> res;
        _enumerate_intersect(root, l, r, res);
        return res;
    }
  
    // 全区間を列挙
    std::vector<std::pair<Idx, Idx>> enumerate_all() {
        return enumerate_intersect(minf, inf);
    }

    void clear() {
        root = nullptr;
    }

    void swap(segment_set<Idx> &r) {
        std::swap(root, r.root);
    }

    // rの要素を全て移動(永続でないためrの要素は全て消える)
    void meld(segment_set<Idx> &r) {
        if (size() < r.size()) swap(r);
        for (auto [L, R] : r.enumerate_all()) merge(L, R);
        r.clear();
    }
};
#endif
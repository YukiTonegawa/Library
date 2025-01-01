#ifndef _PERSITENT_SEGTREE_LAZY_H_
#define _PERSITENT_SEGTREE_LAZY_H_
#include <vector>
#include <type_traits>
#include <cassert>
#include <array>
#include "../../math/base/bit_ceil.hpp"

template<typename S, S (*op)(S, S), S (*e)(), typename F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id_f)(), unsigned MAXIDX>
struct persistent_segtree_lazy {
    using self_t = persistent_segtree_lazy<S, op, e, F, mapping, composition, id_f, MAXIDX>;
    static constexpr unsigned log = bit_ceil_log(MAXIDX);
    static constexpr int nullid = -1;

    template<unsigned dep, typename = void>
    struct node {};

    template<unsigned dep>
    struct node <dep, std::enable_if_t<dep == log>> {
        S val;
        node() : val(e()) {}
    };

    template<unsigned dep>
    struct node <dep, std::enable_if_t<dep != log>> {
        int lch, rch;
        F lzl, lzr;
        S val;
        node() : lch(nullid), rch(nullid), lzl(id_f()), lzr(id_f()), val(e()) {}
    };

    template<unsigned dep>
    struct node_vector {
        static std::vector<node<dep>> V;
        static int copy_node(int id = nullid) {
            if (id == nullid) {
                id = V.size();
                V.push_back(node<dep>());
                return id;
            } else {
                V.push_back(V[id]);
                id = V.size() - 1;
                return id;
            }
        }
        static S get_val(int id) {
            if (id == nullid) return e();
            return V[id].val;
        }
    };

    template<unsigned dep>
    struct node_ref {
        int id;
        node_ref(int _id) : id(_id) {}
        node<dep>& operator * () { return node_vector<dep>::V[id]; }
    };

    template<unsigned dep, unsigned len, std::enable_if_t<dep == log>* = nullptr>
    static void all_apply(int id, F lz) {
        node_ref<dep> v(id);
        (*v).val = mapping(lz, (*v).val, len);
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep != log>* = nullptr>
    static void all_apply(int id, F lz) {
        node_ref<dep> v(id);
        (*v).val = mapping(lz, (*v).val, len);
        (*v).lzl = composition(lz, (*v).lzl);
        (*v).lzr = composition(lz, (*v).lzr);
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep == log>* = nullptr>
    static int set(int id, unsigned k, const S &x, F lz) {
        id = node_vector<dep>::copy_node(id);
        node_vector<dep>::V[id].val = x;
        return id;
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep != log>* = nullptr>
    static int set(int id, unsigned k, const S &x, F lz) {
        id = node_vector<dep>::copy_node(id);
        node_ref<dep> v(id);
        all_apply<dep, len>(id, lz);
        int dir = (k >> (log - dep - 1)) & 1;
        if (dir == 0) {
            (*v).lch = set<dep + 1, len / 2>((*v).lch, k, x, composition(lz, (*v).lzl));
            (*v).lzl = id_f();
            S val_r = mapping(composition(lz, (*v).lzr), node_vector<dep + 1>::get_val((*v).rch), len / 2);
            (*v).val = op(node_vector<dep + 1>::get_val((*v).lch), val_r);
        } else {
            (*v).rch = set<dep + 1, len / 2>((*v).rch, k, x, composition(lz, (*v).lzr));
            (*v).lzr = id_f();
            S val_l = mapping(composition(lz, (*v).lzl), node_vector<dep + 1>::get_val((*v).lch), len / 2);
            (*v).val = op(val_l, node_vector<dep + 1>::get_val((*v).rch));
        }
        return id;
    }

    template<unsigned dep>
    static S get(int id, unsigned k, F lz) {
        if (id == nullid) return mapping(lz, e(), 1);
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            return mapping(lz, (*v).val, 1);
        } else {
            int dir = (k >> (log - dep - 1)) & 1;
            if (dir == 0) {
                return get<dep + 1>((*v).lch, k, composition(lz, (*v).lzl));
            } else {
                return get<dep + 1>((*v).rch, k, composition(lz, (*v).lzr));
            }
        }
    }
    
    template<unsigned dep, unsigned len, std::enable_if_t<dep == log>* = nullptr>
    static int apply(int id, unsigned l, unsigned r, unsigned L, unsigned R, F lz, F lz2) {
        id = node_vector<dep>::copy_node(id);
        if (r <= L || R <= l) {
            all_apply<dep, len>(id, lz);
        } else {
            all_apply<dep, len>(id, composition(lz2, lz));
        }
        return id;
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep != log>* = nullptr>
    static int apply(int id, unsigned l, unsigned r, unsigned L, unsigned R, F lz, F lz2) {
        id = node_vector<dep>::copy_node(id);
        all_apply<dep, len>(id, lz);
        if (r <= L || R <= l) return id;
        if (l <= L && R <= r) {
            all_apply<dep, len>(id, lz2);
            return id;
        }
        node_ref<dep> v(id);
        unsigned M = (L + R) / 2;
        S suml = e(), sumr = e();
        if (std::max(L, l) < std::min(M, r)) {
            (*v).lch = apply<dep + 1, len / 2>((*v).lch, l, r, L, M, (*v).lzl, lz2);
            (*v).lzl = id_f();
            suml = node_vector<dep + 1>::get_val((*v).lch);
        } else {
            suml = mapping((*v).lzl, node_vector<dep + 1>::get_val((*v).lch), len / 2);
        }
        if (std::max(M, l) < std::min(R, r)) {
            (*v).rch = apply<dep + 1, len / 2>((*v).rch, l, r, M, R, (*v).lzr, lz2);
            (*v).lzr = id_f();
            sumr = node_vector<dep + 1>::get_val((*v).rch);
        } else {
            sumr = mapping((*v).lzr, node_vector<dep + 1>::get_val((*v).rch), len / 2);
        }
        (*v).val = op(suml, sumr);
        return id;
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep == log>* = nullptr>
    static S prod(int id, unsigned l, unsigned r, unsigned L, unsigned R, F lz) {
        if (r <= L || R <= l) return e();
        return mapping(lz, node_vector<dep>::get_val(id), len);
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep != log>* = nullptr>
    static S prod(int id, unsigned l, unsigned r, unsigned L, unsigned R, F lz) {
        if (r <= L || R <= l) return e();
        if (l <= L && R <= r) return mapping(lz, node_vector<dep>::get_val(id), len);
        if (id == nullid) return mapping(lz, e(), std::min(R, r) - std::max(L, l));
        node_ref<dep> v(id);
        unsigned M = (L + R) / 2;
        return op(prod<dep + 1, len / 2>((*v).lch, l, r, L, M, composition(lz, (*v).lzl)), 
                prod<dep + 1, len / 2>((*v).rch, l, r, M, R, composition(lz, (*v).lzr)));
    }

    template<unsigned dep, std::enable_if_t<dep == log>* = nullptr>
    int build(const std::vector<S> &V, unsigned L, unsigned R) {
        if (L >= V.size()) return nullid;
        int id = node_vector<dep>::copy_node();
        node_vector<dep>::V[id].val = V[L];
        return id;
    }

    template<unsigned dep, std::enable_if_t<dep != log>* = nullptr>
    int build(const std::vector<S> &V, unsigned L, unsigned R) {
        if (L >= V.size()) return nullid;
        int id = node_vector<dep>::copy_node();
        node_ref<dep> v(id);
        unsigned M = (L + R) / 2;
        (*v).lch = build<dep + 1>(V, L, M);
        (*v).rch = build<dep + 1>(V, M, R);
        (*v).val = op(node_vector<dep + 1>::get_val((*v).lch), node_vector<dep + 1>::get_val((*v).rch));
        return id;
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep == log>* = nullptr>
    static int copy(int id1, int id2, unsigned l, unsigned r, unsigned L, unsigned R, F lz1, F lz2) {
        if (r <= L || R <= l) {
            id1 = node_vector<dep>::copy_node(id1);
            all_apply<dep, len>(id1, lz1);
            return id1;
        } else {
            id2 = node_vector<dep>::copy_node(id2);
            all_apply<dep, len>(id2, lz2);
            return id2;
        }
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep != log>* = nullptr>
    static int copy(int id1, int id2, unsigned l, unsigned r, unsigned L, unsigned R, F lz1, F lz2) {
        if (r <= L || R <= l) {
            id1 = node_vector<dep>::copy_node(id1);
            all_apply<dep, len>(id1, lz1);
            return id1;
        }
        if (l <= L && R <= r) {
            id2 = node_vector<dep>::copy_node(id2);
            all_apply<dep, len>(id2, lz2);
            return id2;
        }
        id1 = node_vector<dep>::copy_node(id1);
        all_apply<dep, len>(id1, lz1);
        node_ref<dep> v1(id1), v2(id2);
        unsigned M = (L + R) / 2;
        S suml = e(), sumr = e();
        if (std::max(L, l) < std::min(M, r)) {
            int lch2 = (id2 == nullid ? nullid : (*v2).lch);
            F llz2 = (id2 == nullid ? lz2 : composition(lz2, (*v2).lzl));
            (*v1).lch = copy<dep + 1, len / 2>((*v1).lch, lch2, l, r, L, M, (*v1).lzl, llz2);
            (*v1).lzl = id_f();
            suml = node_vector<dep + 1>::get_val((*v1).lch);
        } else {
            suml = mapping((*v1).lzl, node_vector<dep + 1>::get_val((*v1).lch), len / 2);
        }
        if (std::max(M, l) < std::min(R, r)) {
            int rch2 = (id2 == nullid ? nullid : (*v2).rch);
            F rlz2 = (id2 == nullid ? lz2 : composition(lz2, (*v2).lzr));
            (*v1).rch = copy<dep + 1, len / 2>((*v1).rch, rch2, l, r, M, R, (*v1).lzr, rlz2);
            (*v1).lzr = id_f();
            sumr = node_vector<dep + 1>::get_val((*v1).rch);
        } else {
            sumr = mapping((*v1).lzr, node_vector<dep + 1>::get_val((*v1).rch), len / 2);
        }
        (*v1).val = op(suml, sumr);
        return id1;
    }

    int rootid;
    persistent_segtree_lazy(int _rootid) : rootid(_rootid) {}
  
  public:
    persistent_segtree_lazy() : rootid(nullid) {}
    // [0, |V|)の範囲で初期化
    persistent_segtree_lazy(const std::vector<S> &V) : rootid(build<0>(V, 0, 1 << log)) {}

    self_t set(unsigned k, const S &x) const {
        int id = set<0, (unsigned)1 << log>(rootid, k, x, id_f());
        return self_t(id);
    }

    S get(unsigned k) const {
        return get<0>(rootid, k, id_f());
    }

    self_t apply(unsigned l, unsigned r, F lz) const {
        int id = apply<0, (unsigned)1 << log>(rootid, l, r, 0, (unsigned)1 << log, id_f(), lz);
        return self_t(id);
    }
    
    S prod(unsigned l, unsigned r) const {
        return prod<0, (unsigned)1 << log>(rootid, l, r, 0, (unsigned)1 << log, id_f());
    }

    S operator [] (unsigned k) const {
        return get(k);
    }

    // [l, r)をA[l, r)にする
    self_t copy(self_t A, unsigned l, unsigned r) const {
        return copy<0, (unsigned)1 << log>(rootid, A.rootid, l, r, 0, (unsigned)1 << log, id_f(), id_f());
    }
};

template<typename S, S (*op)(S, S), S (*e)(), typename F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id_f)(), unsigned MAXIDX>
template<unsigned dep>
std::vector<typename persistent_segtree_lazy<S, op, e, F, mapping, composition, id_f, MAXIDX>::template node<dep>> 
persistent_segtree_lazy<S, op, e, F, mapping, composition, id_f, MAXIDX>::node_vector<dep>::V;
#endif
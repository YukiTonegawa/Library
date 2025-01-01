#ifndef _PERSISTENT_SEGTREE_H_
#define _PERSISTENT_SEGTREE_H_
#include <vector>
#include <type_traits>
#include <cassert>
#include <array>
#include "../../math/base/bit_ceil.hpp"

template<typename S, S (*op)(S, S), S (*e)(), unsigned MAXIDX>
struct persistent_segtree {
    using self_t = persistent_segtree<S, op, e, MAXIDX>;
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
        S val;
        node() : lch(nullid), rch(nullid), val(e()) {}
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

    template<unsigned dep>
    static int set(int id, unsigned k, const S &x) {
        id = node_vector<dep>::copy_node(id);
        node_ref<dep> v(id);
        if constexpr (dep == log) {    
            (*v).val = x;
            return id;
        } else {
            int dir = (k >> (log - dep - 1)) & 1;
            if (dir == 0) (*v).lch = set<dep + 1>((*v).lch, k, x);
            else (*v).rch = set<dep + 1>((*v).rch, k, x);
            (*v).val = op(node_vector<dep + 1>::get_val((*v).lch), node_vector<dep + 1>::get_val((*v).rch));
            return id;
        }
    }

    template<unsigned dep>
    static S get(int id, unsigned k) {
        if (id == nullid) return e();
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            return (*v).val;
        } else {
            int dir = (k >> (log - dep - 1)) & 1;
            if (dir == 0) return get<dep + 1>((*v).lch, k);
            else return get<dep + 1>((*v).rch, k);
        }
    }

    template<unsigned dep, std::enable_if_t<dep == log>* = nullptr>
    static S prod(int id, unsigned l, unsigned r, unsigned L, unsigned R) {
        if (id == nullid || r <= L || R <= l) return e();
        return node_vector<dep>::get_val(id);
    }

    template<unsigned dep, std::enable_if_t<dep != log>* = nullptr>
    static S prod(int id, unsigned l, unsigned r, unsigned L, unsigned R) {
        if (id == nullid || r <= L || R <= l) return e();
        node_ref<dep> v(id);
        if (l <= L && R <= r) return (*v).val;
        unsigned M = (L + R) / 2;
        return op(prod<dep + 1>((*v).lch, l, r, L, M), prod<dep + 1>((*v).rch, l, r, M, R));
    }

    template<unsigned dep, std::enable_if_t<dep == log>* = nullptr>
    int build(const std::vector<S> &V, unsigned L, unsigned R) {
        if (L >= V.size()) return nullid;
        int id = node_vector<dep>::copy_node();
        node_ref<dep> v(id);
        (*v).val = V[L];
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

    template<unsigned dep, std::enable_if_t<dep == log>* = nullptr>
    static int copy(int id1, int id2, unsigned l, unsigned r, unsigned L, unsigned R) {
        if (r <= L || R <= l) return id1;
        return id2;
    }

    template<unsigned dep, std::enable_if_t<dep != log>* = nullptr>
    static int copy(int id1, int id2, unsigned l, unsigned r, unsigned L, unsigned R) {
        if (r <= L || R <= l) return id1;
        if (l <= L && R <= r) return id2;
        id1 = node_vector<dep>::copy_node(id1);
        unsigned M = (L + R) / 2;
        node_ref<dep> v1(id1);
        int lch2 = id2 == nullid ? nullid : node_vector<dep>::V[id2].lch;
        int rch2 = id2 == nullid ? nullid : node_vector<dep>::V[id2].rch;
        (*v1).lch = copy<dep + 1>((*v1).lch, lch2, l, r, L, M);
        (*v1).rch = copy<dep + 1>((*v1).rch, rch2, l, r, M, R);
        (*v1).val = op(node_vector<dep + 1>::get_val((*v1).lch), node_vector<dep + 1>::get_val((*v1).rch));
        return id1;
    }

    int rootid;
    persistent_segtree(int _rootid) : rootid(_rootid) {}
  
  public:
    persistent_segtree() : rootid(nullid) {}
    
    // [0, |V|)の範囲で初期化
    persistent_segtree(const std::vector<S> &V) : rootid(build<0>(V, 0, (unsigned)1 << log)) {}

    self_t set(unsigned k, const S &x) const {
        int id = set<0>(rootid, k, x);
        return self_t(id);
    }

    S get(unsigned k) const {
        return get<0>(rootid, k);
    }

    S prod(unsigned l, unsigned r) const {
        return prod<0>(rootid, l, r, 0, (unsigned)1 << log);
    }

    S operator [] (unsigned k) const {
        return get(k);
    }

    // [l, r)をA[l, r)にする
    self_t copy(self_t A, unsigned l, unsigned r) const {
        return copy<0>(rootid, A.rootid, l, r, 0, (unsigned)1 << log);
    }
};

template<typename S, S (*op)(S, S), S (*e)(), unsigned MAXIDX> template<unsigned dep>
std::vector<typename persistent_segtree<S, op, e, MAXIDX>::template node<dep>> persistent_segtree<S, op, e, MAXIDX>::node_vector<dep>::V;
#endif
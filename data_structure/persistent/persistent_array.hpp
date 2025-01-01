#ifndef _PERSISTENT_ARRAY_H_
#define _PERSISTENT_ARRAY_H_
#include <vector>
#include <type_traits>
#include <cassert>
#include <array>
#include "../../math/base/bit_ceil.hpp"

// 使用可能なインデックスの範囲が[0, MAXIDX)
template<typename T, T (*e)(), unsigned MAXIDX>
struct persistent_array {
  private:
    using self_t = persistent_array<T, e, MAXIDX>;
    static constexpr unsigned K = 2; // 2^K分木
    static constexpr unsigned log = (bit_ceil_log(MAXIDX) + K - 1) / K;
    static constexpr int nullid = -1;

    template<unsigned dep, typename = void>
    struct node {};

    template<unsigned dep>
    struct node <dep, std::enable_if_t<dep == log>> {
        T val;
        node() {}
    };

    template<unsigned dep>
    struct node <dep, std::enable_if_t<dep != log>> {
        std::array<int, 1 << K> ch;
        node() { ch.fill(nullid); }
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
    };

    template<unsigned dep>
    struct node_ref {
        int id;
        node_ref(int _id) : id(_id) {}
        node<dep>& operator * () { return node_vector<dep>::V[id]; }
    };

    template<unsigned dep>
    static int set(int id, unsigned k, const T &x) {
        id = node_vector<dep>::copy_node(id);
        node_ref<dep> v(id);
        if constexpr (dep == log) {    
            (*v).val = x;
            return id;
        } else {
            int dir = (k >> (K * (log - dep - 1))) % (1 << K);
            (*v).ch[dir] = set<dep + 1>((*v).ch[dir], k, x);
            return id;
        }
    }

    template<unsigned dep>
    static T get(int id, unsigned k) {
        if (id == nullid) return e();
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            return (*v).val;
        } else {
            int dir = (k >> (K * (log - dep - 1))) % (1 << K);
            return get<dep + 1>((*v).ch[dir], k);
        }
    }

    template<unsigned dep, std::enable_if_t<dep == log>* = nullptr>
    int build(const std::vector<T> &V, unsigned L, unsigned R) {
        if (L >= V.size()) return nullid;
        int id = node_vector<dep>::copy_node();
        node_vector<dep>::V[id].val = V[L];
        return id;
    }

    template<unsigned dep, std::enable_if_t<dep != log>* = nullptr>
    int build(const std::vector<T> &V, unsigned L, unsigned R) {
        if (L >= V.size()) return nullid;
        int id = node_vector<dep>::copy_node();
        node_ref<dep> v(id);
        unsigned M = (L + R) / 2;
        (*v).lch = build<dep + 1>(V, L, M);
        (*v).rch = build<dep + 1>(V, M, R);
        return id;
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep == log>* = nullptr>
    static int copy(int id1, int id2, unsigned l, unsigned r, unsigned L, unsigned R) {
        if (r <= L || R <= l) return id1;
        return id2;
    }

    template<unsigned dep, unsigned len, std::enable_if_t<dep != log>* = nullptr>
    static int copy(int id1, int id2, unsigned l, unsigned r, unsigned L, unsigned R) {
        if (r <= L || R <= l) return id1;
        if (l <= L && R <= r) return id2;
        id1 = node_vector<dep>::copy_node(id1);
        node_ref<dep> v1(id1);
        for (int i = 0; i < (1 << K); i++) {
            int ch2 = id2 == nullid ? nullid : node_vector<dep>::V[id2].ch[i];
            (*v1).ch[i] = copy<dep + 1, (len >> K)>((*v1).ch[i], ch2, l, r, L + i * (len >> K), L + (i + 1) * (len >> K));
        }
        return id1;
    }

    int rootid;
    persistent_array(int _rootid) : rootid(_rootid) {}
  
  public:

    persistent_array() : rootid(nullid) {}
    
    // [0, |V|)の範囲で初期化
    persistent_array(const std::vector<T> &V) : rootid(build<0>(V, 0, (unsigned)1 << log)) {}

    self_t set(unsigned k, const T &x) const {
        int id = set<0>(rootid, k, x);
        return self_t(id);
    }

    T get(unsigned k) const {
        return get<0>(rootid, k);
    }

    T operator [] (unsigned k) const {
        return get(k);
    }

    // [l, r)をA[l, r)にする
    self_t copy(self_t A, unsigned l, unsigned r) const {
        return copy<0, (unsigned)1 << log>(rootid, A.rootid, l, r, 0, (unsigned)1 << log);
    }
};

template<typename T, T (*e)(), unsigned MAXIDX> template<unsigned dep>
std::vector<typename persistent_array<T, e, MAXIDX>::template node<dep>> persistent_array<T, e, MAXIDX>::node_vector<dep>::V;
#endif
#ifndef _PERSISTENT_ARRAY_H_
#define _PERSISTENT_ARRAY_H_
#include <vector>
#include <type_traits>
#include <cassert>
#include <array>
#include "../../math/base/bit_ceil.hpp"

// 使用可能なインデックスの範囲が[0, MAXIDX)
template<typename T, T (*e)(), int MAXIDX>
struct persistent_array {
  private:
    using self_t = persistent_array<T, e, MAXIDX>;
    static constexpr int K = 2;
    static constexpr int log = (bit_ceil_log(MAXIDX) + K - 1) / K;
    static constexpr int nullid = -1;

    template<int dep, typename = void>
    struct node {};

    template<int dep>
    struct node <dep, std::enable_if_t<dep == log>> {
        T val;
        node() {}
    };

    template<int dep>
    struct node <dep, std::enable_if_t<dep != log>> {
        std::array<int, 1 << K> ch;
        node() { ch.fill(nullid); }
    };

    template<int dep>
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

    template<int dep>
    struct node_ref {
        int id;
        node_ref(int _id) : id(_id) {}
        node<dep>& operator * () { return node_vector<dep>::V[id]; }
    };

    template<int dep>
    static int set(int id, int k, const T &x) {
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

    template<int dep>
    static T get(int id, int k) {
        if (id == nullid) return e();
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            return (*v).val;
        } else {
            int dir = (k >> (K * (log - dep - 1))) % (1 << K);
            return get<dep + 1>((*v).ch[dir], k);
        }
    }

    template<int dep>
    int build(const std::vector<T> &V, int L, int R) {
        if (L >= V.size()) return nullid;
        int id = node_vector<dep>::copy_node();
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            (*v).val = V[L];
        } else {
            int M = (L + R) / 2;
            (*v).lch = build<dep + 1>(V, L, M);
            (*v).rch = build<dep + 1>(V, M, R);
        }
        return id;
    }

    template<int dep>
    static int copy(int id1, int id2, int l, int r, int L, int R) {
        if (r <= L || R <= l) return id1;
        if (l <= L && R <= r) return id2;
        id1 = node_vector<dep>::copy_node(id1);
        node_ref<dep> v1(id1);
        int d = (R - L) >> K;
        for (int i = 0; i < (1 << K); i++) {
            if constexpr (dep < log) {
                int ch2 = (id2 == nullid ? nullid : node_vector<dep>::V[id2].ch[i]);
                (*v1).ch[i] = copy<dep + 1>((*v1).ch[i], ch2, l, r, L + i * d, L + (i + 1) * d);
            }
        }
        return id1;
    }

    int rootid;
    persistent_array(int _rootid) : rootid(_rootid) {}
  
  public:

    persistent_array() : rootid(nullid) {}
    
    // [0, |V|)の範囲で初期化
    persistent_array(const std::vector<T> &V) : rootid(build<0>(V, 0, (int)K << log)) {}

    self_t set(int k, const T &x) const {
        int id = set<0>(rootid, k, x);
        return self_t(id);
    }

    T get(int k) const {
        return get<0>(rootid, k);
    }

    T operator [] (int k) const {
        return get(k);
    }

    // [l, r)をA[l, r)にする
    self_t copy(self_t A, int l, int r) const {
        return copy<0>(rootid, A.rootid, l, r, 0, (int)K << log);
    }
};

template<typename T, T (*e)(), int MAXIDX> template<int dep>
std::vector<typename persistent_array<T, e, MAXIDX>::template node<dep>> persistent_array<T, e, MAXIDX>::node_vector<dep>::V;
#endif
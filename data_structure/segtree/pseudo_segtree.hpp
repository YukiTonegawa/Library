#ifndef _PSEUDO_SEGTREE_H_
#define _PSEUDO_SEGTREE_H_
#include "../../math/base/bit_ceil.hpp"
#include <algorithm>
#include <cassert>

struct pseudo_segtree {
    // num_leaf = 2 ^ log
    static int log(int N) { return bit_ceil_log(N); }

    // [1, num_leaf) : 葉以外
    // [num_leaf, 2 * num_leaf) : 葉
    static int num_leaf(int N) { return bit_ceil(N); }

    // 頂点番号が [1, 2 * num_leaf)
    static int end_idx(int N) { return 2 * num_leaf(N); }

    // 頂点番号1を根(深さ0)としたときのidの深さ
    static int depth(int N, int id) { return 31 - __builtin_clz(id); }

    static int width(int N, int id) { return 1 << depth(N, id); }

    // idが管理する区間
    static std::pair<int, int> segment(int N, int id) { 
        int d = depth(N, id), s = id - (1 << d);
        return {s << d, (s + 1) << d};
    }

    template<typename F>
    static void query_leaf(int N, int leaf_id, F f) {
        leaf_id += num_leaf(N);
        while (leaf_id) {
            f(leaf_id);
            leaf_id /= 2;
        }
    }

    // f(対応する区間番号)を呼び出す
    template<typename F>
    static void query_segment(int N, int l, int r, F f) {
        l += num_leaf(N);
        r += num_leaf(N);
        while (l < r) {
            if (l & 1) f(l++);
            if (r & 1) f(--r);
            l >>= 1;
            r >>= 1;
        }
    }

    // f(対応する区間番号)を左から右に呼び出す
    template<typename F>
    static void query_segment_order(int N, int l, int r, F f) {
        static std::array<int, 60> tmp;
        static int pos = 0;
        l += num_leaf(N);
        r += num_leaf(N);
        while (l < r) {
            if (l & 1) f(l++);
            if (r & 1) tmp[pos++] = --r;
            l >>= 1;
            r >>= 1;
        }
        while (pos) f(tmp[--pos]);
    }
};

#endif
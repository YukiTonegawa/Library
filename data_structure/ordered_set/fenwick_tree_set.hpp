#ifndef _FENWICK_TREE_SET_H_
#define _FENWICK_TREE_SET_H_
#include "../fenwick_tree/fenwick_tree.hpp"
#include <vector>

struct fenwick_tree_set {
  private:
    using ull = uint64_t;
    static constexpr int len_block = 64;
    int N, M;
    std::vector<ull> val;
    fenwick_tree<int> ft;
  public:
    fenwick_tree_set() : fenwick_tree_set(0) {}
    fenwick_tree_set(int n): N(n), M((n + len_block - 1) / len_block), val(M, 0), ft(M) {}
  
    // xを追加(trueを返す)
    // 既にある場合は何もしない(falseを返す)
    bool insert(int x) {
        assert(0 <= x && x < N);
        int i = x / len_block;
        int j = x % len_block;
        if ((val[i] >> j) & 1) return false;
        val[i] ^= ull(1) << j;
        ft.apply(i, 1);
        return true;
    }

    // xを削除(trueを返す)
    // ない場合は何もしない(falseを返す)
    bool erase(int x) {
        assert(0 <= x && x < N);
        int i = x / len_block;
        int j = x % len_block;
        if (!((val[i] >> j) & 1)) return false;
        val[i] ^= ull(1) << j;
        ft.apply(i, -1);
        return true;
    }

    // xがあるか
    bool find(int x) {
        assert(0 <= x && x < N);
        int i = x / len_block;
        int j = x % len_block;
        return (val[i] >> j) & 1;
    }

    // [0, r)の要素数
    int rank(int r) {
        assert(0 <= r && r <= N);
        int i = r / len_block;
        int j = r % len_block;
        int res = ft.prod(i);
        if (j) res += __builtin_popcountll(val[i] << (len_block - j));
        return res;
    }
};

#endif
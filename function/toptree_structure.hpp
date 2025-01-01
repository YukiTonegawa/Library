#ifndef _TOPTREE_STRUCTURE_H_
#define _TOPTREE_STRUCTURE_H_
#include <algorithm>
#include <numeric>

//      Hp
//      |
//      Hv
//   /  | 
// L    Hc
//      |
//      Hc2...

// のような木で Lの値は確定している. Hの値は子孫が続くので確定していない. (関数の形を取る)
// merge_heavy_light (Hc, L) -> Hc' 木dpの部分木の集約
// merge_heavy (Hv, Hc) -> Hv' 木dpの子のマージ
// merge_light (L, L2) -> L'   木dpの部分木の集約

template<typename T>
struct max_dist{
  struct F{
    T allsz, clen;
    F(T key = 0): allsz(key), clen(key){}
  };
  using HVal = F;
  using LVal = T;
  static constexpr HVal id(){ return HVal(0); }
  static constexpr LVal to_light(const HVal &v){ return v.clen; }
  static constexpr HVal merge_heavy_light(const HVal &a, const LVal &b){
    HVal res = a;
    res.clen = std::max(res.clen, b);
    return res;
  }
  static constexpr HVal merge_heavy(const HVal &p, const HVal &c){
    HVal res = p;
    res.clen = std::max(p.clen, p.allsz + c.clen);
    res.allsz += c.allsz;
    return res;
  }
  static constexpr LVal merge_light(const LVal &a, const LVal &b){ return std::max(a, b); }
};

/*
template<typename T>
struct tt_range_sum{
  using Val = T;
  using Lazy = T;
  static constexpr Val id(){ return 0; }
  static constexpr Lazy id_lazy(){ return 0; }
  // heavy_edgeの子, 親を反転
  static constexpr void flip(Val &a){}
  static constexpr void merge_subtree(Val &v, const Val &p, const Val &c, const Val &l){
    v += p + c + l;
  }
  // light_edge同士をマージ
  static constexpr void merge_light(Val &v, const Val &c){
    v += c;
  }
  static constexpr void apply(Val &a, const Lazy &b, int sz){
    a += b * sz;
  }
  static constexpr void propagate(Lazy &a, const Lazy &b){
    a += b;
  }
};
*/

// f(v) = A_v + prod(f(c))
template<typename T>
struct hash_on_tree{
  // 葉だけ傾きを0にする
  // B[i] = new node({!is_leaf[i], A[i]});
  struct F{ T a, b; };
  using HVal = F;
  using LVal = T;
  static constexpr HVal id(){ return {1, 0}; }
  static constexpr LVal to_light(const HVal &v){ return v.a + v.b; }
  static constexpr HVal merge_heavy_light(const HVal &a, const LVal &b){ return {a.a * b, a.b * b}; }
  static constexpr HVal merge_heavy(const HVal &p, const HVal &v){
    T b = p.b + v.b * p.a;
    T a = v.a * p.a;
    return {a, b};
  }
  static constexpr LVal merge_light(const LVal &a, const LVal &b){ return a * b; }
};
// sum(頂点vから根の関数のcomposite)
template<typename T>
struct path_composite_sum{
  struct F{
    T a, b, c;
    int cnt;
  };
  struct G{
    T c;
    int cnt;
  };
  using HVal = F;
  using LVal = G;
  static constexpr HVal id(){ return {1, 0, 0, 0}; }
  static constexpr LVal to_light(const HVal &v){ return {v.c, v.cnt}; }
  static constexpr HVal merge_heavy_light(const HVal &a, const LVal &b){ return {a.a, a.b, a.c + b.c, a.cnt + b.cnt}; }
  static constexpr HVal merge_heavy(const HVal &p, const HVal &v){
    T c = p.a * v.c + p.b * v.cnt + p.c;
    T b = p.b + v.b * p.a;
    T a = v.a * p.a;
    return {a, b, c, v.cnt + p.cnt};
  }
  static constexpr LVal merge_light(const LVal &a, const LVal &b){ return {a.c + b.c, a.cnt + b.cnt}; }
};
template<typename T>
struct distance_sum{
  struct F{ T distsum, add, len; };
  struct G{ T distsum, add, addmax; };
  using HVal = F;
  using LVal = G;
  static constexpr HVal id(){ return {0, 0, 0}; }
  static constexpr LVal to_light(const HVal &v){ return {v.distsum, v.add, v.add}; }
  static constexpr HVal merge_heavy_light(const HVal &a, const LVal &b){ return {a.distsum + b.distsum, a.add + b.add, a.len}; }
  static constexpr HVal merge_heavy(const HVal &p, const HVal &c){ return {p.distsum + c.distsum + p.len * c.add, p.add + c.add, p.len + c.len}; }
  static constexpr LVal merge_light(const LVal &a, const LVal &b){ return {a.distsum + b.distsum, a.add + b.add, std::max(a.addmax, b.addmax)}; }
};

#endif
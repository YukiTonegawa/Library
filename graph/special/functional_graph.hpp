#ifndef _FUNCTIONAL_GRAPH_H_
#define _FUNCTIONAL_GRAPH_H_
#include <vector>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <queue>

//#include "../../minior/convert_directed_functional_graph.hpp"
//#include "../../algebraic_structure/monoid.hpp"

/*
struct functional_graph {
  private:
    using self_t = functional_graph;
    std::vector<int> dep, mdep, path, ord, root;

    int la(int v, int k) const {
        if (dep[v] < k) return -1;
        int h = mdep[v] - dep[v] + 1, t = h + k;
        while (2 * h <= t) {
            v = path[ord[v] + h];
            h *= 2;
        }
        return path[ord[v] + (t - h)];
    }
    
  public:
    functional_graph() {}
    functional_graph(const std::vector<int> &P) : dep(P.size(), 0), mdep(P.size(), 0), ord(P.size()), root(P.size(), -1) {
        int N = P.size();
        std::vector<int> deg(N, 0);
        auto dfs = [&](auto &&dfs, int v) -> int {
            if (root[v] != -1) return root[v];
            root[v] = v;
            root[v] = dfs(dfs, P[v]);
            mdep[v] = dep[v] = (root[v] == v ? 0 : dep[P[v]] + 1);
            if (dep[v]) deg[P[v]]++;
            return root[v];
        };
        std::queue<int> que;
        for (int i = 0; i < N; i++) dfs(dfs, i);
        for (int i = 0; i < N; i++) {
            if (deg[i] == 0) que.push(i);
        }
        while (!que.empty()) {
            int v = que.front();
            que.pop();
            if (dep[v] == 0) continue;
            int p = P[v];
            if ((--deg[p]) == 0) {
                deg[p] = 1 + v;
                mdep[p] = mdep[v];
                que.push(p);
            }
        }
        for (int i = 0; i < N; i++) {
            if (deg[i]) continue;
            int dbegin = dep[i], dend = -1;
            for (int v = i; ; v = P[v]) {
                if (dend == -1) ord[v] = path.size();
                path.push_back(v);
                if (dep[v] == 0 || dep[v] == dend) break;
                if (deg[P[v]] != v + 1) {
                    int len = dbegin - dep[v] + 1;
                    dend = std::max(0, dep[v] - len);
                }
            }
        }
        for (int i = 0; i < N; i++) {
            if (root[i] == i) {
                root[i] = P[i];
            }
        }
    }


    int size() const { return dep.size(); }

    int len_loop(int v) const {
        if (dep[v]) v = root[v];
        v = root[v];
        return dep[v] + 1;
    }

    int next(int v) const {
        return path[ord[v] + 1];
    }
    
    int next(int v, uint64_t k) const {
        if (k <= dep[v]) return la(v, k);
        k -= dep[v] + 1;
        if (dep[v]) v = root[v];
        v = root[v];
        return la(v, k % (dep[v] + 1));
    }

    // O(NlogN)
    self_t pow(uint64_t k) {
        int N = size();
        std::vector<int> res(N);
        for (int i = 0; i < N; i++) res[i] = next(i, k);
        return self_t(res);
    }
    
    static self_t e(int _N) {
        std::vector<int> tmp(_N, 0);
        std::iota(tmp.begin(), tmp.end(), 0);
        return self_t(tmp);
    }

    // g(f)
    static self_t composition(const self_t &g, const self_t &f) {
        assert(g.size() == f.size());
        int N = f.size();
        std::vector<int> res(N);
        for (int i = 0; i < N; i++) res[i] = g.next(f.next(i));
        return self_t(res);
    }
};
*/
/*
struct functional_graph {
  private:
    using self_t = functional_graph;
    int N;
    std::vector<int> P;

    static constexpr int bit(uint64_t x) { return x ? 64 - __builtin_clzll(x) : 0; }
    
    // [0, 2^r)
    void build(int r) {
        if (P.size() >= N * r) return;
        int l = P.size() / N;
        P.resize(N * r);
        while (l < r) {
            int s = (l - 1) * N;
            for (int j = 0; j < N; j++) {
                P[s + N + j] = P[s + P[s + j]];
            }
            l++;
        }
    }
    
  public:
    functional_graph() {}
    functional_graph(const std::vector<int> &_P) : N(_P.size()), P(_P) {}

    int size() const { return N; }

    int next(int p, uint64_t k) {
        build(bit(k));
        int s = 0;
        while (k) {
            if (k & 1) p = P[s + p];
            s += N;
            k >>= 1;
        }
        return p;
    }

    self_t pow(uint64_t k) {
        build(bit(k));
        std::vector<int> res(N);
        std::iota(res.begin(), res.end(), 0);
        int s = 0;
        while (k) {
            if (k & 1) {
                for (int i = 0; i < N; i++) {
                    res[i] = P[s + res[i]];
                }
            }
            s += N;
            k >>= 1;
        }
        return self_t(res);
    }

    static self_t e(int _N) {
        std::vector<int> tmp(_N, 0);
        std::iota(tmp.begin(), tmp.end(), 0);
        return self_t(tmp);
    }

    // g(f)
    static self_t composition(const self_t &g, const self_t &f) {
        assert(g.size() == f.size());
        int _N = f.size();
        self_t res(std::vector<int>(f.P.begin(), f.P.begin() + _N));
        for (int i = 0; i < _N; i++) res.P[i] = g.P[res.P[i]];
        return res;
    }
};
*/

/*
struct functional_graph {
  private:
    int n, h = 0;
    uint64_t MAX_K;
  using vec = std::vector<int>;
  std::vector<vec> v;
  std::vector<int> l, d, drank;
  static vec composition(const vec &a, const vec &b){
    vec ret(a.size());
    for(int i = 0; i < a.size(); i++) ret[i] = a[b[i]];
    return ret;
  }
  void init_loop(){
    l.resize(n, -1), d.resize(n, -1), drank.resize(n, -1);
    std::vector<bool> used(n, false);
    auto f = [&](auto &&f, int cur, std::queue<int> &q){
      if(used[cur]) return;
      used[cur] = 1;
      q.push(cur);
      int next = v[0][cur];
      f(f, next, q);
      // すでに探索済みのループ
      if(l[next] != -1){
        if(l[cur] == -1){
          l[cur] = l[next], d[cur] = d[next] + 1;
        }
        return;
      }
      // 新しいループ
      while(q.front() != next) q.pop();
      int len = q.size();
      while(!q.empty()){
        int u = q.front();
        l[u] = len, d[u] = 0;
        q.pop();
      }
    };
    auto g = [&](auto &&g, int cur, int k){
      if(d[cur]) return;
      if(drank[cur] != -1) return;
      drank[cur] = k;
      g(g, v[0][cur], k + 1);
    };
    std::queue<int> que;
    for(int i = 0; i < n; i++) f(f, i, que);
    for(int i = 0; i < n; i++) g(g, i, 0);
  }
  // 頂点iのループ部分でのランク(ループ部分でない場合は-1)
  int __rank_loop(int i){
    if(d.empty()) init_loop();
    return drank[i];
  }
public:
  functional_graph(){}
  functional_graph(const vec &_v, uint64_t _MAX_K): n(_v.size()), MAX_K(_MAX_K){
    while(_MAX_K) _MAX_K /= 2, h++;
    v.resize(h, vec(n));
    if(h) v[0] = _v;
    for(int i = 1; i < h; i++) v[i] = composition(v[i - 1], v[i - 1]);
  }
  // iからk回遷移した状態
  int move(int i, uint64_t k){
    assert(0 <= k && k <= MAX_K);
    for(int j = h - 1; k && j >= 0; j--){
      if(((uint64_t)1 << j) <= k){
        i = v[j][i];
        k -= (uint64_t)1 << j;
      }
    }
    return i;
  }
  // iから辿り着けるループの周期
  int len_loop(int i){
    if(l.empty()) init_loop();
    return l[i];
  }
  // iからループまでの距離
  int to_loop(int i){
    if(d.empty()) init_loop();
    return d[i];
  }
};

template<typename monoid>
struct functional_graph_monoid{
  using Val = typename monoid::Val;
  static constexpr auto id = monoid::id;
  static constexpr auto merge = monoid::merge;
private:
  int n, h = 0;
  uint64_t MAX_K;
  using vec = std::vector<std::pair<int, Val>>;
  std::vector<vec> v;
  std::vector<int> l, d, drank;
  static vec composition(const vec &a, const vec &b){
    vec ret(a.size());
    for(int i = 0; i < a.size(); i++){
      ret[i].first = a[b[i].first].first;
      ret[i].second = merge(b[i].second, a[b[i].first].second);
    }
    return ret;
  }
  void build(const vec &_v, uint64_t _MAX_K){
    n = _v.size();
    MAX_K = _MAX_K;
    while(_MAX_K) _MAX_K /= 2, h++;
    v.resize(h, vec(n));
    if(h) v[0] = _v;
    for(int i = 1; i < h; i++) v[i] = composition(v[i - 1], v[i - 1]);
  }
  void init_loop(){
    l.resize(n, -1), d.resize(n, -1), drank.resize(n, -1);
    std::vector<bool> used(n, false);
    auto f = [&](auto &&f, int cur, std::queue<int> &q){
      if(used[cur]) return;
      used[cur] = 1;
      q.push(cur);
      int next = v[0][cur].first;
      f(f, next, q);
      // すでに探索済みのループ
      if(l[next] != -1){
        if(l[cur] == -1){
          l[cur] = l[next], d[cur] = d[next] + 1;
        }
        return;
      }
      // 新しいループ
      while(q.front() != next) q.pop();
      int len = q.size();
      while(!q.empty()){
        int u = q.front();
        l[u] = len, d[u] = 0;
        q.pop();
      }
    };
    auto g = [&](auto &&g, int cur, int k){
      if(d[cur]) return;
      if(drank[cur] != -1) return;
      drank[cur] = k;
      g(g, v[0][cur].first, k + 1);
    };
    std::queue<int> que;
    for(int i = 0; i < n; i++) f(f, i, que);
    for(int i = 0; i < n; i++) g(g, i, 0);
  }
  // 頂点iのループ部分でのランク(ループ部分でない場合は-1)
  int __rank_loop(int i){
    if(d.empty()) init_loop();
    return drank[i];
  }
public:
  functional_graph_monoid(){}
  functional_graph_monoid(const vec &_v, uint64_t _MAX_K){
    build(_v, _MAX_K);
  }
  functional_graph_monoid(const std::vector<int> &nxt, const std::vector<Val> &_v, uint64_t _MAX_K){
    assert(nxt.size() == _v.size());
    vec tmp(nxt.size());
    for(int i = 0; i < nxt.size(); i++) tmp[i] = {nxt[i], _v[i]};
    build(tmp, _MAX_K);
  }
  // f(k回遷移する間の積(始点も含める))がtrueになる最小のk
  template<typename F>
  std::tuple<uint64_t, int, Val> bisect(int i, const F &f){
    uint64_t k = 0;
    int pos = i;
    Val left = id();
    for(int j = h - 1; j >= 0; j--){
      Val nxt = merge(left, v[j][pos].second);
      if(f(nxt)) continue;
      left = nxt;
      pos = v[j][pos].first;
      k += uint64_t(1) << h;
    }
    return {k, pos, merge(left, v[0][pos].second)};
  }
  // iからk回遷移した状態
  int move(int i, uint64_t k){
    assert(0 <= k && k <= MAX_K);
    for(int j = h - 1; k && j >= 0; j--){
      if(((uint64_t)1 << j) <= k){
        i = v[j][i].first;
        k -= (uint64_t)1 << j;
      }
    }
    return i;
  }
  // iからk回遷移した状態, k回遷移する間の積(始点も含める)
  std::pair<int, Val> move2(int i, uint64_t k){
    assert(0 <= k && k <= MAX_K);
    Val ret_val = id();
    for(int j = h - 1; k && j >= 0; j--){
      if(((uint64_t)1 << j) <= k){
        ret_val = merge(ret_val, v[j][i].second);
        i = v[j][i].first;
        k -= (uint64_t)1 << j;
      }
    }
    return {i, merge(ret_val, v[0][i].second)};
  }
  // iから辿り着けるループの周期
  int len_loop(int i){
    if(l.empty()) init_loop();
    return l[i];
  }
  // iからループまでの距離
  int to_loop(int i){
    if(d.empty()) init_loop();
    return d[i];
  }
  // 無向functional_graphでのi -> jの距離
  // 異なる連結成分にいる場合は-1(これ以外の場合で-1にならない)
  int dist_undirected(int i, int j){
    int d1 = to_loop(i), d2 = to_loop(j);
    int ret = 0;
    if(d1 > d2){
      i = move(i, d1 - d2);
      ret += d1 - d2;
      d1 = d2;
    }else{
      j = move(j, d2 - d1);
      ret += d2 - d1;
    }
    if(i == j) return ret; // 同じ枝
    ret += 2 * d1;
    i = move(i, d1);
    j = move(j, d1);
    int diff = __rank_loop(j) - __rank_loop(i);
    if(diff < 0) diff += len_loop(i);
    if(move(i, diff) != j) return -1; // 異なる連結成分
    return ret + std::min(diff, len_loop(i) - diff);
  }
  // 有向functional_graphでのi -> jの距離 移動が不可能な場合-1
  int dist_directed(int i, int j){
    int d1 = to_loop(i), d2 = to_loop(j);
    if(d2 > d1) return -1;
    i = move(i, d1 - d2);
    if(i == j) return d1 - d2;
    else if(d2) return -1; // i, jが違う枝にいる場合
    int diff = __rank_loop(j) - __rank_loop(i);
    if(diff < 0) diff += len_loop(i);
    if(move(i, diff) != j) return -1; // 異なる連結成分
    return d1 + diff;
  }
};

*/
// ダブリングの基数Bを変えると遅くなるがメモリを省ける
template<int B = 2>
struct functional_graph_arbitrary_radix{
private:
  int n, h = 0;
  uint64_t MAX_K;
  using vec = std::vector<int>;
  std::vector<vec> v;
  std::vector<int> l, d, drank;
  std::vector<uint64_t> bpow;
  static vec composition(const vec &a, const vec &b){
    vec ret(a.size());
    for(int i = 0; i < a.size(); i++) ret[i] = a[b[i]];
    return ret;
  }
  void init_loop(){
    l.resize(n, -1), d.resize(n, -1), drank.resize(n, -1);
    std::vector<bool> used(n, false);
    auto f = [&](auto &&f, int cur, std::queue<int> &q){
      if(used[cur]) return;
      used[cur] = 1;
      q.push(cur);
      int next = v[0][cur];
      f(f, next, q);
      // すでに探索済みのループ
      if(l[next] != -1){
        if(l[cur] == -1){
          l[cur] = l[next], d[cur] = d[next] + 1;
        }
        return;
      }
      // 新しいループ
      while(q.front() != next) q.pop();
      int len = q.size();
      while(!q.empty()){
        int u = q.front();
        l[u] = len, d[u] = 0;
        q.pop();
      }
    };
    auto g = [&](auto &&g, int cur, int k){
      if(d[cur]) return;
      if(drank[cur] != -1) return;
      drank[cur] = k;
      g(g, v[0][cur], k + 1);
    };
    std::queue<int> que;
    for(int i = 0; i < n; i++) f(f, i, que);
    for(int i = 0; i < n; i++) g(g, i, 0);
  }
  // 頂点iのループ部分でのランク(ループ部分でない場合は-1)
  int __rank_loop(int i){
    if(d.empty()) init_loop();
    return drank[i];
  }
public:
  functional_graph_arbitrary_radix(){}
  functional_graph_arbitrary_radix(const vec &_v, uint64_t _MAX_K): n(_v.size()), MAX_K(_MAX_K){
    while(_MAX_K) _MAX_K /= B, h++;
    v.resize(h, vec(n));
    bpow.resize(h);
    if(h) v[0] = _v, bpow[0] = 1;
    for(int i = 1; i < h; i++){
      assert(std::numeric_limits<uint64_t>::max() / B >= bpow[i - 1]);
      bpow[i] = bpow[i - 1] * B;
      v[i] = v[i - 1];
      for(int j = 0; j < B - 1; j++){
        v[i] = composition(v[i], v[i - 1]);
      }
    }
  }
  // iからk回遷移した状態
  int move(int i, uint64_t k){
    assert(0 <= k && k <= MAX_K);
    for(int j = h - 1; k && j >= 0; j--){
      while(bpow[j] <= k){
        i = v[j][i];
        k -= bpow[j];
      }
    }
    return i;
  }
  // iから辿り着けるループの周期
  int len_loop(int i){
    if(l.empty()) init_loop();
    return l[i];
  }
  // iからループまでの距離
  int to_loop(int i){
    if(d.empty()) init_loop();
    return d[i];
  }
  // 無向functional_graphでのi -> jの距離
  // 異なる連結成分にいる場合は-1(これ以外の場合で-1にならない)
  int dist_undirected(int i, int j){
    int d1 = to_loop(i), d2 = to_loop(j);
    int ret = 0;
    if(d1 > d2){
      i = move(i, d1 - d2);
      ret += d1 - d2;
      d1 = d2;
    }else{
      j = move(j, d2 - d1);
      ret += d2 - d1;
    }
    if(i == j) return ret; // 同じ枝
    ret += 2 * d1;
    i = move(i, d1);
    j = move(j, d1);
    int diff = __rank_loop(j) - __rank_loop(i);
    if(diff < 0) diff += len_loop(i);
    if(move(i, diff) != j) return -1; // 異なる連結成分
    return ret + std::min(diff, len_loop(i) - diff);
  }
  // 有向functional_graphでのi -> jの距離 移動が不可能な場合-1
  int dist_directed(int i, int j){
    int d1 = to_loop(i), d2 = to_loop(j);
    if(d2 > d1) return -1;
    i = move(i, d1 - d2);
    if(i == j) return d1 - d2;
    else if(d2) return -1; // i, jが違う枝にいる場合
    int diff = __rank_loop(j) - __rank_loop(i);
    if(diff < 0) diff += len_loop(i);
    if(move(i, diff) != j) return -1; // 異なる連結成分
    return d1 + diff;
  }
};

/*
// ダブリングの基数Bを変えると遅くなるがメモリを省ける
template<typename monoid, int B = 2>
struct functional_graph_monoid_arbitrary_radix{
  using Val = typename monoid::Val;
  static constexpr auto id = monoid::id;
  static constexpr auto merge = monoid::merge;
private:
  int n, h = 0;
  uint64_t MAX_K;
  using vec = std::vector<std::pair<int, Val>>;
  std::vector<vec> v;
  std::vector<int> l, d, drank;
  std::vector<uint64_t> bpow;
  static vec composition(const vec &a, const vec &b){
    vec ret(a.size());
    for(int i = 0; i < a.size(); i++){
      ret[i].first = a[b[i].first].first;
      ret[i].second = merge(b[i].second, a[b[i].first].second);
    }
    return ret;
  }
  void build(const vec &_v, uint64_t _MAX_K){
    n = _v.size();
    MAX_K = _MAX_K;
    while(_MAX_K) _MAX_K /= B, h++;
    v.resize(h, vec(n));
    bpow.resize(h);
    if(h) v[0] = _v, bpow[0] = 1;
    for(int i = 1; i < h; i++){
      assert(std::numeric_limits<uint64_t>::max() / B >= bpow[i - 1]);
      bpow[i] = bpow[i - 1] * B;
      v[i] = v[i - 1];
      for(int j = 0; j < B - 1; j++){
        v[i] = composition(v[i], v[i - 1]);
      }
    }
  }
  void init_loop(){
    l.resize(n, -1), d.resize(n, -1), drank.resize(n, -1);
    std::vector<bool> used(n, false);
    auto f = [&](auto &&f, int cur, std::queue<int> &q){
      if(used[cur]) return;
      used[cur] = 1;
      q.push(cur);
      int next = v[0][cur].first;
      f(f, next, q);
      // すでに探索済みのループ
      if(l[next] != -1){
        if(l[cur] == -1){
          l[cur] = l[next], d[cur] = d[next] + 1;
        }
        return;
      }
      // 新しいループ
      while(q.front() != next) q.pop();
      int len = q.size();
      while(!q.empty()){
        int u = q.front();
        l[u] = len, d[u] = 0;
        q.pop();
      }
    };
    auto g = [&](auto &&g, int cur, int k){
      if(d[cur]) return;
      if(drank[cur] != -1) return;
      drank[cur] = k;
      g(g, v[0][cur].first, k + 1);
    };
    std::queue<int> que;
    for(int i = 0; i < n; i++) f(f, i, que);
    for(int i = 0; i < n; i++) g(g, i, 0);
  }
  // 頂点iのループ部分でのランク(ループ部分でない場合は-1)
  int __rank_loop(int i){
    if(d.empty()) init_loop();
    return drank[i];
  }
public:
  functional_graph_monoid_arbitrary_radix(){}
  functional_graph_monoid_arbitrary_radix(const vec &_v, uint64_t _MAX_K){
    build(_v, _MAX_K);
  }
  functional_graph_monoid_arbitrary_radix(const std::vector<int> &nxt, const std::vector<Val> &_v, uint64_t _MAX_K){
    assert(nxt.size() == _v.size());
    vec tmp(nxt.size());
    for(int i = 0; i < nxt.size(); i++) tmp[i] = {nxt[i], _v[i]};
    build(tmp, _MAX_K);
  }
  // f(k回遷移する間の積(始点も含める))がtrueになる最小のk
  template<typename F>
  std::tuple<uint64_t, int, Val> bisect(int i, const F &f){
    uint64_t k = 0;
    int pos = i;
    Val left = id();
    for(int j = h - 1; j >= 0; j--){
      for(int t = 0; t < B - 1; t++){
        Val nxt = merge(left, v[j][pos].second);
        if(f(nxt)) break;
        left = nxt;
        pos = v[j][pos].first;
        k += bpow[j];
      }
    }
    return {k, pos, merge(left, v[0][pos].second)};
  }
  int move(int i, uint64_t k){
    assert(0 <= k && k <= MAX_K);
    for(int j = h - 1; k && j >= 0; j--){
      while(bpow[j] <= k){
        i = v[j][i].first;
        k -= bpow[j];
      }
    }
    return i;
  }
  // iからk回遷移した状態, k回遷移する間の積(始点も含める)
  std::pair<int, Val> move2(int i, uint64_t k){
    assert(0 < k && k <= MAX_K);
    Val ret_val = monoid::template id<Val>();
    for(int j = h - 1; k && j >= 0; j--){
      while(bpow[j] <= k){
        ret_val = merge(ret_val, v[j][i].second);
        i = v[j][i].first;
        k -= bpow[j];
      }
    }
    return {i, merge(ret_val, v[0][i].second)};
  }
  // iから辿り着けるループの周期
  int len_loop(int i){
    if(l.empty()) init_loop();
    return l[i];
  }
  // iからループまでの距離
  int to_loop(int i){
    if(d.empty()) init_loop();
    return d[i];
  }
  // 無向functional_graphでのi -> jの距離
  // 異なる連結成分にいる場合は-1(これ以外の場合で-1にならない)
  int dist_undirected(int i, int j){
    int d1 = to_loop(i), d2 = to_loop(j);
    int ret = 0;
    if(d1 > d2){
      i = move(i, d1 - d2);
      ret += d1 - d2;
      d1 = d2;
    }else{
      j = move(j, d2 - d1);
      ret += d2 - d1;
    }
    if(i == j) return ret; // 同じ枝
    ret += 2 * d1;
    i = move(i, d1);
    j = move(j, d1);
    int diff = __rank_loop(j) - __rank_loop(i);
    if(diff < 0) diff += len_loop(i);
    if(move(i, diff) != j) return -1; // 異なる連結成分
    return ret + std::min(diff, len_loop(i) - diff);
  }
  // 有向functional_graphでのi -> jの距離 移動が不可能な場合-1
  int dist_directed(int i, int j){
    int d1 = to_loop(i), d2 = to_loop(j);
    if(d2 > d1) return -1;
    i = move(i, d1 - d2);
    if(i == j) return d1 - d2;
    else if(d2) return -1; // i, jが違う枝にいる場合
    int diff = __rank_loop(j) - __rank_loop(i);
    if(diff < 0) diff += len_loop(i);
    if(move(i, diff) != j) return -1; // 異なる連結成分
    return d1 + diff;
  }
};
*/
#endif
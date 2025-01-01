/*
#include ".lib/template.hpp"
#include ".lib/data_structure/bit_sequence/bit_operation.hpp"

struct dynamic_bitvector_sqrt{
  using Int = unsigned long long;
  static constexpr int w = 64, wmod = 63, wdiv = 6;
  int sz_all;
  struct block{
    int sz_block, pop_block;
    std::vector<Int> small;
    std::vector<size_t> sz;
  };
  std::vector<block> large;
  int size(){
    return sz_all;
  }
  // 大きくなったblockを2つに分割
  void split_block(int k){
    int m = large[k].small.size() / 2;
    //large.insert(large.begin() + k + 1, )
    large[k].small.erase(large[k].small.begin() + m, large[k].small.end());
    large[k].sz.erase(large[k].sz.begin() + m, large[k].sz.end());
  }
  // [0, r)の1の数
  int rank1(int r){
    int i = 0, res = 0;
    while(r && r >= large[i].sz_block){
      res += large[i].pop_block;
      r -= large[i++].sz_block;
    }
    int j = 0;
    while(r && r >= large[i].sz[j]){
      res += __builtin_popcountll(large[i].small[j]);
      r -= large[i].sz[j++];
    }
    if(r) res += __builtin_popcountll(large[i].small[j] << (w - r));
    return res;
  }
  // [0, r)の0の数
  int rank0(int r){
    int i = 0, res = 0;
    while(r && r >= large[i].sz_block){
      res += (large[i].sz_block - large[i].pop_block);
      r -= large[i++].sz_block;
    }
    int j = 0;
    while(r && r >= large[i].sz[j]){
      res += large[i].sz[j] - __builtin_popcountll(large[i].small[j]);
      r -= large[i].sz[j++];
    }
    if(r) res += r - __builtin_popcountll(large[i].small[j] << (w - r));
    return res;
  }
  // k番目の1の位置(無い場合-1)
  int select1(int k){
    int i = 0, res = 0;
    while(i < large.size() && k > large[i].pop_block){
      res += large[i].sz_block;
      k -= large[i++].pop_block;
    }
    if(i == large.size()) return -1;
    int j = 0, tmp;
    while(k > (tmp = __builtin_popcountll(large[i].small[j]))){
      k -= tmp;
      j++;
    }
    return res + select_64bit(large[i].small[j], k);
  }
  // k番目の0
  int select0(int k){
    int i = 0, res = 0;
    while(i < large.size() && k > large[i].sz_block - large[i].pop_block){
      res += large[i].sz_block;
      k -= large[i].sz_block - large[i++].pop_block;
    }
    if(i == large.size()) return -1;
    int j = 0, tmp;
    while(k > (tmp = large[i].sz[j] - __builtin_popcountll(large[i].small[j]))){
      k -= tmp;
      j++;
    }
    return res + select_64bit(~large[i].small[j], k);
  }
  // k番目の要素
  bool access(int k){
    int i = 0;
    while(k && k >= large[i].sz_block){
      k -= large[i++].sz_block;
    }
    int j = 0;
    while(k && k >= large[i].sz[j]){
      k -= large[i].sz[j++];
    }
    return (large[i].small[j] >> k) & 1;
  }
  // k番目にfを追加
  void insert(int k, bool f){
    //
  }
  // k番目の要素を削除して消した要素を返す
  bool erase(int k){
    //
  }
  dynamic_bitvector_sqrt(){}
  dynamic_bitvector_sqrt(const std::vector<bool> &_v){
    //
  }
};


struct dynamic_wavelet_tree{
private:
  using bitvector = dynamic_bitvector_sqrt;
  int n, h, inf;
  std::vector<bitvector> bv;
  void build(std::vector<int> v){
    bv.resize((1 << h) - 1);
    std::queue<std::array<int, 4>> q;// (dep, v上でのl, r)
    std::vector<int> tmp(n);
    if(h) q.push({h - 1, 0, n, 0});
    while(!q.empty()){
      auto [d, l, r, k] = q.front();
      q.pop();
      std::vector<bool> bits(r - l);
      int lidx = 0, ridx = 0;
      for(int i = l; i < r; i++){
        bool b = (v[i] >> d) & 1;
        bits[i - l] = b;
        if(b) tmp[ridx++] = v[i];
        else v[l + lidx++] = v[i];
      }
      for(int i = 0; i < ridx; i++) v[l + lidx + i] = tmp[i];
      bv[k] = bitvector(bits);
      if(d){
        int mid = l + lidx;
        q.push({d - 1, l, mid, k * 2 + 1});
        q.push({d - 1, mid, r, k * 2 + 2});
      }
    }
  }
  int range_freq(int l, int r, int b, const int s, const int t, int S, int T){
    if(l == r || t <= S || T <= s) return 0;
    if(s <= S && T <= t) return r - l;
    int l0 = (!l ? 0 : bv[b].rank0(l)), l1 = l - l0;
    int r0 = bv[b].rank0(r), r1 = r - r0;
    return range_freq(l0, r0, b * 2 + 1, s, t, S, (S + T) / 2) +
            range_freq(l1, r1, b * 2 + 2, s, t, (S + T) / 2, T);
  }
  int rank(int r, int x, int b, int d){
    if(!r) return 0;
    int f = (x >> d) & 1;
    r = f ? bv[b].rank1(r) : bv[b].rank0(r);
    return !d ? r : rank(r, x, b * 2 + 1 + f, d - 1);
  }
  int select(int k, int x, int b, int d){
    bool f = (x >> d) & 1;
    int nxt_k = f ? bv[b].select1(k) : bv[b].select0(k);
    if(nxt_k == -1) return -1;
    return !b ? nxt_k : select(nxt_k, x, (b - 1) >> 1, d + 1);
  }
  int select_erase(int k, int x, int b, int d){
    bool f = (x >> d) & 1;
    int nxt_k = f ? bv[b].select1(k) : bv[b].select0(k);
    assert(nxt_k != -1);
    bv[b].erase(nxt_k);
    return !b ? nxt_k : select_erase(nxt_k, x, (b - 1) >> 1, d + 1);
  }
  int access(int k, int b, int d){
    int f = bv[b].access(k);
    int nxt_k = f ? bv[b].rank1(k) : bv[b].rank0(k);
    return (f << d) + (d ? access(nxt_k, b * 2 + 1 + f, d - 1) : 0);
  }
  int find_next(int k, int x, int b, int d){
    int f = (x >> d) & 1;
    int nxt_k = f ? bv[b].rank1(k) : bv[b].rank0(k);
    return (f << d) + (d ? access(nxt_k, b * 2 + 1 + f, d - 1) : 0);
  }
  void insert(int k, int x, int b, int d){
    bool f = (x >> d) & 1;
    int nxt_k = f ? bv[b].rank1(k) : bv[b].rank0(k);
    bv[b].insert(k, f);
    if(d) insert(nxt_k, x, b * 2 + 1 + f, d - 1);
  }
  int erase(int k, int b, int d){
    int f = bv[b].erase(k);
    int nxt_k = f ? bv[b].rank1(k) : bv[b].rank0(k);
    return (f << d) + (d ? erase(nxt_k, b * 2 + 1 + f, d - 1) : 0);
  }
  int range_kth_smallest(int l, int r, int k, int b, int d){
    int l0 = bv[b].rank0(l), l1 = l - l0;
    int r0 = bv[b].rank0(r), r1 = r - r0;
    int z = r0 - l0;
    if(z <= k) return (1 << d) + (d ? range_kth_smallest(l1, r1, k - z, b * 2 + 2, d - 1) : 0);
    else return (d ? range_kth_smallest(l0, r0, k, b * 2 + 1, d - 1) : 0);
  }
  std::pair<int, int> range_kth_smallest_super(int l, int r, const int s, const int t, int S, int T, int k, int b, int d){
    if(l == r || t <= S || T <= s) return {-1, 0};
    if(s <= S && T <= t){
      if(r - l <= k) return {-1, r - l};
      if(d == -1) return {0, r - l};
    }
    int l0 = (!l ? 0 : bv[b].rank0(l)), l1 = l - l0;
    int r0 = bv[b].rank0(r), r1 = r - r0;
    auto L = range_kth_smallest_super(l0, r0, s, t, S, (S + T) >> 1, k, b * 2 + 1, d - 1);
    if(L.first != -1) return L;
    auto R = range_kth_smallest_super(l1, r1, s, t, (S + T) >> 1, T, k - L.second, b * 2 + 2, d - 1);
    if(R.first == -1) R.second += L.second;
    else R.first += 1 << d;
    return R;
  }
  std::pair<int, int> range_kth_largest_super(int l, int r, const int s, const int t, int S, int T, int k, int b, int d){
    if(l == r || t <= S || T <= s) return {-1, 0};
    if(s <= S && T <= t){
      if(r - l <= k) return {-1, r - l};
      if(d == -1) return {0, r - l};
    }
    int l0 = (!l ? 0 : bv[b].rank0(l)), l1 = l - l0;
    int r0 = bv[b].rank0(r), r1 = r - r0;
    auto R = range_kth_largest_super(l1, r1, s, t, (S + T) >> 1, T, k, b * 2 + 2, d - 1);
    if(R.first != -1){
      R.first += 1 << d;
      return R;
    }
    auto L = range_kth_largest_super(l0, r0, s, t, S, (S + T) >> 1, k - R.second, b * 2 + 1, d - 1);
    if(L.first == -1) L.second += R.second;
    return L;
  }
  void count_prefix(int l, int r, int x, std::vector<int> &ret){
    for(int i = h - 1, j = 0; i >= 0 && l < r; i--){
      int l0 = bv[j].rank0(l), r0 = bv[j].rank0(r);
      if((x >> i) & 1){
        ret[h - 1 - i] = r0 - l0;
        l = l - l0, r = r - r0;
        j = j * 2 + 2;
      }else{
        ret[h - 1 - i] = (r - l) - (r0 - l0);
        l = l0, r = r0;
        j = j * 2 + 1;
      }
    }
    ret[h] = r - l;
  }
public:
  dynamic_wavelet_tree(){}
  dynamic_wavelet_tree(const std::vector<int> &v, int inf): n(v.size()), inf(inf){
    assert(inf > 1);
    h = 0;
    while((1 << h) < inf) h++;
    build(v);
  }
  int size(){
    return n;
  }
  int access(int k){
    assert(0 <= k && k < n);
    return access(k, 0, h - 1);
  }
  // [0, r)のcの数
  int rank(int r, int c){
    assert(0 <= r && r <= n);
    assert(0 <= c && c < inf);
    return rank(r, c, 0, h - 1);
  }
  // k番目のc, ない場合は-1
  int select(int k, int c){
    assert(0 <= k && k < n);
    assert(0 <= c && c < inf);
    int left_leaf = (1 << h) - 1;
    return select(k, c, (left_leaf + c - 1) >> 1, 0);
  }
  // k番目のcの位置を返してそれを削除する
  // k < count(c)でない状態で使うと壊れる
  int select_erase(int k, int c){
    assert(0 <= k && k < n);
    assert(0 <= c && c < inf);
    int left_leaf = (1 << h) - 1;
    int ret = select_erase(k, c, (left_leaf + c - 1) >> 1, 0);
    n = bv[0].size();
    return ret;
  }
  // k以降(k含む)のc, 無い場合は-1
  int find_next(int k, int c){
    if(c < 0 || c >= inf) return -1;
    return select(rank(k, c), c);
  }
  // k以前(k含む)のc, 無い場合は-1
  int find_prev(int k, int c){
    if(c < 0 || c >= inf) return -1;
    int r = rank(k + 1, c);
    if(r == 0) return -1;
    return select(r - 1, c);
  }
  // k番目にcを挿入
  void insert(int k, int c){
    assert(0 <= k && k <= n);
    assert(0 <= c && c < inf);
    insert(k, c, 0, h - 1);
    n = bv[0].size();
  }
  // k番目を削除してその値を返す
  int erase(int k){
    assert(0 <= k && k < n);
    int ret = erase(k, 0, h - 1);
    n = bv[0].size();
    return ret;
  }
  // k番目の値をcにする
  void update(int k, int c){
    erase(k);
    insert(k, c);
  }
  // [l, r)の[s, t)の数
  int range_freq(int l, int r, int s, int t){
    assert(0 <= l && r <= n);
    assert(0 <= s && t <= inf);
    if(l >= r || s >= t) return 0;
    return range_freq(l, r, 0, s, t, 0, 1 << h);
  }
  // [l, r)でk番目に小さい要素(ない場合は-1)
  int range_kth_smallest(int l, int r, int k){
    assert(0 <= l && r <= n);
    if(k >= r - l) return -1;
    return range_kth_smallest(l, r, k, 0, h - 1);
  }
  // [l, r)でk番目に大きい要素(ない場合は-1)
  int range_kth_largest(int l, int r, int k){
    assert(0 <= l && r <= n);
    if(k >= r - l) return -1;
    return range_kth_smallest(l, r, r - l - 1 - k, 0, h - 1);
  }
  // [l, r)で値が[s, t)の要素でk番目に大きい要素とその個数(ない場合は-1)
  std::pair<int, int> range_kth_smallest_super(int l, int r, int s, int t, int k){
    assert(0 <= l && r <= n);
    assert(0 <= s && t <= inf);
    return range_kth_smallest_super(l, r, s, t, 0, 1 << h, k, 0, h - 1);
  }
  // [l, r)で値が[s, t)の要素でk番目に大きい要素とその個数(ない場合は-1)
  std::pair<int, int> range_kth_largest_super(int l, int r, int s, int t, int k){
    assert(0 <= l && r <= n);
    assert(0 <= s && t <= inf);
    return range_kth_largest_super(l, r, s, t, 0, 1 << h, k, 0, h - 1);
  }
  // 区間[l, r)のx以上の最小要素(ない場合は-1)
  int range_lower_bound(int l, int r, int x){
    assert(0 <= l && r <= n);
    assert(0 <= x && x < inf);
    int cnt = range_freq(l, r, 0, x);
    return range_kth_smallest(l, r, cnt);
  }
  // 区間[l, r)のx以下の最大要素(ない場合は-1)
  int range_lower_bound_rev(int l, int r, int x){
    assert(0 <= l && r <= n);
    assert(0 <= x && x < inf);
    int cnt = range_freq(l, r, 0, x + 1);
    if(cnt == 0) return -1;
    return range_kth_smallest(l, r, cnt - 1);
  }
  // [l, r)で値が[s, t)の要素でインデックス順にk番目, ない場合は-1
  int range_select(int l, int r, int s, int t, int k){
    if(range_freq(l, r, s, t) <= k) return -1;
    int L = l;
    while(r - l > 1){
      int mid_r = (l + r) / 2;
      if(range_freq(L, mid_r, s, t) > k) r = mid_r;
      else l = mid_r;
    }
    return r - 1;
  }
  // ret[i] := [l, r)でh-bitの2進数で見たときxとのlcpがちょうどiの要素の数
  std::vector<int> count_prefix(int l, int r, int x){
    std::vector<int> ret(h + 1, 0);
    count_prefix(l, r, x, ret);
    return ret;
  }
  std::vector<int> to_list(){
    std::vector<int> pos(n), res(n, 0);
    std::iota(pos.begin(), pos.end(), 0);
    std::vector<int> tmpL, tmpR;
    auto f = [&](auto &&f, int k, int l, int r, int d) -> void {
      if(k >= (1 << h) - 1) return;
      auto v = bv[k].to_list();
      assert(v.size() == r - l);
      for(int i = 0; i < r - l; i++){
        if(v[i]){
          tmpR.push_back(pos[i + l]);
          res[pos[i + l]] += 1 << d;
        }else{
          tmpL.push_back(pos[i + l]);
        }
      }
      int j = l, lsz = tmpL.size();
      for(int i = 0; i < tmpL.size(); i++) pos[j++] = tmpL[i];
      for(int i = 0; i < tmpR.size(); i++) pos[j++] = tmpR[i];
      tmpL.clear();
      tmpR.clear();
      v.clear();
      if(lsz) f(f, k * 2 + 1, l, l + lsz, d - 1);
      if(lsz != r - l) f(f, k * 2 + 2, l + lsz, r, d - 1);
    };
    f(f, 0, 0, n, h - 1);
    return res;
  }
};


int main(){
  io_init();
  int n;
  std::cin >> n;
  vector<int> allval;
  auto a = read_vec<int>(n);
  allval = a;
  vector<tuple<int, int, int>> Q;
  int q;
  std::cin >> q;
  range(i, 0, q){
    int t;
    std::cin >> t;
    if(t == 1){
      int x, y;
      std::cin >> x >> y;
      allval.push_back(y);
      Q.push_back({t, x, y});
    }else{
      int x;
      std::cin >> x;
      Q.push_back({t, x, -1});
    }
  }
  // compress
  sort(allof(allval));
  allval.erase(unique(allof(allval)), allval.end());
  range(i, 0, n) a[i] = lower_bound(allof(allval), a[i]) - allval.begin();
  dynamic_wavelet_tree wt(a, allval.size() + 1);
  range(i, 0, q){
    auto [t, x, y] = Q[i];
    if(t == 1){
      x = lower_bound(allof(allval), x) - allval.begin();
      y = lower_bound(allof(allval), y) - allval.begin();
      int idx = wt.select(0, x);
      wt.insert(idx + 1, y);
    }else{
      x = lower_bound(allof(allval), x) - allval.begin();
      wt.select_erase(0, x);
    }
  }
  int sz = wt.size();
  vector<int> ans = wt.to_list();
  range(i, 0, sz) ans[i] = allval[ans[i]];
  std::cout << ans << "\n";
}
*/
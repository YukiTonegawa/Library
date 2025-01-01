#ifndef _DYNAMIC_UPPER_HULL_BBST_H_
#define _DYNAMIC_UPPER_HULL_BBST_H_
#include <vector>
#include <algorithm>
#include <cassert>
#include <set>

// 同じ点が複数あると壊れる
// T2は(yの最大値) * (xの最大値)^2くらいになる
template<typename T, typename T2>
struct dynamic_upper_hull_bbst{
  struct point{
    T x, y;
    bool operator == (const point &p) const { return x == p.x && y == p.y; }
    bool operator != (const point &p) const { return x != p.x || y != p.y; }
    bool operator < (const point &p) const { return x < p.x || (x == p.x && y < p.y); }
    bool operator <= (const point &p) const { return x < p.x || (x == p.x && y <= p.y); }
    // 0だと一列
    // 0未満だと反時計回り
    // 0より大きいと時計回り
    static T2 cross(const point &a, const point &b, const point &c){
      return (T2)(b.y - a.y) * (c.x - a.x) - (T2)(c.y - a.y) * (b.x - a.x);
    }
  };
  struct node{
    int sz;
    node *lch, *rch, *p, *prev, *next;
    point lp, lbr, rbr;
    // 葉
    node(T x, T y): sz(1), lch(nullptr), rch(nullptr), p(nullptr), lp{x, y}, lbr{x, y}, rbr{x, y}{}
    // 葉以外(pullで初期化)
    node(): lch(nullptr), rch(nullptr), p(nullptr){}
    bool is_leaf(){ return !lch && !rch; }
  };
  // パラメータ
  static constexpr float alpha = 0.6;
  static constexpr int max_node_size = 1000000;

  int max_node_count;
  inline bool check_balanced_insert(node *v){
    int threshold = alpha * v->sz;
    return v->lch->sz <= threshold && v->rch->sz <= threshold;
  }
  inline bool check_balanced_erase(){
    return !root || (root->sz > alpha * max_node_count);
  }
  T dx, dy;
  node *root;
  int pos_stock;
  std::array<node*, max_node_size> nd_stock, nd_build;

  node *_pop(){
    if(!pos_stock) return new node();
    return nd_stock[--pos_stock];
  }
  void _push(node *v){ nd_stock[pos_stock++] = v;}

  std::tuple<node*, node*, node*> build(int l, int r){
    if(r - l == 1){
      node *res = nd_build[l];
      res->prev = res->next = res;
      return {res, res, res};
    }
    node *p = _pop();
    int m = (l + r) / 2;
    auto [left, left_lmost, left_rmost] = build(l, m);
    auto [right, right_lmost, right_rmost] = build(m, r);
    p->lch = left, left->p = p;
    p->rch = right, right->p = p;
    p->p = nullptr;
    pull_val(p);
    // 橋
    while(left_lmost != left_rmost || right_lmost != right_rmost){
      point a = left_rmost->prev->lbr, b = left_rmost->lbr;
      point c = right_lmost->lbr, d = right_lmost->next->lbr;
      if(left_lmost != left_rmost && point::cross(a, b, c) < 0){
        left_rmost = left_rmost->prev;
      }else if(right_lmost != right_rmost && point::cross(b, c, d) < 0){
        right_lmost = right_lmost->next;
      }else{
        break;
      }
    }
    p->lbr = left_rmost->lbr;
    p->rbr = right_lmost->lbr;
    left_rmost->next = right_lmost;
    right_lmost->prev = left_rmost;
    return {p, left_lmost, right_rmost};
  }
  node* rebuild(node *v){
    int pos_build = 0;
    auto dfs = [&](auto &&dfs, node *u) -> void {
      if(u->is_leaf()){
        nd_build[pos_build++] = u;
      }else{
        _push(u);
        dfs(dfs, u->lch);
        dfs(dfs, u->rch);
      }
    };
    dfs(dfs, v);
    return std::get<0>(build(0, pos_build));
  }
  void pull_val(node *v){
    v->sz = v->lch->sz + v->rch->sz;
    v->lp = v->lch->lp;
  }
  // vの左/右部分木への橋頂点を再計算
  void pull(node *v){
    node *l = v->lch, *r = v->rch;
    T mx = r->lp.x;
    while(!l->is_leaf() || !r->is_leaf()){
      point a = l->lbr, b = l->rbr;
      point c = r->lbr, d = r->rbr;
      T2 s1 = point::cross(a, b, c);
      if(a != b && s1 < 0){
        l = l->lch;
      }else if(c != d && point::cross(b, c, d) < 0){
        r = r->rch;
      }else if(a == b){
        r = r->lch;
      }else if(c == d){
        l = l->rch;
      }else{
        T2 s2 = point::cross(b, a, d);
        assert(s1 + s2 <= 0);
        if(s1 + s2 == 0 || s1 * (d.x - mx) > s2 * (mx - c.x)){
          l = l->rch;
        }else{
          r = r->lch;
        }
      }
    }
    v->lbr = l->lbr;
    v->rbr = r->lbr;
  }

  node *insert(node *v, point q){
    if(!v) return new node(q.x, q.y);
    if(v->is_leaf()){
      node *p = _pop();
      node *u = new node(q.x, q.y);
      v->p = u->p = p;
      p->lch = v, p->rch = u;
      if(q < v->lbr) std::swap(p->lch, p->rch);
      pull_val(p);
      pull(p);
      return p;
    }
    if(q < v->rch->lp){
      v->lch = insert(v->lch, q);
      v->lch->p = v;
    }else{
      v->rch = insert(v->rch, q);
      v->rch->p = v;
    }
    if(!check_balanced_insert(v)){
      return rebuild(v);
    }else{
      pull_val(v);
      pull(v);
      return v;
    }
  }
  void enumerate(node *v, std::vector<node*> &res){
    if(v->is_leaf()){
      res.push_back(v);
    }else{
      enumerate(v->lch, res);
      enumerate(v->rch, res);
    }
  }
  void get_hull(node *v, point L, point R, std::vector<node*> &res){
    if(v->is_leaf()){
      res.push_back(v);
      return;
    }
    if(R <= v->lbr){
      get_hull(v->lch, L, R, res);
    }else if(v->rbr <= L){
      get_hull(v->rch, L, R, res);
    }else{
      get_hull(v->lch, L, v->lbr, res);
      get_hull(v->rch, v->rbr, R, res);
    }
  }
public:
  // x座標の昇順にソート済 {x, y}
  dynamic_upper_hull_bbst(): max_node_count(0), dx(0), dy(0), root(nullptr), pos_stock(0){}
  dynamic_upper_hull_bbst(const std::vector<std::pair<T, T>>& val): max_node_count(0), dx(0), dy(0), root(nullptr), pos_stock(0){
    if(val.size()){
      for(int i = 0; i < val.size(); i++) nd_build[i] = new node(val[i].first, val[i].second);
      root = std::get<0>(build(0, val.size()));
      max_node_count = root->sz;
    }
  }

  void shift_all_x(T x){ dx += x; }
  void shift_all_y(T y){ dy += y; }

  // 分数として見た時の最大値を探す, 空の場合nullptr
   node *find_max_frac(){
    if(!root) return nullptr;
    node *v = root;
    while(!v->is_leaf()){
      T lx = v->lbr.x + dx, ly = v->lbr.y + dy;
      T rx = v->rbr.x + dx, ry = v->rbr.y + dy;
      if((T2)ly * rx <= (T2)lx * ry) v = v->rch;
      else v = v->lch;
    }
    return v;
  }

  // v(葉)を消す
  // 葉以外の頂点が2つの子を持つ構造を維持する
  void erase(node *v){
    assert(v->is_leaf());
    _push(v);
    node *p = v->p;
    if(!p){
      root = nullptr;
      return;
    }
    node *pp = p->p;
    node *c = (p->lch == v ? p->rch : p->lch);
    c->p = pp;
    if(pp){
      if(pp->lch == p){
        pp->lch = c;
      }else{
        pp->rch = c;
      }
    }else{
      root = c;
      return;
    }
    while(pp){
      pull_val(pp);
      if(pp->lbr == v->lbr || pp->rbr == v->lbr) pull(pp);
      pp = pp->p;
    }
    _push(p);
    if(!check_balanced_erase()) root = rebuild(root), max_node_count = root->sz;
  }
  void insert(T x, T y){
    root = insert(root, point{x - dx, y - dy});
    max_node_count++;
  }
  // v(葉)のy座標をyに変更
  void change_y(node *v, T y){
    node *p = v->p;
    point prev = v->lbr;
    v->lbr.y = v->rbr.y = y - dy;
    while(p){
      pull_val(p);
      if(prev.y + dy >= y || p->lbr == prev || p->rbr == prev) pull(p);
      p = p->p;
    }
  }
  std::pair<T, T> get_val(node *v){ return {v->lbr.x + dx, v->lbr.y + dy}; }
  
  std::vector<node*> enumerate(){
    if(!root) return {};
    std::vector<node*> res;
    enumerate(root, res);
    return res;
  }
  // 上側凸包の頂点を全て列挙
  std::vector<node*> get_hull(){
    static constexpr T minf = std::numeric_limits<T>::min();
    static constexpr T inf = std::numeric_limits<T>::max();
    if(!root) return {};
    std::vector<node*> res;
    get_hull(root, {minf, minf}, {inf, inf}, res);
    return res;
  }
  // x座標がxの点がが存在するか
  node *find_x(T x){
    x -= dx;
    if(!root) return nullptr;
    node *v = root;
    while(!v->is_leaf()){
      if(x < v->rch->lp.x){
        v = v->lch;
      }else{
        v = v->rch;
      }
    }
    return v->lbr.x == x ? v : nullptr;
  }
};
#endif
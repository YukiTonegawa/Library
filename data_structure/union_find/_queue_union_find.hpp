#ifndef _QUEUE_UNION_FIND_H_
#define _QUEUE_UNION_FIND_H_
#include <cassert>
#include <vector>
#include <algorithm>
struct link_cut_tree_dynamic_connectivity{
  static constexpr int inf = 1 << 30;
  struct node{
    node *l, *r, *p, *min_edge;
    bool flip;
    int size, subsize, deltime;
    node(int deltime): l(nullptr), r(nullptr), p(nullptr), min_edge(this), flip(false), size(deltime == inf + 1), subsize(0), deltime(deltime){}
    bool is_root(){
      return !p || (p->l != this && p->r != this);
    }
  };
private:
  static void update(node *v){
    v->min_edge = v;
    v->size = (v->deltime == inf + 1) + v->subsize;
    if(v->l){
      v->size += v->l->size;
      if(v->min_edge->deltime > v->l->min_edge->deltime) v->min_edge = v->l->min_edge;
    }
    if(v->r){
      v->size += v->r->size;
      if(v->min_edge->deltime > v->r->min_edge->deltime) v->min_edge = v->r->min_edge;
    }
  }
  static void push_down(node *v){
    if(v->flip){
      if(v->l) flip(v->l);
      if(v->r) flip(v->r);
      v->flip = false;
    }
  }
  static void flip(node *v){
    std::swap(v->l, v->r);
    v->flip ^= 1;
  }
  static void rotate_right(node *v){
    node *p = v->p, *pp = p->p;
    if((p->l = v->r)) v->r->p = p;
    v->r = p, p->p = v;
    update(p), update(v);
    if((v->p = pp)){
      if(pp->l == p) pp->l = v;
      if(pp->r == p) pp->r = v;
      update(pp);
    }
  }
  static void rotate_left(node *v){
    node *p = v->p, *pp = p->p;
    if((p->r = v->l)) v->l->p = p;
    v->l = p, p->p = v;
    update(p), update(v);
    if((v->p = pp)){
      if(pp->l == p) pp->l = v;
      if(pp->r == p) pp->r = v;
      update(pp);
    }
  }
  static void splay(node *v){
    push_down(v);
    while(!v->is_root()){
      node *p = v->p;
      if(p->is_root()){
        push_down(p), push_down(v);
        if(p->l == v) rotate_right(v);
        else rotate_left(v);
      }else{
        node *pp = p->p;
        push_down(pp), push_down(p), push_down(v);
        if(pp->l == p){
          if(p->l == v) rotate_right(p);
          else rotate_left(v);
          rotate_right(v);
        }else{
          if(p->r == v) rotate_left(p);
          else rotate_right(v);
          rotate_left(v);
        }
      }
    }
  }
  static node *evert(node *v){
    expose(v);
    flip(v);
    push_down(v);
    return v;
  }
public:
  static node *make_node(){return new node(inf + 1);}
  static node *make_edge(int deltime){return new node(deltime);}
  static node *expose(node *v){
    node *c = nullptr;
    for(node *u = v; u; u = u->p){
      splay(u);
      if(u->r) u->subsize += u->r->size;
      if(c) u->subsize -= c->size;
      u->r = c;
      update(u);
      c = u;
    }
    splay(v);
    return c;
  }
  // 連結成分が同じか
  static bool same(node *u, node *v){
    if(!u || !v) return false;
    evert(u);
    expose(v);
    return u->p;
  }
  // vの削除時刻をtにする
  static void set_del(node *v, int t){
    expose(v);
    v->deltime = t;
    update(v);
  }
  static int size(node *v){
    expose(v);
    return v->size;
  }
  // 端点がs, tであるeを削除, 連結成分が増加したか
  static bool cut(node *e, node *s, node *t){
    if(e->deltime == -1) return false;
    evert(t);
    expose(s);
    splay(e);
    assert(e->l && e->r);
    e->l->p = e->r->p = nullptr;
    return true;
  }
  // 連結成分が減少したか
  static bool link(node *e, node *s, node *t){
    assert(s != t);
    assert(e->deltime != -1);
    bool res = true;
    evert(t);
    expose(s);
    // same(s, t)
    if(t->p){
      // (s, t)パス中で最小の辺
      node *f = s->min_edge;
      assert(f->deltime != inf + 1); // s-tが連結かつs!=t -> 1本以上の辺がある
      if(e->deltime <= f->deltime){
        e->deltime = -1;
        return false;
      }
      splay(f);
      f->l->p = f->r->p = nullptr;
      f->deltime = -1;
      res = false;
      splay(t);
      splay(s);
    }
    t->p = s->p = e;
    e->l = s, e->r = t;
    update(e);
    return res;
  }
  // s-tパスが切れる時刻, 非連結な場合は0
  static int cuttime(node *s, node *t){
    assert(s != t);
    int res = 0;
    evert(t);
    expose(s);
    // same(s, t)
    if(t->p){
      // (s, t)パス中で最小の辺
      node *f = s->min_edge;
      res = f->deltime + 1;
      assert(f->deltime != inf + 1); // s-tが連結かつs!=t -> 1本以上の辺がある
      splay(f);
    }
    return res;
  }
};

struct queue_union_find{
  using lct = link_cut_tree_dynamic_connectivity;
  using node = lct::node;
  std::vector<node*> V;
  int T;
  queue_union_find(int n): V(n), T(0){ for(int i = 0; i < n; i++) V[i] = lct::make_node(); }
  // 辺を末尾に追加
  void push_back(int u, int v){
    int t = V.size();
    node *e = lct::make_edge(t);
    lct::link(e, V[u], V[v]);
  }
  void pop_front(){
    assert(T < V.size());
    T++;
  }
  int size(){ return V.size() - T; }
  bool same(int u, int v){
    return T < lct::cuttime(V[u], V[v]);
  }
  // あとk回pop_frontしてもu-vが連結であるkの最大値(非連結な場合は-1)
  int max_pop_count(int u, int v){
    int t = lct::cuttime(V[u], V[v]);
    if(T >= t) return -1;
    return t - T - 1;
  }
};
#endif
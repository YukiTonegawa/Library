#ifndef _COMPRESS_BBST_H_
#define _COMPRESS_BBST_H_
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>

template<typename Key>
struct splay_tree{
  struct node{
    int sz;
    Key key, lkey, rkey;
    node *l, *r, *p;
    node(Key _key): sz(1), key(_key), lkey(key), rkey(key), l(nullptr), r(nullptr), p(nullptr){}
  };
  static int size(node *v){
    return !v ? 0 : v->sz;
  }
  static void update(node *v){
    v->sz = 1;
    v->lkey = v->rkey = v->key;
    if(v->l){
      v->sz += v->l->sz;
      v->lkey = v->l->lkey;
    }
    if(v->r){
      v->sz += v->r->sz;
      v->rkey = v->r->rkey;
    }
  }
  // pの左の子vをpの位置にする　
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
  // pの右の子vをpの位置にする　
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
  // vを根にする
  static void splay(node *v){
    if(!v) return;
    while(v->p){
      node *p = v->p;
      if(!p->p){
        if(p->l == v) rotate_right(v);
        else rotate_left(v);
      }else{
        node *pp = p->p;
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
  // 左端のノード
  static node *leftmost(node *v){
    while(v->l) v = v->l;
    splay(v);
    return v;
  }
  // 右端のノード
  static node *rightmost(node *v){
    while(v->r) v = v->r;
    splay(v);
    return v;
  }
  // k以上になる　最小のノード
  // ない場合は最も右側のノード
  static node *find(node *v, Key k){
    if(!v) return v;
    while(v){
      if(v->key < k){
        if(!v->r){
          splay(v);
          return v;
        }
        v = v->r;
      }else if(!v->l || v->l->rkey < k){
        splay(v);
        return v;
      }else{
        v = v->l;
      }
    }
    assert(false);
    return v;
  }
  static node *merge(node *a, node *b){
    if(!a || !b) return !a ? b : a;
    if(size(a) > size(b)){
      b = leftmost(b);
      b->l = a;
      a->p = b;
      update(b);
      return b;
    }else{
      a = rightmost(a);
      a->r = b;
      b->p = a;
      update(a);
      return a;
    }
  }
  // {k未満, k以上}
  static std::pair<node*, node*> split(node *v, Key k){
    if(!v) return {nullptr, nullptr};
    if(v->rkey < k) return {v, nullptr};
    node *u = find(v, k);
    node *l = u->l;
    assert(u);
    u->l = nullptr;
    if(l) l->p = nullptr;
    update(u);
    return {l, u};
  }
  // {l未満, l以上r未満, r以上}
  static std::tuple<node*, node*, node*> split3(node *v, Key l, Key r){
    if(!v) return {nullptr, nullptr, nullptr};
    if(v->rkey < r){
      auto [a, b] = split(v, l);
      return {a, b, nullptr};
    }
    auto [b, c] = split(v, r); // cの左が空
    assert(c);
    auto [a, b2] = split(b, l); // b2の左が空　
    return {a, b2, c};
  }
  // split3によって分割された直後の組でないと壊れる
  static node *merge3(node *a, node *b, node *c){
    if(!b){
      b = a;
    }else{
      if((b->l = a)){
        a->p = b;
        update(b);
      }
    }
    if(!c) return b;
    if((c->l = b)){
      b->p = c;
      update(c);
    }
    return c;
  }
  // keyを追加する, すでにある場合は何もしない
  static node *insert(node *v, Key key){
    if(!v) return new node(key);
    node *u = find(v, key);
    if(u->key == key) return u; // すでにある
    node *n = new node(key);
    if(u->key < key){
      n->l = u;
      u->p = n;
      update(n);
      return n;
    }else{
      node *l = u->l;
      if((n->l = l)){
        l->p = n;
        update(n);
      }
      u->l = n;
      n->p = u;
      update(u);
      return u;
    }
  }
  // keyを消す, ない場合は何もしない
  static node *erase(node *v, Key key){
    if(!v) return v;
    node *u = find(v, key);
    if(u->key != key) return u;
    node *l = u->l, *r = u->r;
    if(l) l->p = nullptr;
    if(r) r->p = nullptr;
    return merge(l, r);
  }
  bool exist(node *v, Key key){
    if(!v) return false;
    v = find(v, key);
    return v->key == key;
  }
};

template<typename Key, typename monoid>
struct splay_tree_monoid{
  using Val = typename monoid::Val;
  static constexpr auto id = monoid::id;
  static constexpr auto merge_val = monoid::merge;

  struct node{
    int sz;
    Key key, lkey, rkey;
    Val val, sum;
    node *l, *r, *p;
    node(Key _key, Val _val): sz(1), key(_key), lkey(key), rkey(key), val(_val), sum(val), l(nullptr), r(nullptr), p(nullptr){}
  };
  static int size(node *v){
    return !v ? 0 : v->sz;
  }
  static void update(node *v){
    v->sz = 1;
    v->lkey = v->rkey = v->key;
    v->sum = v->val;
    if(v->l){
      v->sz += v->l->sz;
      v->lkey = v->l->lkey;
      v->sum = merge_val(v->l->sum, v->sum);
    }
    if(v->r){
      v->sz += v->r->sz;
      v->rkey = v->r->rkey;
      v->sum = merge_val(v->sum, v->r->sum);
    }
  }
  // pの左の子vをpの位置にする　
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
  // pの右の子vをpの位置にする　
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
  // vを根にする
  static void splay(node *v){
    if(!v) return;
    while(v->p){
      node *p = v->p;
      if(!p->p){
        if(p->l == v) rotate_right(v);
        else rotate_left(v);
      }else{
        node *pp = p->p;
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
  // 左端のノード
  static node *leftmost(node *v){
    while(v->l) v = v->l;
    splay(v);
    return v;
  }
  // 右端のノード
  static node *rightmost(node *v){
    while(v->r) v = v->r;
    splay(v);
    return v;
  }
  // k以上になる　最小のノード
  // ない場合は最も右側のノード
  static node *find(node *v, Key k){
    if(!v) return v;
    while(v){
      if(v->key < k){
        if(!v->r){
          splay(v);
          return v;
        }
        v = v->r;
      }else if(!v->l || v->l->rkey < k){
        splay(v);
        return v;
      }else{
        v = v->l;
      }
    }
    assert(false);
    return v;
  }
  // {k未満, k以上}
  static std::pair<node*, node*> split(node *v, Key k){
    if(!v) return {nullptr, nullptr};
    if(v->rkey < k) return {v, nullptr};
    node *u = find(v, k);
    node *l = u->l;
    assert(u);
    u->l = nullptr;
    if(l) l->p = nullptr;
    update(u);
    return {l, u};
  }
  // {l未満, l以上r未満, r以上}
  static std::tuple<node*, node*, node*> split3(node *v, Key l, Key r){
    if(!v) return {nullptr, nullptr, nullptr};
    if(v->rkey < r){
      auto [a, b] = split(v, l);
      return {a, b, nullptr};
    }
    auto [b, c] = split(v, r); // cの左が空
    assert(c);
    auto [a, b2] = split(b, l); // b2の左が空　
    return {a, b2, c};
  }
  static node *merge(node *a, node *b){
    if(!a || !b) return !a ? b : a;
    if(!b->l || size(a) > size(b)){
      b = leftmost(b);
      b->l = a;
      a->p = b;
      update(b);
      return b;
    }else{
      a = rightmost(a);
      a->r = b;
      b->p = a;
      update(a);
      return a;
    }
  }
  // split3によって分割された直後の組でないと壊れる
  static node *merge3(node *a, node *b, node *c){
    if(!b){
      b = a;
    }else{
      if((b->l = a)){
        a->p = b;
        update(b);
      }
    }
    if(!c) return b;
    if((c->l = b)){
      b->p = c;
      update(c);
    }
    return c;
  }
  // {key, val}を追加する. すでにある場合
  // mode = 0 -> 何もしない
  // mode = 1 -> 上書き
  // mode = 2 -> マージ
  static node *insert(node *v, Key key, Val val, int mode){
    if(!v) return new node(key, val);
    node *u = find(v, key);
    assert(0 <= mode && mode <= 2);
    if(u->key == key){
      if(mode == 0) return u;
      else if(mode == 1) u->val = val;
      else if(mode == 2) u->val = merge_val(u->val, val);
      update(u);
      return u;
    }
    node *n = new node(key, val);
    if(u->key < key){
      n->l = u;
      u->p = n;
      update(n);
      return n;
    }else{
      node *l = u->l;
      if((n->l = l)){
        l->p = n;
        update(n);
      }
      u->l = n;
      n->p = u;
      update(u);
      return u;
    }
  }
  // keyを消す, ない場合は何もしない
  static node *erase(node *v, Key key){
    if(!v) return v;
    node *u = find(v, key);
    if(u->key != key) return u;
    node *l = u->l, *r = u->r;
    if(l) l->p = nullptr;
    if(r) r->p = nullptr;
    return merge(l, r);
  }
  static bool exist(node *v, Key key){
    if(!v) return false;
    v = find(v, key);
    return v->key == key;
  }
  // keyの値を得る. ない場合は単位元
  static Val get(node *v, Key key){
    if(!v) return id();
    v = find(v, key);
    return (v->key == key ? v->val : id());
  }
};

template<typename Key>
struct compress_bbst{
  using st = splay_tree<Key>;
  using splay_node = typename splay_tree<Key>::node;
  struct node{
    Key cmp;
    Key lkey, rkey;
    node *l, *r, *p;
    splay_node *n;
    node(Key cmp, splay_node *_n): cmp(cmp), lkey(_n->lkey), rkey(_n->rkey), l(nullptr), r(nullptr), p(nullptr), n(_n){}
  };
  static node *make_node(Key k, Key cmp){
    return new node(cmp, new splay_node(k));
  }
  static node *make_node(splay_node *n, Key cmp){
    return new node(cmp, n);
  }
  static void update(node *v){
    v->lkey = v->n->lkey;
    v->rkey = v->n->rkey;
    if(v->l) v->lkey = v->l->lkey;
    if(v->r) v->rkey = v->r->rkey;
  }
  // pの左の子vをpの位置にする　
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
  // pの右の子vをpの位置にする　
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
  // vを根にする
  static void splay(node *v){
    if(!v) return;
    while(v->p){
      node *p = v->p;
      if(!p->p){
        if(p->l == v) rotate_right(v);
        else rotate_left(v);
      }else{
        node *pp = p->p;
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
  // 左端のノード
  static node *leftmost(node *v){
    while(v->l) v = v->l;
    splay(v);
    return v;
  }
  // 右端のノード
  static node *rightmost(node *v){
    while(v->r) v = v->r;
    splay(v);
    return v;
  }
  // k <= rkeyとなるような最小のノード
  // ない場合は最も右のノード
  static node *find(node *v, Key k){
    if(!v) return v;
    while(v){
      if(v->n->rkey < k){
        if(!v->r){
          splay(v);
          return v;
        }
        v = v->r;
      }else if(!v->l || v->l->rkey < k){
        splay(v);
        return v;
      }else{
        v = v->l;
      }
    }
    assert(false);
    return v;
  }
  static node *erase(node *v, Key k){
    auto [a, b, c] = split3(v, k, k + 1);
    return merge(a, c);
  }
  // kを追加, すでにある場合はcmpを上書き
  static node *insert(node *v, Key k, Key cmp){
    auto [L, R] = split(v, k);
    if(!R){
      return merge(L, make_node(k, cmp));
    }
    if(R->lkey != k){
      assert(!L || L->rkey < k);
      node *u = make_node(k, cmp);
      if((u->l = L)) L->p = u;
      if((u->r = R)) R->p = u;
      update(u);
      return u;
    }
    if(R->n->sz == 1){
      R->cmp = cmp;
      st::update(R->n);
      return merge(L, R);
    }
    R->n = st::leftmost(R->n);
    R->n = R->n->r;
    R->n->p = nullptr;
    node *u = make_node(k, cmp);
    if((u->l = L)) L->p = u;
    if((u->r = R)) R->p = u;
    update(u);
    return u;
  }
  // {k未満, k以上}
  // (splayの)lkey < k <= (splayの)rkeyとなるようなsplay_nodeが存在する場合
  // {lkey, k未満}, {k以上, rkey}となるように内部のsplay木を分割
  static std::pair<node*, node*> split(node *v, Key k){
    if(!v) return {nullptr, nullptr};
    if(v->rkey < k) return {v, nullptr};
    v = find(v, k);
    if(k <= v->n->lkey){
      node *l = v->l;
      v->l = nullptr;
      if(l) l->p = nullptr;
      update(v);
      return {l, v};
    }
    auto [L, R] = st::split(v->n, k);
    assert(L && R);
    node *u = make_node(L, v->cmp);
    node *l = v->l;
    if((u->l = l)){
      l->p = u;
      update(u);
    }
    v->n = R;
    v->l = nullptr;
    update(v);
    return {u, v};
  }
  static std::tuple<node*, node*, node*> split3(node *v, Key l, Key r){
    if(!v) return {nullptr, nullptr, nullptr};
    auto [b, c] = split(v, r);
    auto [a, b2] = split(b, l);
    return {a, b2, c};
  }
  static node *merge(node *a, node *b){
    if(!a || !b) return !a ? b : a;
    assert(a->rkey <= b->lkey);
    b = leftmost(b);
    b->l = a;
    a->p = b;
    update(b);
    return b;
  }
  // split3によって分割された直後の組でないと壊れる
  static node *merge3(node *a, node *b, node *c){
    return merge(merge(a, b), c);
  }
  // vの部分木のnode内部のsplay_nodeを1つのnodeにまとめる
  // O(v部分木のサイズ * logN)
  static node *compress(node *v){
    if(!v) return nullptr;
    splay_node *s = nullptr;
    auto f = [&](auto &&f, node *u) -> void {
      if(u->l) f(f, u->l);
      s = st::merge(s, u->n);
      if(u->r) f(f, u->r);
    };
    f(f, v);
    v->n = s;
    v->l = v->r = nullptr;
    update(v);
    return v;
  }
  static node *compress2(node *v, std::vector<std::tuple<Key, Key, Key>> &res){
    if(!v) return nullptr;
    splay_node *s = nullptr;
    auto f = [&](auto &&f, node *u) -> void {
      if(u->l) f(f, u->l);
      res.push_back({u->n->lkey, u->n->rkey, u->cmp});
      s = st::merge(s, u->n);
      if(u->r) f(f, u->r);
    };
    f(f, v);
    v->n = s;
    v->l = v->r = nullptr;
    update(v);
    return v;
  }
};

template<typename Key, typename monoid>
struct compress_bbst_monoid{
  using Val = typename monoid::Val;
  static constexpr auto id = monoid::id;
  static constexpr auto merge_val = monoid::merge;
  using st = splay_tree_monoid<Key, monoid>;
  using splay_node = typename splay_tree_monoid<Key, monoid>::node;
  struct node{
    Key cmp;
    Key lkey, rkey;
    Val sum;
    node *l, *r, *p;
    splay_node *n;
    node(Key cmp, splay_node *_n): cmp(cmp), lkey(_n->lkey), rkey(_n->rkey), sum(_n->sum), l(nullptr), r(nullptr), p(nullptr), n(_n){}
  };
  static node *make_node(Key k, Val val, Key cmp){
    return new node(cmp, new splay_node(k, val));
  }
  static node *make_node(splay_node *n, Key cmp){
    return new node(cmp, n);
  }
  static void update(node *v){
    v->lkey = v->n->lkey;
    v->rkey = v->n->rkey;
    v->sum = v->n->sum;
    if(v->l){
      v->lkey = v->l->lkey;
      v->sum = merge_val(v->l->sum, v->sum);
    }
    if(v->r){
      v->rkey = v->r->rkey;
      v->sum = merge_val(v->sum, v->r->sum);
    }
  }
  // pの左の子vをpの位置にする　
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
  // pの右の子vをpの位置にする　
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
  // vを根にする
  static void splay(node *v){
    if(!v) return;
    while(v->p){
      node *p = v->p;
      if(!p->p){
        if(p->l == v) rotate_right(v);
        else rotate_left(v);
      }else{
        node *pp = p->p;
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
  // 左端のノード
  static node *leftmost(node *v){
    while(v->l) v = v->l;
    splay(v);
    return v;
  }
  // 右端のノード
  static node *rightmost(node *v){
    while(v->r) v = v->r;
    splay(v);
    return v;
  }
  // k <= rkeyとなるような最小のノード
  // ない場合は最も右のノード
  static node *find(node *v, Key k){
    if(!v) return v;
    while(v){
      if(v->n->rkey < k){
        if(!v->r){
          splay(v);
          return v;
        }
        v = v->r;
      }else if(!v->l || v->l->rkey < k){
        splay(v);
        return v;
      }else{
        v = v->l;
      }
    }
    assert(false);
    return v;
  }
  static node *erase(node *v, Key k){
    auto [a, b, c] = split3(v, k, k + 1);
    return merge(a, c);
  }
  // kを追加, すでにある場合はcmpを上書き
  static node *insert(node *v, Key k, Val val, Key cmp){
    auto [L, R] = split(v, k);
    if(!R){
      return merge(L, make_node(k, val, cmp));
    }
    if(R->lkey != k){
      assert(!L || L->rkey < k);
      node *u = make_node(k, val, cmp);
      if((u->l = L)) L->p = u;
      if((u->r = R)) R->p = u;
      update(u);
      return u;
    }
    if(R->n->sz == 1){
      R->cmp = cmp;
      R->n->val = val;
      st::update(R->n);
      return merge(L, R);
    }
    R->n = st::leftmost(R->n);
    R->n = R->n->r;
    R->n->p = nullptr;
    node *u = make_node(k, val, cmp);
    if((u->l = L)) L->p = u;
    if((u->r = R)) R->p = u;
    update(u);
    return u;
  }
  // {k未満, k以上}
  // (splayの)lkey < k <= (splayの)rkeyとなるようなsplay_nodeが存在する場合
  // {lkey, k未満}, {k以上, rkey}となるように内部のsplay木を分割
  static std::pair<node*, node*> split(node *v, Key k){
    if(!v) return {nullptr, nullptr};
    if(v->rkey < k) return {v, nullptr};
    v = find(v, k);
    if(k <= v->n->lkey){
      node *l = v->l;
      v->l = nullptr;
      if(l) l->p = nullptr;
      update(v);
      return {l, v};
    }
    auto [L, R] = st::split(v->n, k);
    assert(L && R);
    node *u = make_node(L, v->cmp);
    node *l = v->l;
    if((u->l = l)){
      l->p = u;
      update(u);
    }
    v->n = R;
    v->l = nullptr;
    update(v);
    return {u, v};
  }
  static std::tuple<node*, node*, node*> split3(node *v, Key l, Key r){
    if(!v) return {nullptr, nullptr, nullptr};
    auto [b, c] = split(v, r);
    auto [a, b2] = split(b, l);
    return {a, b2, c};
  }
  static node *merge(node *a, node *b){
    if(!a || !b) return !a ? b : a;
    assert(a->rkey <= b->lkey);
    b = leftmost(b);
    b->l = a;
    a->p = b;
    update(b);
    return b;
  }
  // split3によって分割された直後の組でないと壊れる
  static node *merge3(node *a, node *b, node *c){
    return merge(merge(a, b), c);
  }
  // vの部分木のnode内部のsplay_nodeを1つのnodeにまとめる
  // O(v部分木のサイズ * logN)
  static node *compress(node *v){
    if(!v) return nullptr;
    splay_node *s = nullptr;
    auto f = [&](auto &&f, node *u) -> void {
      if(u->l) f(f, u->l);
      s = st::merge(s, u->n);
      if(u->r) f(f, u->r);
    };
    f(f, v);
    v->n = s;
    v->l = v->r = nullptr;
    update(v);
    return v;
  }
  static node *compress2(node *v, std::vector<std::tuple<Key, Key, Key>> &res){
    if(!v) return nullptr;
    splay_node *s = nullptr;
    auto f = [&](auto &&f, node *u) -> void {
      if(u->l) f(f, u->l);
      res.push_back({u->n->lkey, u->n->rkey, u->cmp});
      s = st::merge(s, u->n);
      if(u->r) f(f, u->r);
    };
    f(f, v);
    v->n = s;
    v->l = v->r = nullptr;
    update(v);
    return v;
  }
};
#endif
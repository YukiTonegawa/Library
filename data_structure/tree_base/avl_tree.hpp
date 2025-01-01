#ifndef _AVL_BASE_H_
#define _AVL_BASE_H_
#include <vector>

template<typename T>
struct avl_node {
    T _val;
    char h;
    avl_node *lch, *rch;
    avl_node(T x) : _val(x), h(0) {}
    char balanace_factor() {
        return (lch ? lch->h : 0) - (rch ? rch->h : 0);
    }
};

template<typename Node, void (*update)(Node*), void (*push)(Node*)>
struct avl_tree {
    static void set_lch(Node *p, Node *c) {
        push(p);
        p->lch = c;
        update(p);
    }

    static void set_rch(Node *p, Node *c) {
        push(p);
        p->rch = c;
        update(p);
    }

    static void cut_lch(Node *p) {
        push(p);
        p->lch = nullptr;
        update(p);
    }

    static void cut_rch(Node *p) {
        push(p);
        p->rch = nullptr;
        update(p);
    }

    static Node *rotate_right(Node *v) {
        push(v);
        Node *l = v->l;
        push(l);
        v->l = l->r;
        l->r = v;
        update(v);
        update(l);
        return l;
    }

    static Node *rotate_left(Node *v) {
        push(v);
        Node *r = v->r;
        push(r);
        v->r = r->l;
        r->l = v;
        update(v);
        update(r);
        return r;
    }

    static Node *balance(Node *v) {
        int bf = v->balanace_factor();
        assert(-2 <= bf && bf <= 2);
        push(v);
        if (bf == 2) {
            if (v->l->balanace_factor() == -1) {
                v->l = rotate_left(v->l);
                update(v);
            }
            return rotate_right(v);
        } else if (bf == -2) {
            if (v->r->balanace_factor() == 1) {
                v->r = rotate_right(v->r);
                update(v);
            }
            return rotate_left(v);
        }
        return v;
    }

    static Node *build(const std::vector<Node*> &nodes, int l, int r) {
        int m = (l + r) >> 1;
        Node *v = nodes[m];
        if (l < m) v->l = build(nodes, l, m);
        if (m + 1 < r) v->r = build(nodes, m + 1, r);
        update(v);
        return v;
    }

    template<typename F>
    static void enumerate(Node *a, F f) {
        if (!a) return;
        push(a);
        if (a->lch) enumerate(a->lch, f);
        f(a);
        if (a->rch) enumerate(a->rch, f);
    }

    // [l, r)が
    // ・ある頂点の部分木を完全に含む
    // ・ある頂点と交差する(全頂点のサイズが1の場合は完全に含む)
    // 上記のイベントがO(logN)回起こる
    // 前者ではf(v), 後者ではg(v, _l, _r)を呼ぶ(_l, _rはv中でのインデックス)
    template<int (*get_size)(Node*), int (*get_sumsize)(Node*), typename F, typename G>
    static void intersection(Node *v, int l, int r, F f, G g) {
        if (!v || l >= r || get_sumsize(v) <= l) return;
        if (l == 0 && get_sumsize(v) <= r) {
            f(v);
            update(v);
            return;
        }
        push(v);
        int lsz = v->lch ? get_sumsize(v->lch) : 0;
        intersection<get_size, get_sumsize>(v->lch, l, r, f, g);
        if (r <= lsz) {
            update(v);
            return;
        }
        int vsz = get_size(v); 
        {
            int _l = std::max(l - lsz, 0);
            int _r = std::min(r - lsz, vsz);
            if (_l < _r) g(v, _l, _r);
        }
        l = std::max(0, l - lsz - vsz);
        r = r - lsz - vsz;
        intersection<get_size, get_sumsize>(v->rch, l, r, f, g);
        update(v);
    }
};

/*
  node *leftmost(node *v){
    while(v->l){
      push_down(v);
      v = v->l;
    }
    return v;
  }
  node *rightmost(node *v){
    while(v->r){
      push_down(v);
      v = v->r;
    }
    return v;
  }
  // kの中で最も早く追加された要素
  node *find_key(node *v, Key k){
    node *ret = nullptr;
    while(v){
      push_down(v);
      if(k <= v->key){
        if(k == v->key) ret = v;
        v = v->l;
      }else v = v->r;
    }
    return ret;
  }
  node *find_next_inner(node *v){
    while(v){
      if(v->r) return leftmost(v->r);
      v = v->p;
    }
    return nullptr;
  }
  node *find_prev_inner(node *v){
    while(v){
      if(v->l) return rightmost(v->l);
      v = v->p;
    }
    return nullptr;
  }
  int low_count_unique_inner(node *v, Key k){
    int ret = 0;
    while(v){
      int szl = size_unique(v->l);
      int f = (v->l && v->l->rmost->key != v->key);
      if(k <= v->key) v = v->l;
      else ret += szl + f, v = v->r;
    }
    return ret;
  }
  int low_count_sum_inner(node *v, Key k){
    int ret = 0;
    while(v){
      int szl = size_sum(v->l);
      if(k <= v->key) v = v->l;
      else ret += szl + 1, v = v->r;
    }
    return ret;
  }
  // uの左側にいくつ要素があるか
  int low_count_node_inner(node *v){
    int ret = 0;
    ret += size_sum(v->l);
    while(v){
      if(v->p && v->p->r == v){
        v = v->p;
        ret += size_sum(v->l) + 1;
      }else v = v->p;
    }
    return ret;
  }
  // k種類目のノードの中で最も早く追加されたもの
  node *find_kth_unique(node *v, int k){
    while(true){
      push_down(v);
      int szl = size_unique(v->l);
      if(szl <= k){
        int f = (v->l && v->l->rmost->key != v->key);
        if(szl == k && f) return v;
        k -= szl + f;
        v = v->r;
      }else v = v->l;
    }
  }
  // k番目の要素(複数ある場合は最も早く追加されたもの)
  node *find_kth_sum(node *v, int k){
    while(true){
      push_down(v);
      int szl = size_sum(v->l);
      if(szl <= k){
        if(szl == k) return v;
        k -= szl + 1;
        v = v->r;
      }else v = v->l;
    }
  }
  node *cut_leftmost(node *v){
    push_down(v);
    if(v->l){
      if((v->l = cut_leftmost(v->l))) v->l->p = v;
      update(v);
      return balance(v);
    }
    tmp_node = v;
    return v->r;
  }
  node *cut_rightmost(node *v){
    push_down(v);
    if(v->r){
      if((v->r = cut_rightmost(v->r))) v->r->p = v;
      update(v);
      return balance(v);
    }
    tmp_node = v;
    return v->l;
  }
  // k以上のキーを持つ最小要素(複数ある場合は最も早く追加したもの)
  node *lower_bound_inner(node *v, Key k){
    node *ret = nullptr;
    while(v){
      push_down(v);
      if(k <= v->key){
        ret = v;
        v = v->l;
      }else v = v->r;
    }
    return ret;
  }
  // k以下のキーを持つ最大要素(複数ある場合は最も遅く追加したもの)
  node *lower_bound_rev_inner(node *v, Key k){
    node *ret = nullptr;
    while(v){
      push_down(v);
      if(k >= v->key){
        ret = v;
        v = v->r;
      }else v = v->l;
    }
    return ret;
  }
  // kの要素がすでにある場合それらの中で最も右側に追加
  node *emplace_inner(node *v, Key k, Val val){
    if(!v) return new node(k, val);
    push_down(v);
    if(k < v->key  && ((v->l = emplace_inner(v->l, k, val)))) v->l->p = v;
    if(k >= v->key && ((v->r = emplace_inner(v->r, k, val)))) v->r->p = v;
    update(v);
    return balance(v);
  }
  // kの要素がすでにある場合何もしない
  node *emplace_unique_inner(node *v, Key k, Val val){
    if(!v) return new node(k, val);
    push_down(v);
    if(k < v->key  && ((v->l = emplace_unique_inner(v->l, k, val)))) v->l->p = v;
    if(k > v->key && ((v->r = emplace_unique_inner(v->r, k, val)))) v->r->p = v;
    update(v);
    return balance(v);
  }
  // kの要素がすでにある場合そのうちのいずれかと置き換える
  node *emplace_replace_inner(node *v, Key k, Val val){
    if(!v) return new node(k, val);
    push_down(v);
    if(k < v->key  && ((v->l = emplace_replace_inner(v->l, k, val)))) v->l->p = v;
    if(k > v->key && ((v->r = emplace_replace_inner(v->r, k, val)))) v->r->p = v;
    if(k == v->key) v->val = val;
    update(v);
    return balance(v);
  }
  // kを消す(複数ある場合は最も早く追加した要素を消す)
  node *erase_inner(node *v, Key k){
    if(!v) return nullptr;
    push_down(v);
    if(k < v->key && ((v->l = erase_inner(v->l, k)))) v->l->p = v;
    if(k > v->key && ((v->r = erase_inner(v->r, k)))) v->r->p = v;
    if(k == v->key){
      if(v->l && v->l->rmost->key == k){
        if((v->l = erase_inner(v->l, k))) v->l->p = v;
        update(v);
        return balance(v);
      }
      if(v->r){
        v->r = cut_leftmost(v->r);
        if((tmp_node->l = v->l)) tmp_node->l->p = tmp_node;
        if((tmp_node->r = v->r)) tmp_node->r->p = tmp_node;
        update(tmp_node);
        return balance(tmp_node);
      }
      return v->l;
    }
    update(v);
    return balance(v);
  }
  // vを消す
  node *erase_node_inner(node *v){
    assert(v);
    node *p = v->p, *u = v;
    if(v->r){
      v->r = cut_leftmost(v->r);
      if((tmp_node->l = v->l)) tmp_node->l->p = tmp_node;
      if((tmp_node->r = v->r)) tmp_node->r->p = tmp_node;
      update(tmp_node);
      v = balance(tmp_node);
    }else{
      v = v->l;
    }
    while(p){
      if(p->l == u && ((p->l = v))) v->p = p;
      if(p->r == u && ((p->r = v))) v->p = p;
      update(p);
      u = p;
      p = u->p;
      v = balance(u);
    }
    return v;
  }
  void update_inner(node *v, Key l, Key r, Lazy x){
    if(!v || v->rmost->key < l || r <= v->lmost->key) return;
    if(l <= v->lmost->key && v->rmost->key < r){
      propagate(v, x);
      return;
    }
    push_down(v);
    update_inner(v->l, l, r, x);
    update_inner(v->r, l, r, x);
    if(l <= v->key && v->key < r) v->val = apply(v->val, x, 0, 1);
    update(v);
  }
  Val query_inner(node *v, Key l, Key r){
    if(!v || v->rmost->key < l || r <= v->lmost->key) return id();
    if(l <= v->lmost->key && v->rmost->key < r) return v->sum;
    push_down(v);
    if(r <= v->key) return query_inner(v->l, l, r);
    if(v->key < l) return query_inner(v->r, l, r);
    return merge(query_inner(v->l, l, r), merge(v->val, query_inner(v->r, l, r)));
  }
  template<typename F>
  node *bisect_from_left(node *v, F &f, Key l, Val &ok){
    if(!v || v->rmost->key < l) return nullptr;
    push_down(v);
    Val m = merge(ok, v->sum);
    if(l <= v->lmost->key && !f(m)){
      ok = m;
      return nullptr;
    }
    node *x = bisect_from_left(v->l, f, l, ok);
    if(x) return x;
    if(l <= v->key){
      ok = merge(ok, v->val);
      if(f(ok)) return v;
    }
    return bisect_from_left(v->r, f, l, ok);
  }
  template<typename F>
  node *bisect_from_right(node *v, F &f, Key r, Val &ok){
    if(!v || v->lmost->key > r) return nullptr;
    push_down(v);
    Val m = merge(v->sum, ok);
    if(r >= v->rmost->key && !f(m)){
      ok = m;
      return nullptr;
    }
    node *x = bisect_from_right(v->r, f, r, ok);
    if(x) return x;
    if(v->key <= r){
      ok = merge(v->val, ok);
      if(f(ok)) return v;
    }
    return bisect_from_right(v->l, f, r, ok);
  }
  void to_list_inner(node *v, std::vector<std::pair<Key, Val>> &res){
    push_down(v);
    if(v->l) to_list_inner(v->l, res);
    res.push_back({v->key, v->val});
    if(v->r) to_list_inner(v->r, res);
  }
  lazy_multimap_monoid(): root(nullptr){}
  lazy_multimap_monoid(std::vector<std::pair<Key, Val>> v){
    std::sort(v.begin(), v.end());
    init_sorted(v);
  }
*/
#endif
#ifndef _DYNAMIC_HUFFMAN_TREE_H_
#define _DYNAMIC_HUFFMAN_TREE_H_
#include <vector>
#include <algorithm>
#include <string>
#include <queue>
// xの値が1増える
// 値がxと同じyであって (xのペア) <= (yのペア) であるものが存在しないならそのまま足していい
// そうでない場合 xとペアの値が最大のyと入れ替える -> それに足す
// 親に移動

// xの値が1減る
// 値がxと同じyであって (xのペア) >= (yのペア)　であるものが存在しないならそのまま引いていい
// そうでない場合 xとペアの値が最小のyと入れ替える -> それを引く
// 親に移動

template<typename Val>
struct dynamic_huffman_tree {
  private:
    struct node {
        node *p, *l, *r; // {親, 左, 右}
        Val val;
        node(Val _val) : p(nullptr), l(nullptr), r(nullptr), val(_val) {}
        node(node *_l, node *_r) : p(nullptr), l(_l), r(_r), val(_l->val + _r->val) {
            _l->p = this;
            _r->p = this;
        }
        bool operator < (const node* b) { return val < b->val; }
    };

    node *_root, *_min_leaf;
    std::unordered_map<Val, node*> greater_node;
    std::unordered_map<Val, node*> less_node;
    /*
    // i, jをswap
    void swap_elem(int i, int j) {
        int pi = u[i].p, pj = u[j].p;
        if (pi == pj) {
            modify(pi);
            return;
        }

        Val x, y;
        if (u[pi].l == i) {
            u[pi].l = j;
            x = u[pi].r;
        } else {
            u[pi].r = j;
            x = u[pi].l;
        }
        if (u[pj].l == j) {
            u[pj].l = i;
            y = u[pj].r;
        } else {
            u[pj].r = i;
            y = u[pj].l;
        }
        std::swap(u[i].p, u[j].p);
        modify(pi);
        modify(pj);

        // {i, x}, {j, y}を消して{i, y}, {j, x}を追加
    }
    */
  public:
    dynamic_huffman_tree() : _root(nullptr) {}

    std::vector<node*> build(const std::vector<Val> &V) {
        if (V.empty()) return;
        int N = V.size();
        std::vector<node*> res(N);
        std::priority_queue<node*, std::vector<node*>, std::greater<node*>> pq;
        for (int i = 0; i < N; i++) {
            assert(V[i] >= 0);
            res[i] = new node(V[i]);
            pq.push(res[i]);
        }
        while (pq.size() >= 2) {
            node *l = pq.top();
            pq.pop();
            node *r = pq.pop();
            pq.pop();
            node *p = new node(l, r);
            pq.push(p);
            if (l->val != r->val) {
                greater_node[l->val] = l;
                less_node[r->val] = r;
            }
        }
        _root = pq.top();
    }

    node *root() {
        return _root;
    }

    // 1を1個追加してそのノード(葉)を返す
    node *push_one() { 
        node *v = new node(1);
        if (!_root) {
            _root = _min_leaf = v;
            return v;
        }
        node *m = _min_leaf;
        node *p = m->p;
        node *u = new node(v, m);
        _min_leaf = v;
        if (p) {
            if (p->l == m) {
                p->l = u;
            } else {
                p->r = u;
            }
            u->p = p;
        } else {
            _root = u;
        }
    }
    
    // 値が1の葉vを消す
    void pop_one(node *v) {
        assert(v && v->val == 1 && !v->l && !v->r); // 値が1の葉
        if (_root == v) {
            _root = _min_leaf = nullptr;
            return;
        }
        node *p = v->p, *pp = p->p;
        node *u = (p->l == v ? p->r : p->l);
        if (_min_leaf == v) {
            //
        }
        if (!pp) {
            _root = u;
            return;
        }
        if (pp->l == p) {
            pp->l = u;
        } else {
            pp->r = u;
        }
        u->p = pp;
        while (u->p) {
            u = u->p;
            u->val = u->l->val + u->r->val;
        }
    }

    /*

    // v[i]++
    void add(int i) {
        Val x = v[i]++;
        swd++;
        if (is_root(i)) return;

        auto itr = max_pair.find(x);
        int k = *itr;
        if(v[k] == x){

        }
    
        //__swap_elem(i, j);
        add(plr[i].p);
    }

    // v[i]--
    void del(int i){
        Val x = v[i]--;
        assert(v[i]); // 0になるとハフマン符号が壊れる
        swd--;
        if (is_root(i)) return;
        //auto []
        int j = -1;
        swap_elem(i, j);
        del(plr[i].p);
    }
    */

    // vのハフマン符号, O(log(S))
    std::string encode(node *v) {
        std::string res = "";
        while (v->p) {
            node *p = v->p;
            res += (v == p->l ? '0' : '1');
            v = p;
        }
        std::reverse(res);
        if (res.empty()) res += '0';
        return res;
    }
};

#endif
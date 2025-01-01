#ifndef _ONLINE_DYCONNE_H_
#define _ONLINE_DYCONNE_H_
#include <vector>
#include <cassert>
#include <unordered_set>
#include <unordered_map>

namespace _online_dynamic_connectivity {
    struct euler_tour_tree {

        struct node {
            int s, t, sz, flag;
            node *l, *r, *p, *target;
            node(int _s, int _t) : s(_s), t(_t), sz(1), flag(s == t ? 0 : 2), l(nullptr), r(nullptr), p(nullptr), target(this) {}
        };

        static void update(node *v) {
            v->sz = 1;
            v->target = v;
            if (v->l) {
                v->sz += v->l->sz;
                if (v->target->flag < v->l->target->flag) v->target = v->l->target;
            }
            if (v->r) {
                v->sz += v->r->sz;
                if (v->target->flag < v->r->target->flag) v->target = v->r->target;
            }
        }

        // Pの子POSにCHをset
        #define set_child(P, POS, CH) if ((POS = CH)) CH->p = P
        // CHがあることがわかっている
        #define set_child2(P, POS, CH) POS = CH, CH->p = P
        #define set_parent(V, P, PREV){\
            if ((V->p = P)) {\
                if (P->l == PREV) P->l = V;\
                else P->r = V;\
            }\
        }

        static void zig_right(node *v) {
            node *p = v->p;
            set_child(p, p->l, v->r);
            v->r = p;
            v->p = nullptr;
            p->p = v;
            update(p);
            update(v);
        }

        static void zig_left(node *v) {
            node *p = v->p;
            set_child(p, p->r, v->l);
            v->l = p;
            v->p = nullptr;
            p->p = v;
            update(p);
            update(v);
        }

        static void zig_zig_right(node *v) {
            node *p = v->p;
            node *pp = p->p;
            node *ppp = pp->p;
            set_child(pp, pp->l, p->r);
            set_child(p, p->l, v->r);
            set_child2(p, p->r, pp);
            set_child2(v, v->r, p);
            update(pp);
            update(p);
            update(v);
            set_parent(v, ppp, pp);
        }

        static void zig_zig_left(node *v) {
            node *p = v->p;
            node *pp = p->p;
            node *ppp = pp->p;
            set_child(pp, pp->r, p->l);
            set_child(p, p->r, v->l);
            set_child2(p, p->l, pp);
            set_child2(v, v->l, p);
            update(pp);
            update(p);
            update(v);
            set_parent(v, ppp, pp);
        }

        static void zig_zag_right(node *v) {
            node *p = v->p;
            node *pp = p->p;
            node *ppp = pp->p;
            set_child(p, p->r, v->l);
            set_child(pp, pp->l, v->r);
            set_child2(v, v->l, p);
            set_child2(v, v->r, pp);
            update(pp);
            update(p);
            update(v);
            set_parent(v, ppp, pp);
        }

        static void zig_zag_left(node *v) {
            node *p = v->p;
            node *pp = p->p;
            node *ppp = pp->p;
            set_child(p, p->l, v->r);
            set_child(pp, pp->r, v->l);
            set_child2(v, v->r, p);
            set_child2(v, v->l, pp);
            update(pp);
            update(p);
            update(v);
            set_parent(v, ppp, pp);
        }

        static void splay(node *v) {
            update(v);
            while (v->p) {
                node *p = v->p;
                if (!p->p) {
                    if (p->l == v) zig_right(v);
                    else zig_left(v);
                } else {
                    node *pp = p->p;
                    if (pp->l == p) {
                        if (p->l == v) zig_zig_right(v);
                        else zig_zag_right(v);
                    } else {
                        if (p->r == v) zig_zig_left(v);
                        else zig_zag_left(v);
                    }
                }
            }
        }

        static node *merge(node *a, node *b) {
            if (!a) return b;
            if (!b) return a;
            if (a->sz < b->sz) {
                while (a->r) a = a->r;
                splay(a);
                set_child2(a, a->r, b);
                update(a);
                return a;
            } else {
                while (b->l) b = b->l;
                splay(b);
                set_child2(b, b->l, a);
                update(b);
                return b;
            }
        }

        // {vより左, v以降} -> {v以降, vより左}
        static node *toggle(node *v) {
            splay(v);
            node *l = v->l;
            if (l) v->l = l->p = nullptr;
            update(v);
            return merge(v, l);
        }

        static std::vector<node*> edge_stock;
        
        static node *make_edge(int s, int t) {
            if (edge_stock.empty()) {
                return new node(s, t);
            } else {
                node *res = edge_stock.back();
                edge_stock.pop_back();
                *res = node(s, t);
                return res;
            }
        }

        static node *make_vertex(int s) {
            return new node(s, s);
        }

        // 辺eを消して2つに分離
        static std::pair<node*, node*> erase_split(node *e) {
            splay(e);
            std::pair<node*, node*> res = {e->l, e->r};
            if (e->l) e->l->p = nullptr;
            if (e->r) e->r->p = nullptr;
            update(e);
            edge_stock.push_back(e);
            return res;
        }

        // e1{s, t} と e2{t, s} を消してcut
        // {sの残る木, tの残る木}
        static std::pair<node*, node*> cut(node *e1, node *e2) {
            auto [X, Y] = erase_split(e1);
            int szy = (Y ? Y->sz : 0);
            auto [Z, V] = erase_split(e2);
            bool e1left = (Y == e2 || (Y ? Y->sz : 0) != szy);
            if (e1left) {
                return {merge(X, V), Z};
            } else {
                return {V, merge(Z, Y)};
            }
        }

        // 頂点aと頂点bをマージ
        static std::tuple<node*, node*, node*> link(node *a, node *b, bool is_level_k = true) {
            int s = a->s;
            int t = b->s;
            splay(a);
            b = toggle(b);
            node *e1 = make_edge(s, t);
            node *e2 = make_edge(t, s);
            if (!is_level_k) e1->flag = 0;
            e2->flag = 0;
            set_child2(e1, e1->r, b);
            set_child2(e2, e2->l, e1);
            set_child(e2, e2->r, a->r);
            set_child2(a, a->r, e2);
            update(e1);
            update(e2);
            update(a);
            return {a, e1, e2};
        }

        static node *lmost(node *a) {
            splay(a);
            while (a->l) a = a->l;
            splay(a);
            return a;
        }

        static bool same(node *a, node *b) {
            if (!a || !b) return false;
            if (a == b) return true;
            
            splay(a);
            bool ok = false;
            while (b->p) {
                node *p = b->p;
                if (!p->p) {
                    ok = p == a;
                    if (p->l == b) zig_right(b);
                    else zig_left(b);
                } else {
                    node *pp = p->p;
                    if (!pp->p) ok = pp == a;
                    if (pp->l == p) {
                        if (p->l == b) zig_zig_right(b);
                        else zig_zag_right(b);
                    } else {
                        if (p->r == b) zig_zig_left(b);
                        else zig_zag_left(b);
                    }
                }
            } 

            return ok;
        }

        #undef set_child
        #undef set_child2
        #undef set_parent
    };

    std::vector<_online_dynamic_connectivity::euler_tour_tree::node*> _online_dynamic_connectivity::euler_tour_tree::edge_stock;

    // 時間計算量
    // link/cut ならし O(log^2 N)
    // 検索系 O(log N)
    // 空間計算量
    // O(|存在する辺| + NlogN)
    struct online_dynamic_connectivity {
      private:
        using ett = _online_dynamic_connectivity::euler_tour_tree;
        using node = ett::node;
        int N, LV, cc;
        std::vector<std::vector<node*>> V;
        std::vector<std::vector<std::unordered_multiset<int>>> E;
        std::vector<std::unordered_map<long long, node*>> F;

        long long encode(int a, int b) {
            return ((long long)a << 32) + b;
        }

        void lvup() {
            LV++;
            V.push_back(std::vector<node*>(N, nullptr));
            E.push_back(std::vector<std::unordered_multiset<int>>(N));
            F.push_back({});
        }

        void update_flag(node *v, int f) {
            v->flag = f;
            ett::splay(v);
        }

        bool replace(int a, int b, int k) {
            ett::splay(V[k][a]);
            ett::splay(V[k][b]);

            if (V[k][a]->sz > V[k][b]->sz) std::swap(a, b);
            
            node *e = V[k][a]->target;

            while (e->flag == 2) {
                ett::splay(e);
                int c = e->s;
                int d = e->t;
                if (LV == k + 1) lvup();
                if (!V[k + 1][c]) V[k + 1][c] = ett::make_vertex(c);
                if (!V[k + 1][d]) V[k + 1][d] = ett::make_vertex(d);
                auto [v, e1, e2] = ett::link(V[k + 1][c], V[k + 1][d]);
                F[k + 1].emplace(encode(c, d), e1);
                F[k + 1].emplace(encode(d, c), e2);
                e->flag = 0;
                ett::update(e);
                e = e->target;
            }

            while (e->flag == 1) {
                ett::splay(e);
                int c = e->s;
                bool ok = false;
                while (!ok && !E[k][c].empty()) {
                    auto itr = E[k][c].begin();
                    int d = *itr;
                    E[k][c].erase(itr);
                    E[k][d].erase(E[k][d].find(c));
                    if (!ett::same(V[k][c], V[k][d])) {
                        for (int i = k; i >= 0; i--) {
                            auto [v, e1, e2] = ett::link(V[i][c], V[i][d], i == k);
                            F[i].emplace(encode(c, d), e1);
                            F[i].emplace(encode(d, c), e2);
                        }
                        ok = true;
                    } else {
                        if (LV == k + 1) lvup();
                        E[k + 1][c].insert(d);
                        E[k + 1][d].insert(c);
                        if (V[k + 1][c]->flag == 0) update_flag(V[k + 1][c], 1);
                        if (V[k + 1][d]->flag == 0) update_flag(V[k + 1][d], 1);
                    }
                }

                if (E[k][c].empty()) update_flag(e, 0);
                if (ok) return true;
                ett::splay(e);
                e = e->target;
            }
            return k ? replace(a, b, k - 1) : false;
        }
        
      public:
        online_dynamic_connectivity(int _N) : N(_N), LV(1), cc(N), V(1), E(1), F(1) {
            for (int i = 0; i < N; i++) {
                V[0].push_back(ett::make_vertex(i));
                E[0].push_back({});
            }
        }

        // aとbが連結か
        bool same(int a, int b) {
            return ett::same(V[0][a], V[0][b]);
        }

        // 連結成分の代表番号を返す
        // 仕様: 1回もlink, cutしない間は常に同じ値を返すが、そうでない場合は同じ集合でも異なる値を返す場合がある
        int find(int a) {
            return ett::lmost(V[0][a])->s;
        }

        // aを含む連結成分の要素数
        int size(int a) {
            ett::splay(V[0][a]);
            return (V[0][a]->sz + 2) / 3;
        }

        // 連結成分の個数
        int count_cc() {
            return cc;
        }

        // 辺(a, b)を追加する 辺(a, b)があっても追加する
        // 0: すでに連結だった, 1: 新たに連結になった
        bool link(int a, int b) {
            assert(a != b);
            if (same(a, b)) {
                E[0][a].insert(b);
                E[0][b].insert(a);
                if (V[0][a]->flag == 0) update_flag(V[0][a], 1);
                if (V[0][b]->flag == 0) update_flag(V[0][b], 1);
                return false;
            } else {
                auto [v, e1, e2] = ett::link(V[0][a], V[0][b]);
                F[0].emplace(encode(a, b), e1);
                F[0].emplace(encode(b, a), e2);
                cc--;
                return true;
            }
        }

        // 辺(a, b)を切る 0: 辺が無かった, 1: 辺が橋だった, 2: まだ連結
        int cut(int a, int b) {            
            if (a == b) return 0;
            if (F[0].find(encode(a, b)) == F[0].end()) {
                for (int i = 0; i < LV; i++) {
                    auto itr = E[i][a].find(b);
                    if (itr != E[i][a].end()) {
                        E[i][a].erase(itr);
                        E[i][b].erase(E[i][b].find(a));
                        if (E[i][a].empty()) update_flag(V[i][a], 0);
                        if (E[i][b].empty()) update_flag(V[i][b], 0);
                        return 2;
                    }
                }
                return 0;
            }

            int lv = 0;
            for (; lv < LV; lv++) {
                auto itr = F[lv].find(encode(a, b));
                if (itr == F[lv].end()) break;
                node *e1 = itr->second;
                F[lv].erase(itr);
                auto itr2 = F[lv].find(encode(b, a));
                node *e2 = itr2->second;
                ett::cut(e1, e2);
                F[lv].erase(itr2);
            }
            lv--;
            bool f = replace(a, b, lv);
            if (!f) cc++;
            return f ? 2 : 1;
        }
    };
}

using online_dynamic_connectivity = _online_dynamic_connectivity::online_dynamic_connectivity;

#endif
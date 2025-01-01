#ifndef _PERMUTATION_TREE_H_
#define _PERMUTATION_TREE_H_
#include "../segtree/segtree_lazy_fast.hpp"
#include "../ordered_set/nazoki.hpp"
#include <vector>
#include <algorithm>
#include <cassert>

struct permutation_tree {
    using S = int;
    using F = int;
    static constexpr S op(S a, S b) { return std::min(a, b); }
    static constexpr S e() { return 1 << 30; }
    static constexpr S mapping(F f, S a, int len) { return a + f; }
    static constexpr F composition(F a, F b) { return a + b; }
    static constexpr F id() { return 0; }
    using SG = segtree_lazy_fast<S, op, e, F, mapping, composition, id>;

    struct node {
        int l, r; // インデックスの範囲[l, r]
        int min, max; // 値の範囲 [min, max]
        bool linear, inc, dec;
        std::vector<node*> ch;
        node(int _l, int _r, int _min, int _max) : l(_l), r(_r), min(_min), max(_max), linear(true), inc(true), dec(true) {}
    };

    node *root = nullptr;

    // [0, N)の順列
    void build(const std::vector<int> &P) {
        int N = P.size();
        std::vector<int> pos(N), mx, mn;
        nazotree<4> nzk;
        SG seg(std::vector<int>(N, 0));
        for (int i = 0; i < N; i++) {
            pos[P[i]] = i;
            nzk.insert(i);
        }
        std::vector<node*> V(N);
        for (int i = 0; i < N; i++) V[i] = new node(i, i, P[i], P[i]);
        
        for (int p : P) {
            while (!mx.empty() && mx.back() < p) {
                int x = mx.back();
                int r = pos[x] + 1;
                mx.pop_back();
                int l = (mx.empty() ? 0 : pos[mx.back()] + 1);
                seg.apply(l, r, p - x);
            }
            mx.push_back(p);
            while (!mn.empty() && mn.back() > p) {
                int x = mn.back();
                int r = pos[x] + 1;
                mn.pop_back();
                int l = (mn.empty() ? 0 : pos[mn.back()] + 1);
                seg.apply(l, r, x - p);
            }
            mn.push_back(p);
            seg.apply(0, pos[p], -1);

            int r = pos[p];
            while (r) {
                int l = seg.min_left(r, [&](int x) { return x > 0; }) - 1;
                if (l == -1) break;
                std::vector<int> lpos;
                int k = l - 1;
                while (true) {
                    k = nzk.gt(k);
                    if (k == -1 || k > r) break;
                    lpos.push_back(k);
                }
                if (lpos.size() >= 3) {
                    node *v = new node(l, pos[p], V[l]->min, V[r]->max);
                    v->linear = false;
                    v->ch.push_back(V[l]);
                    V[l] = v;
                    for (int i = 1; i < lpos.size(); i++) {
                        int j = lpos[i];
                        V[l]->ch.push_back(V[j]);
                        nzk.erase(j);
                        V[l]->min = std::min(V[l]->min, V[j]->min);
                        V[l]->max = std::max(V[l]->max, V[j]->max);
                        V[j] = nullptr;
                    }
                } else{
                    nzk.erase(r);
                    bool f = (V[l]->inc && V[l]->max + 1 == V[r]->min);
                    bool g = (V[l]->dec && V[r]->max + 1 == V[l]->min);
                    int min_val = std::min(V[l]->min, V[r]->min);
                    int max_val = std::max(V[l]->max, V[r]->max);
                    if (V[l]->linear && (V[l]->min != V[l]->max) && (f || g)) {
                        V[l]->inc = f;
                        V[l]->dec = g;
                        V[l]->r = pos[p];
                        V[l]->min = min_val;
                        V[l]->max = max_val;
                        V[l]->ch.push_back(V[r]);
                        V[r] = nullptr;
                    } else {
                        node *v = new node(l, pos[p], min_val, max_val);
                        v->inc = V[l]->max + 1 == V[r]->min;
                        v->dec = V[r]->max + 1 == V[l]->min;
                        v->ch.push_back(V[l]);
                        v->ch.push_back(V[r]);
                        V[l] = v;
                        V[r] = nullptr;
                    }
                }
                seg.apply(l + 1, r + 1, 1);
                r = l;
            }
        }
        root = V[0];
    }
};
#endif
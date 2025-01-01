#ifndef _SIMPLE_BINARY_TRIE_H_
#define _SIMPLE_BINARY_TRIE_H_
#include <array>
template<typename Key, int log, typename S>
struct simple_binary_trie {
    struct node {
        node *par;
        std::array<node*, 2> ch;
        S val;
        node() : par(nullptr) { ch.fill(nullptr); }
        ~node() {}
    };

    static void update(node *v) {}

    static node *insert(node *v, int dep, Key key, S val) {
        if (dep == log) {
            if (v) return v;
            v = new node();
            v->val = val;
            return v;
        }
        if (!v) v = new node();
        int dir = (key >> (log - dep - 1)) & 1;
        v->ch[dir] = insert(v->ch[dir], dep + 1, key, val);
        if (v->ch[dir]) v->ch[dir]->par = v;
        update(v);
        return v;
    }

    static node *find(node *v, int dep, Key key) {
        if (dep == log) return v;
        int dir = (key >> (log - dep - 1)) & 1;
        return find(v->ch[dir], dep + 1, key);
    }

    static node *erase(node *v, int dep, Key key) {
        if (dep == log) return nullptr;
        int dir = (key >> (log - dep - 1)) & 1;
        v->ch[dir] = erase(v->ch[dir], dep + 1, key);
        if (v->ch[dir]) v->ch[dir]->par = v;
        if (!v->ch[0] && !v->ch[1]) return nullptr;
        update(v);
        return v;
    }
    
    static std::pair<node*, node*> cut(node *v, int dep, Key l, Key r, Key L, Key R) {
        if (!v) return {nullptr, nullptr};
        if (l <= L && R <= r) return {nullptr, v};
        if (R <= l || r <= L) return {v, nullptr};
        Key M = (L + R) / 2;
        auto [a, b] = cut(v->ch[0], dep + 1, l, r, L, M);
        auto [c, d] = cut(v->ch[1], dep + 1, l, r, M, R);
        if (!a && !d) {
            v->ch[0] = b;
            v->ch[1] = c;
            if (v->ch[0]) v->ch[0]->par = v;
            if (v->ch[1]) v->ch[1]->par = v;
            update(v);
            return {nullptr, v};
        }
        v->ch[0] = a;
        v->ch[1] = d;
        if (v->ch[0]) v->ch[0]->par = v;
        if (v->ch[1]) v->ch[1]->par = v;
        node *u = new node();
        u->ch[0] = b;
        u->ch[1] = c;
        if (u->ch[0]) u->ch[0]->par = u;
        if (u->ch[1]) u->ch[1]->par = u;
        update(v);
        update(u);
        return {v, u};
    }

    static node *meld(node *a, node *b) {
        if (!a) return b;
        if (!b) return a;
        a->ch[0] = meld(a->ch[0], b->ch[0]);
        a->ch[1] = melf(a->ch[1], b->ch[1]);
        if (a->ch[0]) a->ch[0]->par = a;
        if (a->ch[1]) a->ch[1]->par = a;
        delete(b);
        update(a);
        return a;
    }
};
#endif
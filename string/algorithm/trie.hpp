#ifndef _TRIE_H_
#define _TRIE_H_
#include <vector>
#include <array>
#include <string>

template<typename Val, Val (*id)()>
struct trie {
    struct node{
        Val val;
        node *p;
        std::array<node*, 26> c;
        node(Val _val = id()): val(_val), p(nullptr){c.fill(nullptr);}
    };

    node *root;
    trie(): root(new node()){}

    node *get_root(){return root;}
    
    // sを追加して終端ノードを返す
    node *insert(const std::string &s) {
        node *v = root, *u;
        for (int i = 0; i < s.size(); i++){
            if (v->c[s[i] - 'a']) {
                v = v->c[s[i] - 'a'];
            } else {
                u = (v->c[s[i] - 'a'] = new node());
                u->p = v;
                v = u;
            }
        }
        return v;
    }

    // sを検索, ない場合はnullptr
    node *find(const std::string &s) {
        node *v = root;
        for (int i = 0; i < s.size(); i++) {
            if (v->c[s[i] - 'a']) v = v->c[s[i] - 'a'];
            else return nullptr;
        }
        return v;
    }

    // 親がある場合書き換えてtrueを返す
    bool parent(node* &v) {
        return v = v->p;
    }

    // 子がある場合書き換えてtrueを返す
    bool next(node* &v, char c){
        return v = v->c[c - 'a'];
    }
};

#endif
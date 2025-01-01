#ifndef _AHO_CORASICK_H_
#define _AHO_CORASICK_H_
#include <vector>
#include <array>
#include <cassert>
#include <string>
#include <queue>

template<typename Val, Val (*id)()>
struct aho_corasick {
    struct node{
        bool is_end; // いずれかのパターンの終端
        Val val;
        node *p, *s;
        std::array<node*, 26> c;
        node(Val _val = id()): is_end(false), val(_val), p(nullptr), s(nullptr) { c.fill(nullptr); }
    };
    
    node *root;
    
    // 文字cに対応するノードvの最長サフィックスリンクを貼る
    void find_longest_suffix(node *v, char c) {
        // vの最長サフィックスリンクは [1]根 / [2]c1文字 / [3]pの最長サフィックスリンクを何回か辿ったノードの子　のいずれか
        // [2]のパターンは[3]に含まれる.
        // 最長のものが欲しいので, pの最長サフィックスリンクを何回か辿っていく過程でcに対応する子ノードを持つ最初のノードが答え
        // 存在しなければ根が答え

        // 文字列Sを追加する過程で最長サフィックスリンクを辿る回数はO(|S|)
        // i + 1文字目を追加するとき, 可能な接頭辞の長さは高々1増える.
        // 新たなノードの最長サフィックスリンクを決める過程で親のリンクを1 + k回辿ると, 可能な接頭辞の長さはk以上減る
        // よってリンクを辿る回数は2|S|回

        assert(v);
        node *p = v->p;
        
        // vまたはpが根なら根
        if (!p || p == root) {
            v->s = root;
            return;
        }

        int k = c - 'a';
        while (p != root) {
            p = p->s;
            if (p->c[k]) {
                v->s = p->c[k];
                return;
            }
        }
        // 見つからなかった(根にする)
        v->s = root;
    }
    
    aho_corasick(): root(new node()) {}
    
    node *get_root() { return root; }
    
    // sを追加して終端ノードを返す
    // 動的に使うとサフィックスリンクが壊れる
    node *insert(const std::string &s) {
        node *v = root, *u;
        for (int i = 0; i < s.size(); i++) {
            if (v->c[s[i] - 'a']) {
                v = v->c[s[i] - 'a'];
            } else {
                u = (v->c[s[i] - 'a'] = new node());
                u->p = v;
                v = u;
            }
        }
        v->is_end = true;
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
    bool next(node* &v, char c) {
        return v = v->c[c - 'a'];
    }

    void build_suffix() {
        std::queue<node*> q;
        q.push(root);
        root->s = root;
        while (!q.empty()) {
            node *v = q.front();
            q.pop();
            for (int i = 0; i < v->c.size(); i++) {
                if (v->c[i]) {
                    find_longest_suffix(v->c[i], i + 'a');
                    q.push(v->c[i]);
                }
            }
        }
    }

    // 現在のノード + cの最長サフィックスに書き換える
    // ない場合は根にしてfalse
    bool next_suffix(node* &v, char c) {
        while (v != root && v->c[c - 'a'] == nullptr) v = v->s;
        if (v->c[c - 'a']) return v = v->c[c - 'a'];
        return false;
    }

    // 文字列SがパターンTを連続部分文字列として含む場合, あるiが存在して S_0, S_1...S_iの接尾辞の集合はTを含む
    // 現在のノードに対応する接尾辞の集合が含むパターンを列挙する
    // 実装上, 接尾辞に相当するノードを全て返して終端ノードかの判定は自分でやる
    std::vector<node*> find_suffix(node *v) {
        std::vector<node*> res;
        while (v != root){
            res.push_back(v);
            v = v->s;
        }
        return res;
    }
  
    // 現在の接尾辞がいずれかのパターンとマッチしたか
    bool match_any(node *v) {
        while (v != root) {
            if (v->is_end) return true;
            v = v->s;
        }
        return false;
    }
};

#endif
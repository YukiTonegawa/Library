#ifndef _PALINDROME_TREE_H_
#define _PALINDROME_TREE_H_
#include <vector>
#include <map>
#include <cassert>
#include <numeric>
#include <algorithm>

template<typename T>
struct palindrome_tree {
    struct node {
        int id, len;
        node *parent;
        node *suffix;
        std::map<T, node*> ch;
        node(int id) : id(id), parent(nullptr), suffix(nullptr) {}
    };
    node *ODD, *current;
    std::vector<node*> A;
    std::vector<node*> nodes;
    int cnt = 0;
    static constexpr T inf = std::numeric_limits<T>::max();
    
    palindrome_tree(const std::vector<T> &_S) {
        std::vector<T> S;
        for (T c : _S) {
            S.push_back(inf);
            S.push_back(c);
        }
        ODD = new node(-1);
        ODD->len = -1;
        current = ODD;
        int N = S.size();
        for (int i = 0; i < N; i++) {
            while (true) {
                int j = i - 1 - current->len;
                if (j >= 0 && S[j] == S[i]) break;
                current = current->suffix;
            }
            auto itr_add = current->ch.find(S[i]);
            if (itr_add != current->ch.end()) {
                if (i & 1) A.push_back(itr_add->second);
                current = itr_add->second;
                continue;
            }
            node *par = current;
            current = new node((i == 0 || i % 2 == 1) ? cnt++ : -1);
            current->parent = par;
            current->len = par->len + 2;
            par->ch.emplace(S[i], current);
            if (i & 1) A.push_back(current);
            if (i == 0 || i % 2) nodes.push_back(current);
            if (current->len == 1) {
                current->suffix = ODD;
                continue;
            }
            while (true) {
                par = par->suffix;
                int j = i - 1 - par->len;
                if (j >= 0 && S[j] == S[i]) {
                    current->suffix = par->ch[S[i]];
                    break;
                }
            }
        }
    }

    // 長さ1文字以上の回文の種類数
    int num_node() const {
        return cnt - 1;
    }

    // ノード番号vの回文の両端を削除した時のノード番号 (2文字の親は0, 1文字の親は-1とする)
    int par(int v) const {
        int L = nodes[v]->len;
        node *p = nodes[v]->parent;
        if (p->parent) p = p->parent;
        if (p->id == -1) return (L % 4 == 3 ? 0 : -1);
        return p->id;
    }
    // ノード番号vの回文長
    int len(int v) const {
        return (nodes[v]->len + 1) / 2;
    }

    // ノード番号vの回文の接尾辞回文のうち|v|未満の最大のノード番号 (0文字の場合は0)
    int max_suffix(int v) const {
        return (nodes[v]->suffix ? std::max(0, nodes[v]->suffix->id) : 0);
    }

    // S0...Siの最長の回文に対応するノード番号
    int ord(int i) const {
        return A[i]->id;
    }
};
#endif
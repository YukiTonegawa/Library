#ifndef _TECC_INCREMENTAL_H_
#define _TECC_INCREMENTAL_H_
#include <vector>
#include <unordered_set>
#include "../../data_structure/union_find/union_find.hpp"

struct tecc_incremental {
  private:
    int b;
    union_find cc, tec;
    std::vector<int> p;   

    int parent(int u) {
        return p[u] = tec.find(p[u]);
    }

    void evert(int u) {
        std::vector<int> V;
        while (true) {
            V.push_back(u);
            if (tec.same(u, p[u])) break;
            u = parent(u); 
        }
        while (!V.empty()) {
            u = V.back();
            V.pop_back();
            p[u] = (V.empty() ? u : V.back());
        }
    }

    void unite_cc(int u, int v) {
        if (cc.size(u) > cc.size(v)) std::swap(u, v);
        evert(u);
        p[u] = v;
        cc.unite(u, v);
        b++;
    }

    int lca(int u, int v) {
        std::unordered_set<int> V;
        while (true) {
            if (u != -1) {
                if (V.find(u) != V.end()) return u;
                V.insert(u);
                int par = parent(u);
                u = (tec.same(u, par) ? -1 : par);
            }
            std::swap(u, v);
        }
    }

    // u-vパスをlに圧縮する, 減った橋の数を返す
    int unite_tecc(int u, int v) {
        int erased_bridge = 0;
        int l = lca(u, v);
        while (!tec.same(u, l)) {
            int par = parent(u);
            tec.unite(u, l);
            u = par;
            erased_bridge++;
        }
        while (!tec.same(v, l)) {
            int par = parent(v);
            tec.unite(v, l);
            v = par;
            erased_bridge++;
        }
        p[tec.find(l)] = p[l];
        b -= erased_bridge;
        return erased_bridge;
    }
  public:
    tecc_incremental(int n = 0): b(0), cc(n), tec(n), p(n) {
        std::iota(p.begin(), p.end(), 0);
    }

    // 0: 非連結->連結になった, 1: 連結->2辺連結になった, 2: 元から2辺連結だった
    // 多重辺でも2辺連結だと見なす
    int add_edge(int u, int v) {
        u = tec.find(u), v = tec.find(v);
        if (u == v) return 2;
        if (!cc.same(u, v)) {
            unite_cc(u, v);
            return 0;
        }
        unite_tecc(u, v);
        return 1;
    }

    // uを含む連結成分のサイズ　
    int size_cc(int u) {
        return cc.size(u);
    }

    // uを含む2辺連結成分のサイズ　
    int size_tecc(int u) {
        return tec.size(u);
    }

    // 連結成分の番号
    int find_cc(int u) {
        return cc.find(u);
    }

    // 2辺連結成分の番号
    int find_tecc(int u) {
        return tec.find(u);
    }
    
    // uとvが  0: 非連結, 1: 連結であるが2辺連結でない, 2: 2辺連結 
    int same(int u, int v){
        if (!cc.same(u, v)) return 0;
        if (!tec.same(u, v)) return 1;
        return 2;
    }

    // 橋の数
    int num_bridge(){
        return b;
    }
};
#endif
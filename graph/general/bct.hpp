#ifndef _BCT_H_
#define _BCT_H_
#include "bcc.hpp"
#include <cassert>

// bccの成分数がN, cut vertexがMの時N+M頂点の木を作る
// 頂点番号 [0, num_block)がブロック
// [num_block, num_block, num_cut)がcut vertex
struct bct {
    int _num_block, _num_cut;
    std::vector<int> cmp;
    
    bct() {}
    bct(const bcc &G) : _num_block(G.group.size()), _num_cut(0), cmp(G._n) {
        int N = G._n;
        assert(G.cut.size() == N);
        for (int i = 0; i < G.group.size(); i++) {
            for (int v : G.group[i]) {
                if (!G.cut[v])  cmp[v] = i;
            }
        }
        for (int i = 0; i < N; i++) {
            if (G.cut[i]) cmp[i] = _num_block + _num_cut++;
        }
    }
};
#endif
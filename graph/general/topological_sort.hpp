#ifndef _TOPOSO_H_
#define _TOPOSO_H_
#include "../base/csr.hpp"
#include <vector>
#include <queue>

// 終了時にinが0でない要素がある <-> 閉路が存在する
// 閉路があるなら空のvectorを返す
template<typename edge = int>
std::vector<int> topological_sort(const csr<edge> &G) {
    int N = G.N();
    std::queue<int> q;
    std::vector<int> in(N, 0), res;
    for (int i = 0; i < G.elist.size(); i++) {
        int to = G.elist[i];
        in[to]++;
    }
    for (int i = 0; i < N; i++) if(!in[i]) q.push(i);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        res.push_back(v);
        for (int i = G.begin(v); i < G.end(v); i++) {
            int to = G.elist[i];
            if ((--in[to]) == 0) q.push(to);
        }
    }
    for(int i = 0; i < N; i++) if(in[i] != 0) return {};
    return res;
}
#endif
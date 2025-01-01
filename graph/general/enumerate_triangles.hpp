#ifndef _ENUMERATE_TRIANGLES_H_
#define _ENUMERATE_TRIANGLES_H_
#include <vector>

// 無向グラフの三角形を列挙
// 自己辺はあっても無視, 多重辺があると時間計算量が壊れる(enumerateの場合)
template<typename edge, typename F>
void enumerate_triangles(const std::vector<std::vector<edge>> &g, F f) {
    int N = g.size();
    std::vector<std::vector<edge>> g2(N);
    for (int i = 0; i < N; i++) {
        int deg_i = g[i].size();
        for (const edge &j : g[i]) {
            int to = j; 
            int deg_j = g[to].size();
            if(deg_i > deg_j || (deg_i == deg_j && i > to)) g2[i].push_back(to);
        }
    }
    std::vector<bool> use(N, false);
    for (int i = 0; i < N; i++) {
        int a = i;
        for (const edge &j : g2[i]) {
            int to = j;
            use[to] = true;
        }
        for (const edge &j : g2[i]) {
            int b = j;
            for (edge &k : g2[b]) {
                int c = k;
                if (use[c]) {
                    f(a, b, c);
                }
            }
        }
        for (const edge &j : g2[i]) {
            int to = j;
            use[to] = false;
        }
    }
}
#endif
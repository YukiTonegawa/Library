#ifndef _GENERAL_MATCHING_H_
#define _GENERAL_MATCHING_H_
#include <vector>
#include <cassert>
#include <queue>
#include <algorithm>

struct genera_matching {
    int n, m;
    std::vector<int> mate;
    std::vector<std::vector<int>> b;
    std::vector<int> p, d, bl;
    std::vector<std::vector<int>> g;

    genera_matching(int n) : n(n), m(n + n / 2), mate(n, -1), b(m), p(m), d(m), bl(m), g(m, std::vector<int>(m, -1)) {}

    void add_edge(int u, int v) {
        g[u][v] = u;
        g[v][u] = v;
    }

    void match(int u, int v) {
        g[u][v] = g[v][u] = -1;
        mate[u] = v;
        mate[v] = u;
    }

    std::vector<int> trace(int x) {
        std::vector<int> vx;
        while (true) {
            while (bl[x] != x) x = bl[x];
            if (!vx.empty() && vx.back() == x) break;
            vx.push_back(x);
            x = p[x];
        }
        return vx;
    }

    void contract(int c, std::vector<int> &vx, std::vector<int> &vy) {
        b[c].clear();
        int r = vx.back();
        while (!vx.empty() && !vy.empty() && vx.back() == vy.back()) {
            r = vx.back();
            vx.pop_back();
            vy.pop_back();
        }
        b[c].push_back(r);
        b[c].insert(b[c].end(), vx.rbegin(), vx.rend());
        b[c].insert(b[c].end(), vy.begin(), vy.end());
        for (int i = 0; i <= c; i++) {
            g[c][i] = g[i][c] = -1;
        }
        for (int z : b[c]) {
            bl[z] = c;
            for(int i = 0; i < c; i++) {
                if (g[z][i] != -1) {
                    g[c][i] = z;
                    g[i][c] = g[i][z];
                }
            }
        }
    }

    std::vector<int> lift(std::vector<int> &vx) {
        std::vector<int> A;
        while (vx.size() >= 2) {
            int z = vx.back(); vx.pop_back();
            if(z < n) {
                A.push_back(z);
                continue;
            }
            int w = vx.back();
            int i = (A.size() % 2 == 0 ? find(b[z].begin(), b[z].end(), g[z][w]) - b[z].begin() : 0);
            int j = (A.size() % 2 == 1 ? find(b[z].begin(), b[z].end(), g[z][A.back()]) - b[z].begin() : 0);
            int k = b[z].size();
            int dif = (A.size() % 2 == 0 ? i % 2 == 1 : j % 2 == 0) ? 1 : k - 1;
            while (i != j) {
                vx.push_back(b[z][i]);
                i = (i + dif) % k;
            }
            vx.push_back(b[z][i]);
        }
        return A;
    }
    
    int solve() {
        for (int ans = 0; ; ans++) {
            std::fill(d.begin(), d.end(), 0);
            std::queue<int> Q;
            for (int i = 0; i < m; i++) bl[i] = i;
            for (int i = 0; i < n; i++) {
                if (mate[i] == -1) {
                    Q.push(i);
                    p[i] = i;
                    d[i] = 1;
                }
            }
            int c = n;
            bool aug = false;
            while (!Q.empty() && !aug) {
                int x = Q.front();
                Q.pop();
                if (bl[x] != x) continue;

                for (int y = 0; y < c; y++) {
                    if (bl[y] == y && g[x][y] != -1) {
                        if (d[y] == 0) {
                            p[y] = x;
                            d[y] = 2;
                            p[mate[y]] = y;
                            d[mate[y]] = 1;
                            Q.push(mate[y]);
                        } else if(d[y] == 1) {
                            std::vector<int> vx = trace(x);
                            std::vector<int> vy = trace(y);

                            if (vx.back() == vy.back()) {
                                contract(c, vx, vy);
                                Q.push(c);
                                p[c] = p[b[c][0]];
                                d[c] = 1;
                                c++;
                            } else {
                                aug = true;
                                vx.insert(vx.begin(), y);
                                vy.insert(vy.begin(), x);
                                std::vector<int> A = lift(vx);
                                std::vector<int> B = lift(vy);
                                A.insert(A.end(), B.rbegin(), B.rend());
                                for (int i = 0; i < (int) A.size(); i += 2) {
                                    match(A[i], A[i + 1]);
                                    if (i + 2 < (int) A.size()) add_edge(A[i + 1], A[i + 2]);
                                }
                            }
                            break;
                        }
                    }
                }
            }
            if (!aug) return ans;
        }
    }
    
    // マッチ先(存在しない場合-1)
    int operator [] (int i) const { 
        assert(0 <= i && i < n);
        return mate[i];
    }
};

#endif
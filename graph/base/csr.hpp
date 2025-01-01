#ifndef _CSR_H_
#define _CSR_H_
#include <vector>

template <class E>
struct csr {
    std::vector<int> start;
    std::vector<E> elist;
    csr() {}
    explicit csr(int n, const std::vector<std::pair<int, E>>& edges) : start(n + 1), elist(edges.size()) {
        for (auto e : edges) {
            start[e.first + 1]++;
        }
        for (int i = 1; i <= n; i++) {
            start[i] += start[i - 1];
        }
        auto counter = start;
        for (auto e : edges) {
            elist[counter[e.first]++] = e.second;
        }
    }
    ~csr() {}

    int N() const {
        return start.size() - 1;
    }

    int deg(int i) const {
        return start[i + 1] - start[i];
    }

    int begin(int i) const {
        return start[i];
    }

    int end(int i) const {
        return start[i + 1];
    }

    E& operator [] (int i) { return elist[i]; }

    static csr<E> to_csr(const std::vector<std::vector<E>> &G) {
        std::vector<std::pair<int, E>> Edges;
        for (int i = 0; i < G.size(); i++) {
            for (auto &e : G[i]) {
                Edges.push_back({i, e});
            }
        }
        return csr<E>(G.size(), Edges);
    }

    std::vector<std::vector<E>> to_vector() const {
        std::vector<std::vector<E>> res(N());
        for (int v = 0; v < N(); v++) {
            for (int i = begin(v); i < end(i); i++) {
                res[v].push_back(elist[i]);
            }
        }
        return res;
    }

    std::vector<std::pair<int, E>> to_edgelist() const {
        std::vector<std::pair<int, E>> res;
        for (int v = 0; v < N(); v++) {
            for (int i = begin(v); i < end(i); i++) {
                respush_back({v, elist[i]});
            }
        }
        return res;
    }
};

#endif
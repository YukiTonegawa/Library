#ifndef _TWO_SAT_H_
#define _TWO_SAT_H_
#include <utility>
#include <vector>
#include "../../graph/base/csr.hpp"
#include "../../graph/general/scc.hpp"

struct two_sat {
  public:
    two_sat() : _n(0), s(0) {}
    two_sat(int n) : _n(n), _answer(n), s(2 * n) {}
    
    // iがf
    void add_clause_one(int i, bool f) {
        assert(0 <= i && i < _n);
        s.add_edge(2 * i + (f ? 0 : 1), 2 * i + (f ? 1 : 0));
    }

    // (iがf) v (jがg)
    void add_clause_or(int i, bool f, int j, bool g) {
        assert(0 <= i && i < _n);
        assert(0 <= j && j < _n);
        s.add_edge(2 * i + (f ? 0 : 1), 2 * j + (g ? 1 : 0));
        s.add_edge(2 * j + (g ? 0 : 1), 2 * i + (f ? 1 : 0));
    }

    // (iがf), (jがg)のちょうど一方が成り立つ
    void add_clause_xor(int i, bool f, int j, bool g) {
        add_clause_or(i, f, j, g);
        add_clause_or(i, !f, j, !g);
    }

    bool satisfiable() {
        auto id = s.scc_ids().second;
        for (int i = 0; i < _n; i++) {
            if (id[2 * i] == id[2 * i + 1]) return false;
            _answer[i] = id[2 * i] < id[2 * i + 1];
        }
        return true;
    }
    
    std::vector<bool> answer() { return _answer; }
private:
    int _n;
    std::vector<bool> _answer;
    scc s;
};
#endif
#ifndef _EDGE_H_
#define _EDGE_H_
#include <cassert>

template<typename _W>
struct edge_base {
    using W = _W;
    int t;
    edge_base() {}
    edge_base(int _t) : t(_t) {}

    virtual int from() const {
        assert(false);
    }

    int to() const {
        return t;
    }
    
    virtual W weight() const {
        assert(false);
    }

    virtual int id() const {
        assert(false);
    }

    operator int() const {
        return t;
    }
};

struct simple_edge : edge_base<int> {
    simple_edge() {}
    simple_edge(int _t) : edge_base<int>(_t) {}
    int weight() const override {
        return 1;
    }
    int id() const override {
        return -1;
    }
};

struct labeled_edge : edge_base<int> {
    int _s, _id;
    labeled_edge() {}
    labeled_edge(int _s, int _t, int _id) : edge_base<int>(_t), _s(_s), _id(_id) {}
    int weight() const override {
        return 1;
    }
    int from() const override {
        return _s;
    }
    int id() const override {
        return _id;
    }
};

template<typename _W>
struct weighted_edge : edge_base<_W> {
    using W = _W;
    W w;
    weighted_edge() {}
    weighted_edge(int _t, _W _w) : edge_base<_W>(_t), w(_w) {}
    W weight() const override {
        return w;
    }
    int id() const override {
        return -1;
    }
};

template<typename _W>
struct weighted_labeled_edge : edge_base<_W> {
    using W = _W;
    W w;
    int _s, _id;
    weighted_labeled_edge() : _id(-1) {}
    weighted_labeled_edge(int _s, int _t, _W _w, int _id) : edge_base<_W>(_t), w(_w), _s(_s), _id(_id) {}
    int from() const override {
        return _s;
    }
    W weight() const override {
        return w;
    }
    int id() const override {
        return _id;
    }
};

#endif
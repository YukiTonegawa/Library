#ifndef _SWAG_H_
#define _SWAG_H_
#include <vector>
#include <deque>

template<typename S, S (*op)(S, S), S (*e)()>
struct swag {
    std::deque<S> q;
    std::vector<S> sum_left, sum_right;
  
    swag() {
        sum_left.push_back(e());
        sum_right.push_back(e());
    }

    int size() {
        return q.size();
    }

    bool empty() {
        return q.size() == 0;
    }

    // 空の場合は単位元
    S query_all() {
        return op(sum_left.back(), sum_right.back());
    }

    void push_back(S x) {
        q.push_back(x);
        sum_right.push_back(merge(sum_right.back(), x));
    }

    void pop_back() {
        assert(size());
        if (sum_right.size() > 1) {
            sum_right.pop_back();
            q.pop_back();
            return;
        }
        int new_r_size = (q.size() + 1) / 2;
        int new_l_size = q.size() - new_r_size;
        q.pop_back();
        for (int i = new_l_size; i < q.size(); i++) {
            sum_right.push_back(op(sum_right.back(), q[i]));
        }
        sum_left.resize(new_l_size + 1);
        for(int i = 0; i < new_l_size; i++) {
            sum_left[i + 1] = op(q[new_l_size - 1 - i], sum_left[i]);
        }
    }

    void push_front(S x) {
        q.push_front(x);
        sum_left.push_back(op(x, sum_left.back()));
    }

    void pop_front() {
        assert(size());
        if (sum_left.size() > 1) {
            sum_left.pop_back();
            q.pop_front();
            return;
        }
        int new_l_size = (q.size() + 1) / 2;
        int new_r_size = q.size() - new_l_size;
        for (int i = 0; i < new_l_size - 1; i++) {
            sum_left.push_back(op(q[new_l_size - 1 - i], sum_left[i]));
        }
        sum_right.resize(new_r_size + 1);
        for (int i = 0; i < new_r_size; i++) {
            sum_right[i + 1] = op(sum_right[i], q[new_l_size + i]);
        }
        q.pop_front();
    }

    S back() {
        return q.back();
    }

    S front() {
        return q.front();
    }
};

template<typename monoid>
using swag_monoid = swag<typename monoid::S, monoid::op, monoid::e>;
#endif
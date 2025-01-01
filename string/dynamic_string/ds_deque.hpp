#ifndef _DS_DEQUE_H_
#define _DS_DEQUE_H_
#include <deque>
#include <vector>
#include "../base/rolling_hash.hpp"

template<typename T, size_t MAX_LEN = 2000000>
struct ds_deque {
    using rh = rolling_hash_base<MAX_LEN>;
    using ull = uint64_t;
    using self_t = ds_deque<T, MAX_LEN>;

    std::deque<ull> q;
    std::vector<ull> sum_left, sum_right;

    ds_deque() {
        sum_left.push_back(0);
        sum_right.push_back(0);
    }

    int size() const {
        return q.size();
    }

    bool empty() const {
        return q.size() == 0;
    }

    ull hash_range(int l, int r) const {
        if (l >= r) return 0;
        int m = (int)sum_left.size() - 1; // [0, m)が左にあり, [m, sz)が右にある
        if (r <= m) {
            return rh::sub(sum_left[m - l], rh::insert_null(r - l, sum_left[m - r]));
        } else if(m <= l) {
            int lsz = l - m;
            return rh::split(lsz, sum_right[l - m], sum_right[r - m]);
        } else {
            int lsz = m - l;
            return rh::concat(lsz, sum_left[m - l], sum_right[r - m]);
        }
    }

    // 空の場合は単位元
    ull hash_all() const { 
        int lsz = (int)sum_left.size() - 1;
        return rh::concat(lsz, sum_left.back(), sum_right.back());
    }

    void push_back(T x) {
        ull h = rh::to_hash(x);
        q.push_back(h);
        int rsz = (int)sum_right.size() - 1;
        sum_right.push_back(rh::concat(rsz, sum_right.back(), h));
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
            int rsz = (int)sum_right.size() - 1;
            sum_right.push_back(rh::concat(rsz, sum_right.back(), q[i]));
        }
        sum_left.resize(new_l_size + 1);
        for (int i = 0; i < new_l_size; i++) {
            sum_left[i + 1] = rh::concat(1, q[new_l_size - 1 - i], sum_left[i]);
        }
    }

    void push_front(T x) {
        ull h = rh::to_hash(x);
        q.push_front(h);
        sum_left.push_back(rh::concat(1, h, sum_left.back()));
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
            sum_left.push_back(rh::concat(1, q[new_l_size - 1 - i], sum_left[i]));
        }
        sum_right.resize(new_r_size + 1);
        for (int i = 0; i < new_r_size; i++) {
            sum_right[i + 1] = rh::concat(i, sum_right[i], q[new_l_size + i]);
        }
        q.pop_front();
    }
    
    ull get(int i) const {
        return q[i];
    }
    
    ull back() const {
        return q.back();
    }

    ull front() const {
        return q.front();
    }

    // lcp(this[l, ...), S2[L, ...))
    int lcp(int l, const self_t &S2, int L) const {
        int maxlen = std::min(size() - l, S2.size() - L);
        assert(maxlen >= 0);
        int a = 0, b = maxlen + 1;
        while (b - a > 1) {
            int c = (a + b) / 2;
            if (hash_range(l, l + c) == S2.hash_range(L, L + c)) {
                a = c;
            } else {
                b = c;
            }
        }
        return a;
    }

    // compare(this[l, ...), S2[L, ...))
    // -1 : this < S2
    // 0 : this == S2
    // 1 : this > S2
    int compare(int l, const self_t &S2, int L) const {
        int _lcp = lcp(l, S2, L);
        if (_lcp != size() - l && _lcp != S2.size() - L) {
            return get(l + _lcp) < S2.get(L + _lcp) ? -1 : 1;
        } else {
            if (_lcp != size() - l) {
                return 1;
            } else if (_lcp != S2.size() - L) {
                return -1;
            } else {
                return 0;   
            }
        }
    }
};
#endif
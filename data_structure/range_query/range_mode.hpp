#ifndef _RANGE_MODE_H_
#define _RANGE_MODE_H_
#include <vector>
#include <algorithm>
#include <cassert>
#include "../../base/unordered_compressor.hpp"

template<typename T>
struct range_mode {
    int n, sq;
    std::vector<T> cmp;
    std::vector<int> val, rank;
    std::vector<std::vector<int>> pos;
    std::vector<std::vector<std::pair<int, int>>> large;
    
    range_mode(const std::vector<T> &v) : n(v.size()), sq(sqrt(n)), val(n), rank(n), pos(n) {
        large.resize(n / sq + 3, std::vector<std::pair<int, int>>(n / sq + 3));
        for (int i = 0; i < n; i++) cmp.push_back(v[i]);
        std::sort(cmp.begin(), cmp.end());
        cmp.erase(std::unique(cmp.begin(), cmp.end()), cmp.end());
        for (int i = 0; i < n; i++) {
            int id = std::lower_bound(cmp.begin(), cmp.end(), v[i]) - cmp.begin();
            val[i] = id;
            rank[i] = pos[id].size();
            pos[id].push_back(i);
        }
        for (int i = 0; i * sq < n; i++) {
            std::vector<int> cnt(n, 0);
            int l = i * sq, r = std::min(n, l + sq);
            std::pair<int, int> m{0, 0};
            for (int j = l, k = i + 1; j < n; j++) {
                cnt[val[j]]++;
                m = std::max(m, {cnt[val[j]], val[j]});
                if (j + 1 == r) {
                    large[i][k++] = m;
                    r = std::min(n, r + sq);
                }
            }
        }
    }

    // {最頻値(のうち値が最大), その頻度}
    std::pair<T, int> mode(int l, int r) {
        int L = (l + sq - 1) / sq, R = r / sq;
        std::pair<int, int> m;

        auto left = [&](int _l, int _r) -> void {
            for (int i = _l; i < _r; i++) {
                int x = val[i], k = rank[i];
                if (k + m.first >= pos[x].size() || pos[x][k + m.first] >= r) continue;
                int j = k + m.first;
                while (j < pos[x].size() && pos[x][j] < r) j++;
                m = std::max(m, {j - k, x});
            }
        };

        if (L >= R) {
            left(l, r);
        } else {
            m = large[L][R];
            left(l, L * sq);
            for (int i = r - 1; i >= R * sq; i--) {
                int x = val[i], k = rank[i];
                if (k - m.first < 0 || pos[x][k - m.first] < l) continue;
                int j = k - m.first;
                while (j >= 0 && pos[x][j] >= l) j--;
                m = std::max(m, {k - j, x});
            }
        }
        return {cmp[m.second], m.first};
    }
};
#endif
#ifndef _ORDERED_MULTISET_SMALL_H_
#define _ORDERED_MULTISET_SMALL_H_
#include <cstdint>
#include <array>

template<typename T>
struct ordered_multiset_small {
    using Size = uint8_t;
    static constexpr int max_size = 16;
    Size size;
    std::array<T, max_size> val;
    ordered_multiset_small() : size(0) {}

    T get(int k) {
        assert(0 <= k && k < size);
        return val[k];
    }
    
    // ソート順を保ってxを追加
    // 重複する場合何もせずfalseを返す
    bool insert(T x) {
        int i = 0;
        while (i < size && val[i] < x) {
            i++;
        }
        for (int j = size; j > i; j--) {
            val[j] = val[j - 1];
        }
        val[i] = x;
        size++;
        return true;
    }

    // 消せた場合trueを返す
    bool erase(T x) {
        int i = 0;
        while (i < size && val[i] < x) {
            i++;
        }
        if (i == size || val[i] != x) return false;
        for (int j = i; j + 1 < size; j++) {
            val[j] = val[j + 1];
        }
        size--;
        return true;
    }

    ordered_multiset_small split_half() {
        assert(size == max_size);
        int r = max_size / 2;
        ordered_multiset_small res;
        for (int i = r; i < max_size; i++) {
            res.val[i - r] = val[i];
        }
        res.size = max_size - r;
        size = r;
        return res;
    }

    // x未満の数
    int lb(T x) {
        for (int i = 0; i < size; i++) {
            if (val[i] >= x) {
                return i;
            }
        }
        return size;
    }

    // x以下の数
    int ub(T x) {
        for (int i = 0; i < size; i++) {
            if (val[i] > x) {
                return i;
            }
        }
        return size;
    }

    T min() {
        return val[0];
    }

    T max() {
        return val[size - 1];
    }
};
#endif
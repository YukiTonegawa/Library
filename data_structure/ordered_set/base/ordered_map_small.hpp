#ifndef _ORDERED_MAP_SMALL_H_
#define _ORDERED_MAP_SMALL_H_
#include <cstdint>
#include <array>

template<typename T, typename U>
struct ordered_map_small {
    using Size = uint8_t;
    static constexpr int max_size = 16;
    Size size;
    std::array<T, max_size> val;
    std::array<U, max_size> second;

    ordered_map_small() : size(0) {}

    std::pair<T, U> get(int k) {
        assert(0 <= k && k < size);
        return {val[k], second[k]};
    }
    
    // ソート順を保ってxを追加
    // 重複する場合何もせずfalseを返す
    bool emplace(T x, U y) {
        int i = 0;
        while (i < size && val[i] < x) {
            i++;
        }
        if (i < size && val[i] == x) return false;
        for (int j = size; j > i; j--) {
            val[j] = val[j - 1];
            second[j] = second[j - 1];
        }
        val[i] = x;
        second[i] = y;
        size++;
        return true;
    }

    // ソート順を保ってxを追加
    // すでにある場合もyに置き換える
    // すでにあった場合はfalse
    bool emplace_replace(T x, U y) {
        int i = 0;
        while (i < size && val[i] < x) {
            i++;
        }
        if (i < size && val[i] == x) {
            second[i] = y;
            return false;
        }
        for (int j = size; j > i; j--) {
            val[j] = val[j - 1];
            second[j] = second[j - 1];
        }
        val[i] = x;
        second[i] = y;
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
            second[j] = second[j + 1];
        }
        size--;
        return true;
    }

    std::pair<bool, U> find(T x) {
        for (int i = 0; i < size && val[i] <= x; i++) {
            if (val[i] == x) {
                return {true, second[i]};
            }
        }
        return {false, U()};
    }

    template<typename F>
    std::pair<bool, U> find(T x, F f) {
        for (int i = 0; i < size && val[i] <= x; i++) {
            if (val[i] == x) {
                f(second[i]);
                return {true, second[i]};
            }
        }
        return {false, U()};
    }

    ordered_map_small split_half() {
        assert(size == max_size);
        int r = max_size / 2;
        ordered_map_small res;
        for (int i = r; i < max_size; i++) {
            res.val[i - r] = val[i];
            res.second[i - r] = second[i];
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
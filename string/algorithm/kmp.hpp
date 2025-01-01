#ifndef _KMP_H_
#define _KMP_H_
#include <vector>

// パターンの検索テーブルを作る O(|pat|)
// Sの中のS_i, S_i+1...がパターンと一致するiを列挙 O(|S|)
template<typename T>
struct kmp {
  private:
    std::vector<T> pat;
    std::vector<int> t;
  
  public:
    kmp(const std::vector<T> &pat) {
        build(pat);
    }

    void build(const std::vector<T> &_pat) {
        pat = _pat;
        int n = pat.size();
        assert(n);
        t.resize(n + 1);
        t[0] = -1;
        int j = -1;
        for (int i = 0; i < n;) {
            while (j >= 0 && pat[i] != pat[j]) j = t[j];
            i++, j++;
            if (i < n && pat[i] == pat[j]) {
                t[i] = t[j];
            } else {
                t[i] = j;
            }
        }
    }

    // 一致する場所を返す
    std::vector<int> find(const std::vector<T> &s) {
        int n = s.size();
        std::vector<int> res;
        for (int i = 0, j = 0; i < n;) {
            while (j >= 0 && s[i] != pat[j]) j = t[j];
            i++, j++;
            if (j == (int)pat.size()) {
                res.push_back(i - j);
                j = t[j];
            }
        }
        return res;
    }
};
#endif
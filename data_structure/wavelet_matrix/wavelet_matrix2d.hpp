#ifndef _WAVELET_MATRIX2D_H_
#define _WAVELET_MATRIX2D_H_
#include "../../math/base/bit_ceil.hpp"
#include "../../base/compressor.hpp"
#include "../bit/simple_bitvector.hpp"
#include "k_ary_wavelet_matrix.hpp"

template<typename Idx, typename Val>
struct wavelet_matrix2d {
  private:
    int N, log;
    compressor<Idx, false> X;
    compressor<Idx, true> Y;
    compressor<Val, true> Z;
    std::vector<simple_bitvector> B;
    std::vector<k_ary_wavelet_matrix<8>> W;
 
    int _range_freq(const int _lx, const int _rx, const int _ly, const int _ry, const int _lz, const int _rz) {
        auto dfs = [&](auto &&dfs, int b, int l, int r, int S, int T) -> int {
            if (l == r || T <= _lz || _rz <= S) return 0;
            if (_lz <= S && T <= _rz) return W[b + 1].range_freq(l, r, _ly, _ry);
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            int l2 = l - lo, r2 = l2 + zero;
            int L2 = N - (B[b].rank(N) - lo), R2 = L2 + one;
            return dfs(dfs, b - 1, l2, r2, S, (S + T) / 2) + dfs(dfs, b - 1, L2, R2, (S + T) / 2, T);
        };
        if (_lx >= _rx || _ly >= _ry || _lz >= _rz) return 0;
        return dfs(dfs, log - 1, _lx, _rx, 0, 1 << log);
    }

    int _kth_smallest(int lx, int rx, int ly, int ry, int k) {
        if (lx >= rx || ly >= ry) return -1;
        if (W[log].range_freq(lx, rx, ly, ry) <= k) return -1;
        int res = 0;
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[b].rank(lx);
            int one = B[b].rank(rx) - lo;
            int zero = (rx - lx) - one;
            int l = lx - lo;
            int r = l + zero;
            int left_cnt = W[b].range_freq(l, r, ly, ry);
            if (left_cnt > k){
                lx = l;
                rx = r;
            }else{
                k -= left_cnt;
                res += 1 << b;
                lx = N - (B[b].rank(N) - lo);
                rx = lx + one;
            }
        }
        return res;
    }

    int _kth_largest(int lx, int rx, int ly, int ry, int k) {
        if (lx >= rx || ly >= ry) return -1;
        if (W[log].range_freq(lx, rx, ly, ry) <= k) return -1;
        int res = 0;
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[b].rank(lx);
            int one = B[b].rank(rx) - lo;
            int zero = (rx - lx) - one;
            int l = N - (B[b].rank(N) - lo);
            int r = l + one;
            int right_cnt = W[b].range_freq(l, r, ly, ry);
            if (right_cnt <= k){
                k -= right_cnt;
                lx = lx - lo;
                rx = lx + zero;
            }else{
                res += 1 << b;
                lx = l;
                rx = r;
            }
        }
        return res;
    }

  public:

    wavelet_matrix2d(std::vector<std::tuple<Idx, Idx, Val>> _v): N(_v.size()){
        if (N == 0) return;
        log = bit_ceil_log(N);

        std::sort(_v.begin(), _v.end(), [](auto a, auto b) {
            return std::get<0>(a) < std::get<0>(b);
        });
        for(int i = 0; i < N; i++){
            auto [x, y, z] = _v[i];
            X.add(x);
            Y.add(y);
            Z.add(z);
        }

        std::vector<int> P(N), Q(N);
        for(int i = 0; i < N; i++){
            auto [x, y, z] = _v[i];
            P[i] = Z.ord(z);
            Q[i] = Y.ord(y);
        }

        W.resize(log + 1);
        B.resize(log);

        W[log] = k_ary_wavelet_matrix<8>(Q);

        for (int b = log - 1; b >= 0; b--) {
            std::vector<bool> bits(N);
            std::vector<int> leftP, rightP, leftQ, rightQ;
            for (int i = 0; i < N; i++) {
                bits[i] = (P[i] >> b) & 1;
                if (!bits[i]) {
                    leftP.push_back(P[i]);
                    leftQ.push_back(Q[i]);
                } else {
                    rightP.push_back(P[i]);
                    rightQ.push_back(Q[i]);
                }
            }
            leftP.insert(leftP.end(), rightP.begin(), rightP.end());
            leftQ.insert(leftQ.end(), rightQ.begin(), rightQ.end());
            std::swap(leftP, P);
            std::swap(leftQ, Q);
            B[b] = simple_bitvector(bits);
            W[b] = k_ary_wavelet_matrix<8>(Q);
        }
    }
    
    // [lx, rx) × [ly, ry)の[lz, rz)の要素数
    int range_freq(Idx lx, Idx rx, Idx ly, Idx ry, Val lz, Val rz){
        return _range_freq(X.ord(lx), X.ord(rx), Y.ord(ly), Y.ord(ry), Z.ord(lz), Z.ord(rz));
    }

    // [lx, rx) × [ly, ry)のk番目に小さい要素が存在するか, 存在する場合その値
    std::pair<bool, Val> kth_smallest(Idx lx, Idx rx, Idx ly, Idx ry, int k) {
        int res = _kth_smallest(X.ord(lx), X.ord(rx), Y.ord(ly), Y.ord(ry), k);
        if (res == -1) {
            return {false, 0};
        } else {
            return {true, Z[res]};
        }
    }

    // [lx, rx) × [ly, ry)のk番目に大きい要素が存在するか, 存在する場合その値
    std::pair<bool, Val> kth_largest(Idx lx, Idx rx, Idx ly, Idx ry, int k) {
        int res = _kth_largest(X.ord(lx), X.ord(rx), Y.ord(ly), Y.ord(ry), k);
        if (res == -1) {
            return {false, 0};
        } else {
            return {true, Z[res]};
        }
    }
};
#endif
#ifndef _SQUARE_FREE_H_
#define _SQUARE_FREE_H_
#include <vector>
#include <cmath>

// [1, N]の無平方数
struct square_free {
    static std::vector<bool> is_prime;
    static std::vector<int8_t> mobius;
    
    static void init(int N) {
        is_prime = std::vector<bool>(N + 1, true);
        mobius = std::vector<int8_t>(N + 1, 1);
        for (int i = 2; i <= N; i++) {
            if (!is_prime[i]) continue;
            mobius[i] = -1;
            for (int j = 2 * i; j <= N; j += i) {
                is_prime[j] = 0;
                mobius[j] *= -1;
            }
            long long x = (long long)i * i;
            for (long long j = x; j <= N; j += x) mobius[j] = 0;
        }
    }
    static uint64_t slow(uint64_t N) {
        int s = sqrtl(N);
        init(s);
        long long ans = 0;
        for (long long i = 1; i <= s; i++) ans += mobius[i] * (N / (i * i));
        return ans;
    }

    static uint64_t fast(uint64_t N) {
        uint64_t I = 1;
        while (N > (I * I * I * I * I)) I++;
        int D = sqrtl(N / I);
        init(D);
        
        long long s1 = 0;
        for (int i = 1; i <= D; i++) s1 += mobius[i] * (N / ((long long)i * i)) ;
        std::vector<int> M_list{0};
        int M = 0;
        for (int i = 1; i <= D; i++) {
            M += mobius[i];
            M_list.push_back(M);
        }
        std::vector<int> Mxi_list;
        long long Mxi_sum = 0;
        for (int i = I - 1; i > 0; i--) {
            long long Mxi = 1;
            long long xi = sqrtl(N / i);
            int sqd = sqrt(xi);
            for (int j = 1; j <= xi / (sqd + 1); j++) {
                Mxi -= (xi / j - xi / (j + 1)) * M_list[j];
            }
            for (int j = 2; j <= sqd; j++) {
                if (xi / j <= D) {
                    Mxi -= M_list[xi / j];
                } else{
                    Mxi -= Mxi_list[I - j * j * i - 1];
                }
            }
            Mxi_list.push_back(Mxi);
            Mxi_sum += Mxi;
        }
        long long s2 = Mxi_sum - (I - 1) * M_list.back();
        return s1 + s2;
    }
};
std::vector<bool> square_free::is_prime;
std::vector<int8_t> square_free::mobius;
#endif
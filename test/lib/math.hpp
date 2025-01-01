#ifndef _INTEGER_H_
#define _INTEGER_H_
#include <cassert>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <numeric>
#include <array>
#include <limits>
#include <cmath>
#include <tuple>
#include <unordered_map>
#include <random>

// ak + b のl <= k < rにおける和
template<typename T = long long>
T arithmetic_sum(T a, T b, T l, T r){
  return a * (r * (r - 1) - l * (l - 1)) / 2 + b * (r - l);
}
// a * k^2の　l <= k < rにおける和
template<typename T = long long>
T square_sum(T a, T l, T r){
  return (r * (r - 1) * (2 * r - 1) - l * (l - 1) * (2 * l - 1)) / 6 * a;
}
// a^k * b のl <= k < rにおける和
// b * (a^r - a^l) / (a - 1)
template<typename T>
T geometric_sum(T a, T b, T l, T r){
  if(a == 1) return 0;
  return b * (a.pow(r) - a.pow(l)) / (a - 1);
}
// @param 0 <= b, a^bがオーバーフローしない
long long llpow(long long a, long long b){
  assert(0 <= b);
  long long ret = 1;
  while(b){
    if(b & 1) ret *= a;
    a *= a;
    b >>= 1;
  }
  return ret;
}
// a^b, オーバーフローする場合numeric_limits<ull>
// @param 0 <= a, b
unsigned long long ullpow_of(unsigned long long a, unsigned long long b){
  static constexpr unsigned long long inf = std::numeric_limits<unsigned long long>::max();
  unsigned long long ret = 1;
  while(b){
    if(b & 1){
      if(ret >= inf / a) return inf;
      ret *= a;
    }
    a = (a >= inf / a ? inf : a * a);
    b >>= 1;
  }
  return ret;
}

long long gcd(long long _a, long long _b){
  uint64_t a = abs(_a), b = abs(_b);
  if(a == 0) return b;
  if(b == 0) return a;
  int shift = __builtin_ctzll(a | b);
  a >>= __builtin_ctzll(a);
  do{
    b >>= __builtin_ctzll(b);
    if(a > b) std::swap(a, b);
    b -= a;
  }while(b);
  return (a << shift);
}
// 64bitに収まらない可能性あり
long long lcm(long long a, long long b){
  return __int128_t(a * b) / gcd(a, b);
}

std::tuple<long long, long long, long long> extgcd(long long a, long long b){
  long long x, y;
  for(long long u = y = 1, v = x = 0; a;){
    long long q = b / a;
    std::swap(x -= q * u, u);
    std::swap(y -= q * v, v);
    std::swap(b -= q * a, a);
  }
  return {x, y, b};//return {x, y, gcd(a, b)} s.t. ax + by = gcd(a, b)
}
constexpr long long __safe_mod(long long x, long long m){
  x %= m;
  if (x < 0) x += m;
  return x;
}
// @param b `1 <= b`
// @return pair(g, x) s.t. g = gcd(a, b), xa = g (mod b), 0 <= x < b/g
constexpr std::pair<long long, long long> inv_ext_gcd(long long a, long long b) {
  a = __safe_mod(a, b);
  if (a == 0) return {b, 0};
  long long s = b, t = a;
  long long m0 = 0, m1 = 1;
  while (t){
    long long u = s / t;
    s -= t * u;
    m0 -= m1 * u;
    auto tmp = s;
    s = t;
    t = tmp;
    tmp = m0;
    m0 = m1;
    m1 = tmp;
  }
  if (m0 < 0) m0 += b / s;
  return {s, m0};
}
// {lcm, ans}
std::pair<long long, long long> crt(const std::vector<long long>& r, const std::vector<long long>& m){
  assert(r.size() == m.size());
  int n = int(r.size());
  // Contracts: 0 <= r0 < m0
  long long r0 = 0, m0 = 1;
  for(int i = 0; i < n; i++){
    assert(1 <= m[i]);
    long long r1 = __safe_mod(r[i], m[i]), m1 = m[i];
    if(m0 < m1){
      std::swap(r0, r1);
      std::swap(m0, m1);
    }
    if(m0 % m1 == 0){
      if(r0 % m1 != r1) return {0, 0};
      continue;
    }
    long long g, im;
    std::tie(g, im) = inv_ext_gcd(m0, m1);
    long long u1 = (m1 / g);
    if ((r1 - r0) % g) return {0, 0};
    long long x = (r1 - r0) / g % u1 * im % u1;
    r0 += x * m0;
    m0 *= u1;
    if (r0 < 0) r0 += m0;
  }
  return {r0, m0};
}

// ai + bj = cの li <= i < ri かつ lj <= j < rjを満たす整数解
// 制約: a, b, c, li, ri, lj, riの絶対値が10^18以下(負でもいい)
//      a, b != 0
long long count_solution(long long a, long long b, long long c, long long li, long long ri, long long lj, long long rj){
  if(li >= (ri--) || lj >= (rj--)) return 0;
  if(a < 0){
    a *= -1, li *= -1, ri *= -1;
    std::swap(li, ri);
  }
  if(b < 0){
    b *= -1, lj *= -1, rj *= -1;
    std::swap(lj, rj);
  }
  assert(a && b);
  auto [i, j, g] = extgcd(a, b);
  if(c % g != 0) return 0;
  a /= g, b /= g, c /= g;
  __int128_t i2 = (__int128_t)i * c;
  __int128_t j2 = (__int128_t)j * c;
  // 任意の整数kに対してa(i2 + bk) + b(j2 - ak) = cを満たす
  __int128_t lk1 = i2 >= li ? -(i2 - li) / b : (li - i2 + b - 1) / b;
  __int128_t rk1 = i2 >= ri ? -(i2 - ri + b - 1) / b : (ri - i2) / b;
  __int128_t lk2 = j2 >= rj ? (j2 - rj + a - 1) / a : -(rj - j2) / a;
  __int128_t rk2 = j2 >= lj ? (j2 - lj) / a : -(lj - j2 + a - 1) / a;
  lk1 = std::max(lk1, lk2);
  rk1 = std::min(rk1, rk2);
  if(lk1 > rk1) return 0;
  return rk1 - lk1 + 1;
}

// 2^k >= xとなる最小の2^k
uint64_t ceil_2_pow(uint64_t x){
  static uint64_t INF = std::numeric_limits<uint64_t>::max();
  uint64_t ret = uint64_t(1) << (63 - __builtin_clzll(x));
  if(ret == x) return x;
  assert(ret <= (INF >> 1));
  return ret << 1;
}

#include <vector>
#include <algorithm>
#include <cstdint>
#include <algorithm>

struct barrett_reduction{
  unsigned int mod;
  unsigned long long m;
  barrett_reduction(unsigned int _mod) : mod(_mod){
    m = ((__uint128_t)1 << 64) / mod;
  }
  unsigned int reduce(unsigned int a){
    unsigned long long q = ((__uint128_t)a * m) >> 64;
    a -= q * mod; // 0 <= a < 2 * mod
    // return a;
    return a >= mod ? a - mod : a;
  }
  unsigned int mul(unsigned int a, unsigned int b){
    return reduce((unsigned long long)a * b);
  }
  // {gcd(a, mod), x}, such that a * x ≡ gcd(a, mod)
  std::pair<unsigned int, unsigned int> inv(unsigned int a){
    if(a >= mod) a = reduce(a);
    if(a == 0) return {mod, 0};
    unsigned int s = mod, t = a;
    long long m0 = 0, m1 = 1;
    while(t){
      int u = s / t;
      s -= t * u;
      m0 -= m1 * u;
      std::swap(m0, m1);
      std::swap(s, t);
    }
    if(m0 < 0) m0 += mod / s;
    return {s, m0};
  }
};
// 64bit mod対応
// R = 2^64
// 偶数modだと壊れる
struct montgomery_reduction_64bit{
private:
  // [0, 2 * MOD)
  inline uint64_t reduce_unsafe(__uint128_t x) const{
    x = (x + ((uint64_t)x * (uint64_t)NEG_INV) * MOD) >> 64;
    return x;
  }
  void _set_mod(uint64_t mod){
    assert((mod < (1ULL << 63)) && (mod & 1));
    MOD = mod;
    NEG_INV = 0;
    __uint128_t s = 1, t = 0;
    for(int i = 0; i < 64; i++){
      if (~t & 1) {
        t += MOD;
        NEG_INV += s;
      }
      t >>= 1;
      s <<= 1;
    }
    R2 = ((__uint128_t)1 << 64) % MOD;
    R2 = R2 * R2 % MOD;
    ONE = generate(1);
  }
  __uint128_t MOD, NEG_INV, R2;
  uint64_t ONE;
public:
  montgomery_reduction_64bit(){}
  montgomery_reduction_64bit(uint64_t mod){_set_mod(mod);}
  void set_mod(uint64_t mod){
    _set_mod(mod);
  }
  uint64_t mod()const{
    return MOD;
  }
  inline uint64_t one()const{
    return ONE;
  }
  inline uint64_t generate(uint64_t x)const{
    return reduce((__uint128_t)x * R2);
  }
  inline uint64_t reduce(__uint128_t x)const{
    x = (x + ((uint64_t)x * (uint64_t)NEG_INV) * MOD) >> 64;
    return x < MOD ? x : x - MOD;
  }
  inline uint64_t fix(uint64_t x)const{
    return x < MOD ? x : x - MOD;
  }
  // [0, 2MOD)
  inline uint64_t mul(uint64_t mx, uint64_t my)const{
    return reduce_unsafe((__uint128_t)mx * my);
  }
  inline uint64_t mul_safe(uint64_t mx, uint64_t my)const{
    return reduce((__uint128_t)mx * my);
  }
  // [0, 2MOD)
  inline uint64_t add(uint64_t mx, uint64_t my)const{
    return (mx >= MOD ? mx - MOD : mx) + (my >= MOD ? my - MOD : my);
  }
  inline uint64_t add_safe(uint64_t mx, uint64_t my)const{
    uint64_t res = (mx >= MOD ? mx - MOD : mx) + (my >= MOD ? my - MOD : my);
    return res >= MOD ? res - MOD : res;
  }
  // [0, 2MOD)
  inline uint64_t sub(uint64_t mx, uint64_t my)const{
    if(my >= MOD) my -= MOD;
    return mx >= my ? mx - my : mx + MOD - my;
  }
  inline uint64_t sub_safe(uint64_t mx, uint64_t my)const{
    if(my >= MOD) my -= MOD;
    uint64_t res = mx >= my ? mx - my : mx + MOD - my;
    return res >= MOD ? res - MOD : res;
  }
  inline uint64_t pow(uint64_t ma, uint64_t b)const{
    uint64_t ret = one();
    while(b){
      if(b & 1) ret = mul(ret, ma);
      ma = mul(ma, ma);
      b >>= 1;
    }
    return ret;
  }
  inline uint64_t pow_safe(uint64_t ma, uint64_t b)const{
    return fix(pow(ma, b));
  }
};
unsigned long long mod_pow_mr(unsigned long long a, unsigned long long b, unsigned long long m){
  montgomery_reduction_64bit mr(m);
  return mr.reduce(mr.pow(mr.generate(a), b));
}
namespace prime_sieve{
  std::vector<int> primes, min_factor;// 素数, 各数を割り切る最小の素数
  // O(MAX_N loglog MAX_N)
  void init(int MAX_N){
    min_factor.resize(MAX_N + 1, -1);
    for(int i = 2; i <= MAX_N; i++){
      if(min_factor[i] == -1){
        primes.push_back(i);
        min_factor[i] = i;
      }
      for(int p : primes){
        if((long long)p * i > MAX_N || p > min_factor[i]) break;
        min_factor[p * i] = p;
      }
    }
  }
  bool is_prime(int n){
    assert(n < min_factor.size());
    return n == min_factor[n];
  }
  // {{素因数, 数}}, O(log n)
  std::vector<std::pair<int, int>> factorize(int n){
    assert(n < min_factor.size());
    std::vector<std::pair<int, int>> ret;
    while(n > 1){
      int cnt = 0, f = min_factor[n];
      while(n % f == 0){
        n /= f;
        cnt++;
      }
      ret.push_back({f, cnt});
    }
    return ret;
  }
  // 約数列挙, O(√n)
  std::vector<int> divisor(int n){
    auto p = factorize(n);
    std::vector<std::vector<int>> x;
    for(int i = 0; i < p.size(); i++){
      x.push_back(std::vector<int>{1});
      for(int j = 0; j < p[i].second; j++) x[i].push_back(x[i][j] * p[i].first);
    }
    int l = 0, r = 1;
    std::vector<int> ret{1};
    for(int i = 0; i < x.size(); i++){
      for(auto e : x[i]){
        for(int j = l; j < r; j++){
          ret.push_back(ret[j] * e);
        }
      }
      l = r;
      r = ret.size();
    }
    return std::vector<int>(ret.begin() + l, ret.end());
  }
  // O(logN)
  unsigned long long totient_sieve(unsigned long long n){
    unsigned long long res = n;
    int prev = -1;
    while(n > 1){
      if(min_factor[n] > prev){
        res -= res / min_factor[n];
        prev = min_factor[n];
      }
      n /= min_factor[n];
    }
    return res;
  }
};

bool _miller_rabin_mr(unsigned long long n, const montgomery_reduction_64bit &mr){
  static std::vector<int> small_p{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
  static std::vector<unsigned long long> A{2, 325, 9375, 28178, 450775, 9780504, 1795265022};
  static std::vector<unsigned long long> B{2, 7, 61};
  if(n <= 1) return false;
  if(n <= 50){
    for(int l = n < 20 ? 0 : 8, r = n < 20 ? 8 : 15; l < r; l++) if(small_p[l] == n) return true;
    return false;
  }
  if(!(n & 1)) return false;
  unsigned long long d = n - 1;
  unsigned long long one = mr.one(), mone = mr.generate(n - 1);
  d >>= __builtin_ctzll(d);
  for(unsigned long long a : (n >> 32) ? A : B){
    if(a % n == 0) continue;
    unsigned long long d2s = d; // d * 2^s, 0 <= s <= (n - 1)が2で割れる回数
    unsigned long long y = mr.pow_safe(mr.generate(a), d);
    while(d2s != n - 1 && y != one && y != mone){
      y = mr.mul_safe(y, y);
      d2s <<= 1;
    }
    if(y != mone && !(d2s & 1)) return false;
  }
  return true;
}
bool miller_rabin_mr(unsigned long long n){
  if(n % 2 == 0) return n == 2 ? true : false;
  montgomery_reduction_64bit mr(n);
  return _miller_rabin_mr(n, mr);
}
// https://en.wikipedia.org/wiki/Binary_GCD_algorithm
unsigned long long binary_gcd(unsigned long long a, unsigned long long b){
  if(!a || !b) return !a ? b : a;
  int shift = __builtin_ctzll(a | b); // [1] gcd(2a', 2b') = 2 * gcd(a', b')
  a >>= __builtin_ctzll(a);
  do{
    // if b is odd
    // gcd(2a', b) = gcd(a', b), if a = 2a'(a is even)
    // gcd(a, b) = gcd(|a - b|, min(a, b)), if a is odd
    b >>= __builtin_ctzll(b); // make b odd
    if(a > b) std::swap(a, b);
    b -= a;
  }while(b); // gcd(a, 0) = a
  return a << shift; // [1]
}
unsigned long long __generate_random_prime(unsigned long long min_n = 2, unsigned long long max_n = ~0ULL){
  static std::random_device seed_gen;
  static std::mt19937_64 engine(seed_gen());
  unsigned long long len = max_n - min_n + 1;
  // about 40 tries per iteration. https://en.wikipedia.org/wiki/Prime_number_theorem
  while(true){
    unsigned long long a = engine() % len + min_n;
    if(miller_rabin_mr(a)){
      return a;
    }
  }
}
namespace rho_factorization{
  unsigned long long rho(unsigned long long n){
    static std::vector<int> small_p{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};

    for(int sp : small_p) if(n % sp == 0) return sp; // n < 50

    montgomery_reduction_64bit mr(n);
    if(_miller_rabin_mr(n, mr)) return n;

    auto try_factorize = [n, mr](unsigned long long c){
      c = mr.generate(c);
      auto f = [mr, c](unsigned long long mx){
        return mr.add(mr.mul(mx, mx), c);
      };
      unsigned long long m = 1LL << ((64 - __builtin_clzll(n)) / 8);
      unsigned long long y = n, r = 1, q = 1;
      unsigned long long x, g, k, ys;
      do{
        x = y;
        y = mr.generate(y);
        for(int i = 0; i < r; i++) y = f(y);
        y = mr.reduce(y);

        k = 0;
        while(k < r && g == 1){
          q = mr.generate(q);
          y = mr.generate(y);
          ys = y;
          for(int i = 0; i < std::min(m, r - k); i++){
            y = f(y);
            unsigned long long z = mr.reduce(y);
            q = mr.mul(q, mr.generate(x > z ? x - z : z - x));
          }
          y = mr.reduce(y);
          g = binary_gcd(mr.reduce(q), n);
          k += m;
        }
        r <<= 1;
      }while(g == 1);
      if(g == n){
        do{
          ys = f(ys);
          unsigned long long z = mr.reduce(ys);
          g = binary_gcd(x > z ? x - z : z - x, n);
        }while(g == 1);
      }
      return g; // g == n when failure
    };
    unsigned long long c = 1, res = n;
    do{
      res = try_factorize(c);
      // c = generate_random_prime(2, n - 1);
      c = (c + 1) % n;
    }while(res == n);
    return res;
  }
  std::vector<unsigned long long> factorize(unsigned long long n){
    if(n <= 1) return {};
    unsigned long long x = rho(n);
    if(x == n) return {x};
    auto l = factorize(x);
    auto r = factorize(n / x);
    l.insert(l.end(), r.begin(), r.end());
    return l;
  }
  // {素数, 個数}の形で返す
  std::vector<std::pair<unsigned long long, int>> factorize2(unsigned long long n){
    auto p = factorize(n);
    sort(p.begin(), p.end());
    std::vector<std::pair<unsigned long long, int>> ret;
    for(int i : p){
      if(ret.empty() || ret.back().first != i) ret.push_back({i, 1});
      else ret.back().second++;
    }
    return ret;
  }
  // 素因数の集合(重複なし, ソート済)を返す
  std::vector<unsigned long long> prime_factor(unsigned long long n){
    auto p = factorize(n);
    std::sort(p.begin(), p.end());
    p.erase(std::unique(p.begin(), p.end()), p.end());
    return p;
  }
  // 10^18以下の高度合成数 897612484786617600の約数が103680個なので全列挙して良さそう
  std::vector<unsigned long long> divisor(unsigned long long n){
    auto p = factorize(n);
    std::sort(p.begin(), p.end());

    std::vector<std::pair<unsigned long long, int>> x;

    for(int i = 0; i < p.size(); i++){
      if(!i || p[i] != p[i - 1]) x.push_back({p[i], 1});
      else x.back().second++;
    }
    int sz = 1;
    for(auto [p_cur, cnt] : x) sz *= cnt + 1;

    std::vector<unsigned long long> res(sz);
    res[0] = 1;
    int r_prev = 1, r = 1;
    for(auto [p_cur, cnt] : x){
      unsigned long long ppow = 1;
      for(int c = 0; c < cnt; c++){
        ppow *= p_cur;
        for(int i = 0; i < r_prev; i++){
          res[r++] = res[i] * ppow;
        }
      }
      r_prev = r;
    }
    return res;
  }
}
// p: 素数
unsigned long long find_primitive_root(unsigned long long p){
  static std::random_device seed_gen;
  static std::mt19937_64 engine(seed_gen());
  //assert(miller_rabin_mr(p));
  auto primes = rho_factorization::prime_factor(p - 1);
  while(true){
    bool f = true;
    unsigned long long a = engine() % (p - 1) + 1;
    for(unsigned long long pk : primes){
      // a ^ (p - 1) / pk ≡ 1 (mod p) -> no
      if(mod_pow_mr(a, (p - 1) / pk, p) == 1){
        f = false;
        break;
      }
    }
    if(f) return a;
  }
}

struct bigint_prime_factor{
  int sq, max_x;
  std::vector<int> low;
  std::unordered_map<int, int> high;
  bigint_prime_factor(int max_x): sq(sqrt(max_x)), max_x(max_x), low(sq + 1, 0){
    // 篩が初期化済か
    assert(prime_sieve::min_factor.size() > max_x);
  }
  // O(log(x))
  // x^kを掛ける
  void mul(int x, int k = 1){
    assert(0 < x && x <= max_x);
    while(x > 1){
      int p = prime_sieve::min_factor[x];
      if(p <= sq) low[p] += k;
      else{
        auto [itr, f] = high.emplace(x, k);
        if(!f) itr->second += k;
      }
      x /= p;
    }
  }
  // O(log(x))
  // x^kで割る(割り切れない場合エラー)
  void div(int x, int k = 1){
    assert(0 < x && x <= max_x);
    while(x > 1){
      int p = prime_sieve::min_factor[x];
      if(p <= sq){
        assert(low[p] >= k);
        low[p] -= k;
      }else{
        auto itr = high.find(p);
        assert(itr != high.end() && itr->second >= k);
        itr->second -= k;
        if(itr->second == 0) high.erase(itr);
      }
      x /= p;
    }
  }
  // 素数pで何回割れるか, O(1)
  // pが素数でない場合0
  int k_divisible_prime(int p){
    if(p > max_x || !prime_sieve::is_prime(p)) return 0;
    if(p > sq){
      auto itr = high.find(p);
      return itr == high.end() ? 0 : itr->second;
    }
    return low[p];
  }
  // xで何回割れるか, O(log(x))
  // x = 1のときinf回割れる
  // @param 0 < x <= 篩の最大値
  int k_divisible(int x){
    assert(x > 0);
    static constexpr int inf = std::numeric_limits<int>::max();
    int ans = inf;
    for(auto [p, num] : prime_sieve::factorize(x)){
      if(p > sq){
        auto itr = high.find(p);
        if(itr == high.end()) return 0;
        ans = std::min(ans, itr->second / num);
      }else{
        ans = std::min(ans, low[p] / num);
      }
    }
    return ans;
  }
  // rで何回割り切れるか, O(sqrt(r.max_x)以下の素数 + rの素因数の種類)
  int k_divisible(const bigint_prime_factor &r){
    static constexpr int inf = std::numeric_limits<int>::max();
    int ans = inf;
    
    for(int i = 0; prime_sieve::primes[i] <= r.sq; i++){
      int p = prime_sieve::primes[i];
      if(!r.low[p]) continue;
      if(p <= sq){
        if(low[p] < r.low[p]) return 0;
        ans = std::min(ans, low[p] / r.low[p]);
      }else{
        auto itr = high.find(p);
        if(itr->second < r.low[p]) return 0;
        ans = std::min(ans, itr->second / r.low[p]);
      }
    }
    for(auto [p, num] : r.high){
      assert(num);
      if(p <= sq){
        if(low[p] < num) return 0;
        ans = std::min(ans, low[p] / num);
      }else{
        auto itr = high.find(p);
        if(itr->second < num) return 0;
        ans = std::min(ans, itr->second / num);
      }
    }
    return ans;
  }
};
// a^k >= xとなる最小のa^k
uint64_t ceil_pow(uint64_t x, uint64_t a){
  assert(a > 1);
  if(x == 0) return 1;
  static uint64_t INF = std::numeric_limits<uint64_t>::max();
  if(a == 2){
    uint64_t ret = uint64_t(1) << (63 - __builtin_clzll(x));
    if(ret == x) return x;
    assert(ret <= (INF >> 1));
    return ret << 1;
  }
  if(a > 10){
    uint64_t ret = 1;
    while(true){
      if(ret > x / a) break;
      ret *= a;
    }
    if(ret == x) return ret;
    assert(ret <= INF / a);
    return ret * a;
  }
  static std::vector<std::vector<uint64_t>> pow_table(11);
  if(pow_table[a].empty()){
    uint64_t tmp = 1;
    pow_table[a].push_back(1);
    while(true){
      if(tmp > INF / a) break;
      pow_table[a].push_back(tmp *= a);
    }
  }
  auto itr = std::lower_bound(pow_table[a].begin(), pow_table[a].end(), x);
  assert(itr != pow_table[a].end());
  return *itr;
}
// a^k <= xを満たす最大のa^k
uint64_t floor_pow(uint64_t x, uint64_t a){
  assert(x > 0 && a > 1);
  if(a == 2) return uint64_t(1) << (63 - __builtin_clzll(x));
  if(a > 10){
    uint64_t ret = 1;
    while(true){
      if(ret > x / a) return ret;
      ret *= a;
    }
  }
  static std::vector<std::vector<uint64_t>> pow_table(11);
  static uint64_t INF = std::numeric_limits<uint64_t>::max();
  if(pow_table[a].empty()){
    uint64_t tmp = 1;
    pow_table[a].push_back(1);
    while(true){
      if(tmp > INF / a) break;
      pow_table[a].push_back(tmp *= a);
    }
  }
  return *--std::upper_bound(pow_table[a].begin(), pow_table[a].end(), x);
}

// floor(x / x以下の自然数)としてあり得る値の昇順
template<typename T>
std::vector<T> enumerate_quotients(T x){
  assert(x >= 0);
  if(x == 0) return {};
  T sq = sqrtl(x);
  std::vector<T> ans(sq);
  std::iota(ans.begin(), ans.end(), 1);
  if(x / sq != ans.back()) ans.push_back(x / sq);
  for(T i = sq - 1; i >= 1; i--) ans.push_back(x / i);
  return ans;
}
// floor(x / y(x以下の自然数))としてあり得る値の昇順
// {商, yの最小値, yの最大値 + 1}
template<typename T>
std::vector<std::tuple<T, T, T>> enumerate_quotients3(T x){
  assert(x >= 0);
  if(x == 0) return {};
  T sq = sqrtl(x);
  std::vector<std::tuple<T, T, T>> ans(sq);
  for(T i = 1, pre1 = x; i <= sq; i++){
    T pre2 = x / (i + 1);
    ans[i - 1] = {i, pre2 + 1, pre1 + 1};
    std::swap(pre1, pre2);
  }
  if(x / sq == std::get<0>(ans.back())) sq--;
  for(T i = sq; i >= 1; i--){
    ans.push_back({x / i, i, i + 1});
  }
  return ans;
}
// enumerate_quotients(x)を呼んだ時にyは何番目の要素に含まれるか
// 含まれない場合は-1
template<typename T>
int index_quotients(T x, T y){
  if(y <= 0 || y > x) return -1;
  T z = x / y;
  if(z <= y) return z;
  T sq = sqrtl(x);
  if(x / sq == sq){
    return 2 * sq - 1 - y; // 2sq - 1型
  }else{
    return 2 * sq - y; // 2sq型
  }
}
// enumerate_quotients(x)を呼んだ時のk番目の要素
// ない場合は-1
template<typename T>
T kth_quotients(T x, int k){
  T sq = sqrtl(x);
  if(k < sq) return k + 1;
  int len = 2 * sq - (x / sq == sq);
  if(len <= k) return -1;
  return x / (len - k);
}
// enumerate_quotients3(x)を呼んだ時のk番目の要素
// ない場合は-1
template<typename T>
std::tuple<T, T, T> kth_quotients3(T x, int k){
  T sq = sqrtl(x);
  if(k < sq) return {k + 1, x / (k + 2) + 1, x / (k + 1) + 1};
  int len = 2 * sq - (x / sq == sq);
  if(len <= k) return {-1, -1, -1};
  len -= k;
  return {x / len, len, len + 1};
}

// x%1 + x%2 ... x%n
// @param 0 <= x, n
// O(√x)
long long mod_sum(long long x, long long n){
  assert(0 <= x && 0 <= n);
  if(x == 0) return 0;
  long long ans = x * n;
  if(n > x) n = x;
  // floor(x / i)の値でグループ分け
  long long sq = sqrtl(x);
  for(int i = 1; i <= sq; i++){
    long long l = x / (i + 1) + 1, r = std::min(n + 1, x / i + 1);
    if(l < r){
      long long sum_lr = (r * (r - 1) - l * (l - 1)) / 2;
      ans -= i * sum_lr;
    }
  }
  if(x / sq == sq) sq--;
  for(int i = std::min(n, sq); i >= 1; i--){
    ans -= i * (x / i);
  }
  return ans;
}

// a ^ k <= xを満たす最大のa
uint64_t kth_root_stable(uint64_t x, uint64_t k){
  if(!x) return 0;
  if(k == 1 || x == 1) return x;
  if(k >= 64) return 1;
  uint64_t l = 1, r = x;
  const static uint64_t threshold = std::numeric_limits<uint64_t>::max();
  while(r - l > 1){
    bool f = false;
    uint64_t mid = l + ((r - l) >> 1), z = 1;
    uint64_t lim = threshold / mid;
    for(int i = 0; i < k; i++){
      if(z > lim){
        f = true;
        break;
      }
      z *= mid;
    }
    if(f | (z > x)) r = mid;
    else l = mid;
  }
  return l;
}

// a^k <= x を満たす最大のa
uint64_t kth_root_fast(uint64_t x, uint64_t k){
  if(x <= 1) return (!k ? 1 : x);
  if(k <= 2) return (k <= 1 ? !k ? 1 : x : sqrtl(x));
  if(k >= 64||!(x >> k)) return 1;
  const static int sz[8] = {40, 31, 27, 24, 22, 21, 20, 19};
  static std::vector<std::vector<uint64_t>> table;
  if(table.empty()){
    table.resize(40);
    for(int i = 0; i < 40; i++){
      for(uint64_t j = 0; j < 8 && sz[j] > i; j++){
        table[i].push_back((!i ? 1 : table[i - 1][j]) *(j + 3));
      }
    }
  }
  if(k >= 19){
    int ans = 10;
    for(;ans > 2; ans--){
      if(sz[ans - 3]<k || table[k - 1][ans - 3] > x) continue;
      return ans;
    }
    return 2;
  }
  uint64_t r = (k != 3 ? pow(x, (1.0 + 1e-6) / k) : cbrt(x) + 1);
  const static uint64_t threshold = std::numeric_limits<uint64_t>::max();
  while(true){
    uint64_t lim = threshold / r, z = 1;
    for(int i = 0; i < k; i++, z *= r) if(z > lim) goto upper;
    if(z > x) upper: r--;
    else break;
  }
  return r;
}


std::vector<long long> v{1,2,4,6,12,24,36,48,60,120,180,240,360,720,840,1260,1680,2520,5040,7560,10080,15120,20160,25200,27720,45360,50400,55440,83160,110880,166320,221760,277200,332640,498960,554400,665280,720720,1081080,1441440,2162160,2882880,3603600,4324320,6486480,7207200,8648640,10810800,14414400,17297280,21621600,32432400,36756720,43243200,61261200,73513440,110270160,122522400,147026880,183783600,245044800,294053760,367567200,551350800,698377680,735134400,1102701600,1396755360,2095133040,2205403200,2327925600,2793510720,3491888400,4655851200,5587021440,6983776800,10475665200,13967553600,20951330400,27935107200,41902660800,48886437600,64250746560,73329656400,80313433200,97772875200,128501493120,146659312800,160626866400,240940299600,293318625600,321253732800,481880599200,642507465600,963761198400,1124388064800,1606268664000,1686582097200,1927522396800,2248776129600,3212537328000,3373164194400,4497552259200,6746328388800,8995104518400,9316358251200,13492656777600,18632716502400,26985313555200,27949074753600,32607253879200,46581791256000,48910880818800,55898149507200,65214507758400,93163582512000,97821761637600,130429015516800,195643523275200,260858031033600,288807105787200,391287046550400,577614211574400,782574093100800,866421317361600,1010824870255200,1444035528936000,1516237305382800,1732842634723200,2021649740510400,2888071057872000,3032474610765600,4043299481020800,6064949221531200,8086598962041600,10108248702552000,1212898443062400,18194847664593600,20216497405104000,24259796886124800,30324746107656000,36389695329187200,48519593772249600,60649492215312000,72779390658374400,74801040398884800,106858629141264000,112201560598327200,149602080797769600,224403121196654400,299204161595539200,374005201994424000,448806242393308800,673209363589963200,748010403988848000,897612484786617600};

std::vector<long long> ans{1,2,3,4,6,8,9,10,12,16,18,20,24,30,32,36,40,48,60,64,72,80,84,90,96,100,108,120,128,144,160,168,180,192,200,216,224,240,256,288,320,336,360,384,400,432,448,480,504,512,576,600,640,672,720,768,800,864,896,960,1008,1024,1152,1200,1280,1344,1440,1536,1600,1680,1728,1792,1920,2016,2048,2304,2400,2688,2880,3072,3360,3456,3584,3600,3840,4032,4096,4320,4608,4800,5040,5376,5760,6144,6720,6912,7168,7200,7680,8064,8192,8640,9216,10080,10368,10752,11520,12288,12960,13440,13824,14336,14400,15360,16128,16384,17280,18432,20160,20736,21504,23040,24576,25920,26880,27648,28672,28800,30720,32256,32768,34560,36864,40320,41472,43008,46080,48384,49152,51840,53760,55296,57600,61440,62208,64512,65536,69120,73728,80640,82944,86016,92160,96768,98304,103680
};

// 高度合成数(N以下の数の中で最も多い約数を持つ数)
// {N以下の高度合成数, その約数}
std::pair<long long, long long> highly_composite_number(long long N){
  assert(0 < N && N <= 1000000000000000000);
  int idx = upper_bound(v.begin(), v.end(), N) - v.begin() - 1;
  assert(idx != 0);
  return {v[idx], ans[idx]};
}


//---tips---
//φ(n) := [1, n]の中でnと互いに素な正整数の数
//φ(n) =  n * π{p:prime factor of n}(1 - 1/p)
//aとnが互いに素な場合, a^φ(n) ≡ 1 (mod n)
unsigned long long totient(unsigned long long n){
  unsigned long long res = n;
  auto prims = rho_factorization::prime_factor(n);
  for(auto p : prims) res -= res / p;
  return res;
}
// n以下のtotient_sum
std::vector<unsigned long long> totient_sum_table(unsigned long long n){
  std::vector<unsigned long long> res(n + 1);
  std::iota(res.begin() + 1, res.end(), 0);
  res[1] = 1;
  for(int i = 2; i <= n; i++){
    // prime
    if(res[i] == i - 1){
      for(int j = 2 * i; j <= n; j += i){
        res[j] -= res[j] / i;
      }
    }
  }
  for(int i = 2; i <= n; i++) res[i] += res[i - 1];
  return res;
}
// [1]
// totient_sum(n) = (i, j){1 <= i <= n, 1 <= j <= i}となるペアの数 - そのうちgcd(i, j) != 1であるものの数
//                = n * (n + 1) / 2 - ∑{2 <= g <= n} gcd(i, j) == gのペア
//                = n * (n + 1) / 2 - ∑{2 <= g <= n} totient_sum(n / g)

// この演算をメモ化再帰で行った時の n / g (切り捨て)の種類数について考える

// [2]
// floor(x / ab) = floor(floor(x / a) / b)
// 証明:
// x = qab + r (0 <= r < ab)とすると
// 左辺 = floor(q + r / ab) = q
// 右辺 = floor((qb + r') / b) (0 <= r' < b) = q

// [3]
// [2]よりnを正整数で0回以上除算した時に得られる商はO(√n)種類
__uint128_t totient_sum(unsigned long long n){
  static std::vector<unsigned long long> low;
  if(low.empty()) low = totient_sum_table(std::min(n, (unsigned long long)4000000));
  std::unordered_map<unsigned long long, __uint128_t> mp;

  unsigned long long memo_max = 0;
  auto tsum = [&](auto &&tsum, unsigned long long m)->__uint128_t{
    if(m < low.size()) return low[m];
    if(m <= memo_max) return mp.at(m);
    __uint128_t res = (__uint128_t)m * (m + 1) / 2;
    auto d = enumerate_quotients3(m);
    std::reverse(d.begin(), d.end());
    for(auto [q, a, b] : d){
      if(q == m) continue;
      res -= (b - a + 1) * tsum(tsum, q);
    }
    mp.emplace(m, res);
    memo_max = m;
    return res;
  };

  return tsum(tsum, n);
}


/*
// mが固定の場合, 定数の前計算やmontgomery reductionによる高速化ができる
// 解が無いと壊れる
struct fixed_mod_garner{
  std::vector<unsigned long long> m, iM, accum;
  std::vector<std::vector<unsigned long long>> M; // M[i][j] := (m_1 ~ m_j-1の積) % m_i
  std::vector<montgomery_reduction_64bit> mr;

  void set_mod(std::vector<unsigned long long> _m){
    int n = _m.size();
    std::unordered_map<long long, std::pair<long long, int>> P;
    for(int i = 0; i < n; i++){
      auto f = rho_factorization::factorize(_m[i]);
      std::sort(f.begin(), f.end());
      f.push_back(0);
      long long p = -1, p_pow = 1;
      for(int j = 0; j < f.size(); j++){
        if(f[j] == p){
          p_pow *= p;
        }else{
          if(p != -1){
            auto itr = P.find(p);
            if(itr == P.end()){
              P.emplace(p, std::make_pair(p_pow, i));
            }else{
              auto [p_pow_max, idx_max] = itr->second;
              if(p_pow_max < p_pow){
                _m[idx_max] /= p_pow_max;
                itr->second = std::make_pair(p_pow, i);
              }else{
                _m[i] /= p_pow;
              }
            }
          }
          p = f[j];
          p_pow = p;
        }
      }
    }
    m = _m;
    mr.resize(n + 1);
    for(int i = 0; i < n; i++) mr[i].set_mod(m[i]);
    M.resize(n + 1, std::vector<unsigned long long>(n + 1));
    for(int i = 0; i < n; i++){
      M[i][0] = mr[i].generate(1);
      for(int j = 1; j <= i; j++){
        M[i][j] = mr[i].mul(M[i][j - 1], mr[i].generate(m[j - 1]));
      }
    }
    iM.resize(n);
    for(int i = 0; i < n; i++) iM[i] = mr[i].generate(inv_gcd(mr[i].reduce(M[i][i]), m[i]).second);
    accum.resize(n + 1, 0);
  }
  unsigned long long query(std::vector<unsigned long long> r, unsigned long long mod){
    int n = r.size();
    assert(n == m.size());
    std::fill(accum.begin(), accum.end(), 0);
    mr[n].set_mod(mod);
    M[n][0] = mr[n].generate(1);
    for(int i = 1; i <= n; i++) M[n][i] = mr[n].mul(M[n][i - 1], mr[n].generate(m[i - 1]));
    for(int i = 0; i < n; i++){
      r[i] = mr[i].generate(r[i] % m[i]);
      unsigned long long x = mr[i].reduce(mr[i].mul(mr[i].sub(r[i], accum[i]), iM[i]));
      for(int j = i + 1; j <= n; j++) accum[j] = mr[j].add(accum[j], mr[j].mul(M[j][i], mr[j].generate(x)));
    }
    return mr[n].reduce(accum[n]);
  }
};
*/
#endif
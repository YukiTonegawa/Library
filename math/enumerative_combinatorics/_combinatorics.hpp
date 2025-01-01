/*

struct four_square_theorem{
  int n;
  std::vector<int> A;
  std::vector<int> sz;
  // 0 <= i < Nについて以下のテーブルを作る
  // sum(A) = iを満たしlen(A)が最小(常に4つ以下になる)
  // O(N√N)
  // N = 10^6で1sec程度
  void init(int _n){
    n = _n;
    A.resize(n, -1);
    sz.resize(n, 5);
    sz[0] = 0;
    for(int i = 1; i < n; i++){
      for(int j = 1; j * j <= i; j++){
        if(sz[i - j * j] + 1 < sz[i]){
          A[i] = j;
          sz[i] = sz[i - j * j] + 1;
        }
      }
    }
  }
  four_square_theorem(int _n){
    init(_n);
  }
  // x = ∑A[i]^2 を満たす要素数最小のA(高々4つ)
  std::vector<int> get_square(int x){
    assert(n);
    assert(0 <= x && x < n);
    std::vector<int> ret;
    while(x){
      int y = A[x];
      ret.push_back(y);
      x -= y * y;
    }
    return ret;
  }
  // x = ∑A[i]^2 を満たすA(要素数最小とは限らない, N = 10^5, x < 10^18なら高々6要素になる)
  // @param x <= 10^18
  std::vector<unsigned long long> get_square_large(unsigned long long x){
    std::vector<unsigned long long> ret;
    while(n <= x){
      unsigned long long y = sqrtl(x);
      ret.push_back(y);
      x -= y * y;
    }
    while(x){
      int y = A[x];
      ret.push_back(y);
      x -= y * y;
    }
    return ret;
  }
  // x = ∑ A[i] * (A[i] + 1) / 2 を満たすA(3つ以下)
  std::vector<int> get_trianglar(int x){
    auto v = get_square(8 * x + 3);
    assert(v.size() == 3);
    std::vector<int> ret;
    for(int i = 0; i < 3; i++) if(v[i] != 1) ret.push_back(v[i] / 2);
    return ret;
  }
  // x = ∑ A[i] * (A[i] + 1) / 2 を満たすA(要素数最小とは限らない, N = 10^5, x < 10^18なら高々5要素になる)
  // @param x <= 10^18
  std::vector<unsigned long long> get_trianglar_large(unsigned long long x){
    std::vector<unsigned long long> ret;
    while(n <= 8 * x + 3){
      unsigned long long y = sqrtl(2 * x);
      while(y * (y + 1) > 2 * x) y--;
      ret.push_back(y);
      x -= (y & 1 ? y * ((y >> 1) + 1) : (y >> 1) * (y + 1));
    }
    auto v = get_square(8 * x + 3);
    for(int i = 0; i < 3; i++) if(v[i] != 1) ret.push_back(v[i] / 2);
    return ret;
  }
};

// ピタゴラス数について
// a^2 + b^2 = c^3を満たす自然数の組(a, b, c)をピタゴラス数と呼ぶ
// bが偶数, a, cが奇数でなければならない
// 自然数x, y(x > y)を用いて(a, b, c) = (x^2 - y^2, 2xy, x^2 + y^2)を表すことができる　


// A[i] := c1 * nC1 + c2 * nC2 + .... cn * nCn
// cが与えられた時, 上の式を満たすAを返す
// cが全部1なら二項係数の式変形で解ける

int main(){
  io_init();
  int n;
  std::cin >> n;
  modcomb<mint> mcb(n);
  simple_tree t(n);
  range(i, 0, n - 1){
    int a, b;
    std::cin >> a >> b;
    a--, b--;
    t.add_edge(a, {a, b});
    t.add_edge(b, {b, a});
  }

  vector<mint> c(n);
  range(i, 0, n){
    c[n - 1]++;
    for(auto e : t[i]){
      int sz = (e.t == t.par(i) ? n - t.size(i) : t.size(e.t));
      c[sz - 1]--;
    }
  }
  // T(n) = 2 * T(n / 2) + O(nlog(n))
  // ci * (1 + x) ^ i

  int N = 1 << bit_highest(n);
  if(N != n) N <<= 1;
  vector<fps> p2(19);
  p2[0] = {1, 1};
  range(i, 1, 19) p2[i] = p2[i - 1] * p2[i - 1];

  auto f = [&](auto &&f, int l, int r) -> fps {
    if(r - l == 1){
      if(l >= n) return fps{0};
      return fps{c[l], c[l]};
    }
    int mid = (l + r) / 2;
    auto L = f(f, l, mid);
    auto R = f(f, mid, r);
    int h = bit_highest(mid - l);
    assert((1 << h) == mid - l);
    R = R * p2[h] + L;
    return R;
  };
  fps ans = f(f, 0, N);
  range(i, 1, n + 1) std::cout << (ans.size() <= i ? 0 : ans[i]) << '\n';
}
#endif
*/
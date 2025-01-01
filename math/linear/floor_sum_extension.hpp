/*

// [xi + y, xi + z) (0 <= y < z <= x)と
// [aj + b, aj + c) (0 <= b < c <= a)が交差する最小の非負整数i
// 解がない場合は-1
long long find_intersection_of_periodic_segments_fast(long long x, long long y, long long z, long long a, long long b, long long c){
  assert(x > 0 && a > 0);
  assert(0 <= y && y < z && z <= x);
  assert(0 <= b && b < c && c <= a);
  // [0, xi + z - 1 - b)と[0, xi + y - c)で含まれるaの倍数の数が異なる < - > (xi + (z - 1 - b)) % aがdiff未満になる最小のi
  long long diff = (z - 1 - b) - (y - c);
  assert(diff > 0);
  return next_range_mod_linear_fast(0, diff, x, z - 1 - b, a);
}

// [xi + y, xi + z) (0 <= y < z <= x)と
// [aj + b, aj + c) (0 <= b < c <= a)が交差する最小の非負整数i
// 解がない場合は-1
long long find_intersection_of_periodic_segments(long long x, long long y, long long z, long long a, long long b, long long c){
  assert(x > 0 && a > 0);
  assert(0 <= y && y < z && z <= x);
  assert(0 <= b && b < c && c <= a);
  long long ag = a / gcd(x, a);
  long long L = 0, R = ag + 1;
  while(R - L > 1){
    long long M = (L + R) / 2;
    if(floor_sum(M, x, z - 1 + a - b, a) == floor_sum(M, x, y + a - c, a)) L = M;
    else R = M;
  }
  if(R == ag + 1) return -1;
  return L;
}
*/
/*
// 0 <= a < cのとき、[ai + b, ci + d]にeの倍数(負の数も含む)が含まれないような自然数iの数(有限)
long long simple_math3(long long a, long long b, long long c, long long d, long long e){
  assert(a >= 0 && c >= 0 && a < c && a + b <= c + d);
  if(std::min(b, d) < 0){
    long long add = (abs(std::min(b, d) + e - 1) / e) * e;
    b += add;
    d += add;
  }
  long long k = (e - 2 - (d - b)) / (c - a);
  if(k <= 0) return 0;
  return k - floor_sum(k, c, c + d, e) + floor_sum(k, a, a + b - 1, e);
}
*/
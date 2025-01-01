/*
template<typename Idx, typename compare_struct>
struct offline_dynamic_rectangle_chmin_point_get{
  using Val = typename compare_struct::Val;
private:
  struct Rect{
    Idx lx, rx, ly, ry;
    int t;
    Val z;
  };
  std::vector<Rect> R;
  kd_tree_3d<Idx, int> kdt;
  using Point = typename kd_tree_3d<Idx, int>::point;
  std::vector<Point> P;
public:
  void update(Idx lx, Idx rx, Idx ly, Idx ry, Val z){
    int qcnt = P.size();
    R.push_back({lx, rx, ly, ry, qcnt, z});
  }
  void query(Idx x, Idx y){
    int qcnt = P.size();
    P.push_back(Point(x, y, qcnt, qcnt));
  }
  // 追加される長方形を値の小さい順に見る
  // これがある質問点を含む場合, その質問点を消していい(それ以降の長方形はより値が大きいため)
  std::vector<Val> solve(){
    kdt = kd_tree_3d<Idx, int>(P);
    std::sort(R.begin(), R.end(), [&](const Rect &a, const Rect &b){
      return compare_struct::compare(a.z, b.z);
    });
    std::vector<Val> ans(P.size(), compare_struct::id());
    for(Rect &r : R){
      auto V = kdt.range_find(r.lx, r.rx, r.ly, r.ry, r.t, P.size());
      for(auto v : V){
        ans[v->p.w] = r.z;
        kdt.erase(v);
      }
    }
    return ans;
  }
};
*/
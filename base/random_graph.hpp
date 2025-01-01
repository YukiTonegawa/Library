#ifndef _RANDOM_GRAPH_H_
#define _RANDOM_GRAPH_H_
#include "random.hpp"

struct random_graph {
    random_generator r;
    random_graph(const int seed = 4321) : r(seed) {}

};

// テスト用ライブラリ
// グラフ可視化
// 簡単なケース生成
// 数列の情報をまとめる 和, 
// p-recursive
#endif
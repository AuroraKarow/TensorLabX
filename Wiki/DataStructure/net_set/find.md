# `find`

## 重載 Overload $^1$

```c++
net_set<uint64_t> find(const arg &tgt,
                       uint64_t  fst_rng,
                       uint64_t  snd_rng);
```

在指定範圍閉區間内尋找目標值在集合中的索引集合。\
Find index set of target value in set by specified range closed interval.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`tgt`|目標值<br>Target value|*輸入<br>input*
`fst_rng`|範圍區間的第一個索引值<br>First index of the range interval|*輸入<br>input*
`snd_rng`|範圍區間的第二個索引值<br>Second index of the range interval|*輸入<br>input*

返回目標值在集合中所有索引的集合，如果範圍區間不合法或不存在目標值則返回空集合。\
Return the set of all target value indexes in set, if the range interval is illegal or there is not target value in set, it would return blank set.

## 重載 Overload $^2$

```c++
net_set<uint64_t> find(const arg &tgt);
```

在整個集合中尋找目標值的所有索引並返回集合，請參閲[重載$^1$](#重載-overload-1)
Find all indexes of target value in set and return set, please refer to [Overload$^1$](#重載-overload-1).

## 重載 Overload $^3$

```c++
net_set<uint64_t> find(const arg               &tgt,
                       const net_set<uint64_t> &idx_set);
```

在指定集合索引中尋找集合元素是目標值的所有索引並返回索引集合。更多請參閲 [`sub_set`](sub_set.md)\
Find all indexes of the specified indexes set of the set whose elements are target element and return the index set. Please refer to [`sub_set`](sub_set.md) for more details.

```c++
using std::cout;
using std::endl;
using neunet::net_set;
// 初始化一个矩阵列集合
// Initialize a matrix set
net_set<vect> test = {
    {{11, 19, 21}},

    {{0, 1},
     {2, 3}},

    {{1, 3, 5},
     {7, 9,11}},
     
    {{0, 2},
     {4, 6},
     {8,10}},

    {{1, 3, 5},
     {7, 9,11}}
};
/* 根據條件在集合中尋找矩陣
 * Find matrix
 * 1 3  5
 * 7 9 11
 * in set with given condition
 */  // 全集合尋找
     // Find in whole set
auto all_idx = test.find({{1, 3, 5},
                          {7, 9,11}}),
     rit_idx = test.find({{1, 3, 5},
                          {7, 9,11}},
     // 在索引區間 [0, 4] 中尋找
     // Find in index interval [0, 4]
                           0, 4),
     spc_idx = test.find({{1, 3, 5},
                          {7, 9,11}},
     // 在索引集合 {0, 2, 4} 中尋找
     // Find in index set {0, 2, 4}
                          {0, 2, 4});
// 以下都會打印 1
// They would print 1 all below
cout << (all_idx == rit_idx) << endl;
cout << (rit_idx == spc_idx) << endl;
// 打印索引集
// Print index set
cout << spc_idx << endl;
```

[<< 返回 Back](cover.md)

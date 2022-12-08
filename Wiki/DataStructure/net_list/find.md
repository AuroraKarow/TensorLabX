# `find`

## 重載 Overload $^1$

```c++
net_set<uint64_t> find(const arg &tgt,
                       uint64_t  fst_rng,
                       uint64_t  snd_rng);
```

在指定範圍閉區間内尋找目標值在表中的索引集合。\
Find index set of target value in list by specified range closed interval.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`tgt`|目標值<br>Target value|*輸入<br>input*
`fst_rng`|範圍區間的第一個索引值<br>First index of the range interval|*輸入<br>input*
`snd_rng`|範圍區間的第二個索引值<br>Second index of the range interval|*輸入<br>input*

返回目標值在表中所有索引的集合，如果範圍區間不合法或不存在目標值則返回空集合。\
Return the set of all target value indexes in list, if the range interval is illegal or there is not target value in list, it would return blank set.

## 重載 Overload $^2$

```c++
net_set<uint64_t> find(const arg &tgt);
```

在整個表中尋找目標值的所有索引並返回集合，請參閲[重載$^1$](#重載-overload-1)
Find all indexes of target value in list and return set, please refer to [Overload$^1$](#重載-overload-1).

## 重載 Overload $^3$

```c++
net_set<uint64_t> find(const arg               &tgt,
                       const net_set<uint64_t> &idx_set);
```

在指定表索引中尋找表元素是目標值的所有索引並返回索引集合。更多請參閲 [`sub_list` 重載$^2$](sub_list.md#重載-overload-2)\
Find all indexes of the specified indexes set of the list whose elements are target element and return the index set. Please refer to [`sub_list` Overload$^2$](sub_list.md#重載-overload-2) for more details.

```c++
using std::cout;
using std::endl;
using neunet::net_list;
// 初始化一个矩阵列表
// Initialize a matrix list
net_list<vect> test = {
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
/* 根據條件在表中尋找矩陣
 * Find matrix
 * 1 3  5
 * 7 9 11
 * in list with given condition
 */  // 全表尋找
     // Find in whole list
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
// 一下都會打印 1
// They would print 1 all below
cout << (all_idx == rit_idx) << endl;
cout << (rit_idx == spc_idx) << endl;
// 打印索引集
// Print index set
cout << spec_idx << endl;
```

[<< 返回 Back](cover.md)

---

*更多關於動態集合請參閲* [`net_set`](../net_set/cover.md)\
*Please refer to* [`net_set`](../net_set/cover.md) *for more about dynamic set*

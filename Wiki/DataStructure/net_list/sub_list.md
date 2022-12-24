# `sub_list`

## 重載 Overload $^1$

```c++
net_list sub_list(uint64_t fst_rng,
                  uint64_t snd_rng);
```

通过两个列表索引之间的一个闭区间来获取子列表。\
Get sub list by a closed interval of a range between two list index.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`fst_rng`|範圍區間的第一個索引值<br>First index of the range interval|*輸入<br>input*
`snd_rng`|範圍區間的第二個索引值<br>Second index of the range interval|*輸入<br>input*

返回指定範圍的子表。如果獲取失敗或表為空，則返回空表。\
Return the sub list by specified range. It would return blank list for getting failed or current list in blank.

## 重載 Overload $^2$

```c++
net_list sub_list(const net_set<uint64_t> &idx_set);
```

通过指定的索引集获取子列表。這個函數會將有效但重複的索引剔除。\
Get sub list by specified index set. This function would delete deuplicated but valid index.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`idx_set`|索引集<br>Index set|*輸入<br>input*

如果索引集有效返回子表，否則為空表。\
Return sub list for valid index set otherwise blank list.

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
// 获得从索引0到3的子列表，并打印它
// Get sub list from index 0 to 3 and print it
cout << test.sub_list(0, 3) << endl;
cout << endl;
// 打印结果應該是一样的
// Print result should be same
cout << test.sub_list({0, 1, 2, 3, 2}) << endl;
```

[<< 返回 Back](cover.md)

---

*更多關於動態集合請參閲* [`net_set`](../net_set/cover.md)\
*Please refer to* [`net_set`](../net_set/cover.md) *for more about dynamic set*

*更多關於類型* `vect` *請參閲* [`net_matrix`](../../MatrixCalculation/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../../MatrixCalculation/net_matrix/cover.md) *for more about type* `vect`

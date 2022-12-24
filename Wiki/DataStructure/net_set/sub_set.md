# `sub_set`

## 重載 Overload $^1$

```c++
net_set sub_set(uint64_t fst_rng,
                  uint64_t snd_rng);
```

通过两个集合索引之间的一个闭区间来获取子集合。\
Get sub set by a closed interval of a range between two set index.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`fst_rng`|範圍區間的第一個索引值<br>First index of the range interval|*輸入<br>input*
`snd_rng`|範圍區間的第二個索引值<br>Second index of the range interval|*輸入<br>input*

返回指定範圍的子表。如果獲取失敗或表為空，則返回空表。\
Return the sub set by specified range. It would return blank set for getting failed or current set in blank.

## 重載 Overload $^2$

```c++
net_set sub_set(const net_set<uint64_t> &idx_set);
```

通过指定的索引集获取子集合。這個函數會將有效但重複的索引剔除。\
Get sub set by specified index set. This function would delete deuplicated but valid index.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`idx_set`|索引集<br>Index set|*輸入<br>input*

如果索引集有效返回子表，否則為空表。\
Return sub set for valid index set otherwise blank set.

```c++
using std::cout;
using std::endl;
using neunet::net_set;
// 初始化一个矩阵集合
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
// 获得从索引0到3的子集合，并打印它
// Get sub set from index 0 to 3 and print it
cout << test.sub_set(0, 3) << endl;
cout << endl;
// 打印结果應該是一样的
// Print result should be same
cout << test.sub_set({0, 1, 2, 3, 2}) << endl;
```

[<< 返回 Back](cover.md)

---

*更多關於類型* `vect` *請參閲* [`net_matrix`](../../MatrixCalculation/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../../MatrixCalculation/net_matrix/cover.md) *for more about type* `vect`

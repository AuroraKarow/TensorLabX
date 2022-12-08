# `supersede`

## 重載 Overload $^1$

```c++
void supersede(const arg &tgt,
               const arg &src,
               uint64_t  fst_rng,
               uint64_t  snd_rng);
```

在給定的索引範圍區間内，將所有與目標值相等的元素替換為指定值。\
In the given range interval of the index, substitute all elements same as target value in list with specified value.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`tgt`|目標值<br>Target value|*輸入<br>input*
`src`|指定值<br>Specified value|*輸入<br>input*
`fst_rng`|範圍區間的第一個索引值<br>First index of the range interval|*輸入<br>input*
`snd_rng`|範圍區間的第二個索引值<br>Second index of the range interval|*輸入<br>input*

## 重載 Overload $^2$

```c++
void supersede(const arg &tgt,
               const arg &src);
```

將表中所有與目標值相等的元素替換為指定值，請參閲[重載$^1$](#重載-overload-1)。
Substitute all elements same as target value in list with specified value, please refer to [Overload$^1$](#重載-overload-1).

## 重載 Overload $^3$

```c++
void supersede(const arg         &tgt,
               const arg         &src, 
               net_set<uint64_t> idx_set);
```

將給定索引序列内對應與目標值相等的表元素替換成指定值。\
Substitute the elements in list corresponding to the given index set same as the target value.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`tgt`|目標值<br>Target value|*輸入<br>input*
`src`|指定值<br>Specified value|*輸入<br>input*
`idx_set`|索引集<br>Index set|*輸入<br>input*

```c++
using std::cout;
using std::endl;
using neunet::net_list;
// 初始化一个列表
// Initialize a list
net_list test_all = {0, 2, 2, 4, 7, 4, 10},
         test_rng = test_all,
         test_idx = test_all;
// 下面的操作是等价的，用 0 代替所有元素 4
// Operations below are equivalent, substitute all element 4 with 0
// 全表
// Whole list
test_all.supersede(4, 0);
// 索引區間 [2, 6]
// Index interval [2, 6]
test_rng.supersede(4, 0, 2, 6);
// 索引集合 {1, 3, 5}
// Index set {1, 3, 5}
test_idx.supersede(4, 0, {1, 3, 5});
```

[<< 返回 Back](cover.md)

---

*更多請參閲* [`sub_list`](sub_list.md)\
*Please refer to* [`sub_list`](sub_list.md) *for more details*

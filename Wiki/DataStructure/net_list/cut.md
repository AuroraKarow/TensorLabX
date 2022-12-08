# `cut`

```c++
bool cut(uint64_t idx, 
         bool     successor = true);
```

切除包括給定索引在内之前或之後的所有表元素。\
Cut out all elements befor or after the specified index in list, including specified index element.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`idx`|目標索引<br>Target index|*輸入<br>input*
`successor`|指定是否是目標索引之後<br>Specify whether after the target index|*輸入<br>input*

切除成功返回`true`，否則為`false`。\
Return `true` for cutting out successfully otherwise `false`.

```c++
using std::cout;
using std::endl;
using neunet::net_list;
// 初始化一个列表
// Initialize a list
net_list odd = {1, 3, 5, 7, 9};
// 切除索引 3 之前的元素
// Cut out elements before index 3
auto odd_cut_status = odd.cut(3 - 1, false); // {7, 9}
// 初始化一个列表
// Initialize a list
net_list even = {0, 2, 4, 6, 8};
// 切除索引 3 之后的元素
// Cut out elements after index 3
auto even_cut_status = even.cut(3 + 1); // {0, 2, 4, 6}
```

[<< 返回 Back](cover.md)

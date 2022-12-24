# `net_set`

## 重載 Overload $^1$

```c++
net_set(initializer_list<arg> init_list);
```

直接使用有限元素初始化集合。\
Initialize set with finite number of element(s) directed.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`init_list`|有限元素列表<br>Finite list of elements|*輸入<br>input*

```c++
using std::cout;
using neunet::net_set;
// 初始化一个元素为{1, 3, 5}的集合
// Initialize a set with element {1, 3, 5}
net_set odd = {1, 3, 5};
// 打印
// Print
for (auto temp : odd) cout << temp << '\n;
```

## 重載 Overload $^2$

```c++
net_set(uint64_t alloc_size = 0);
```

指定集合初始内存大小初始化集合。\
Initialize set with specifying initial memory size of set.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`alloc_size`|初始分配内存大小<br>Initial memory size of allocation|*輸入<br>input*

## 重載 Overload $^3$

```c++
net_set(net_ptr_base<arg> &&src);
```

使用指針結構初始化集合，請參閲屬性[`pointer`](pointer.md) 。/
Initialize set with pointer structure, please refer to property [`pointer`](pointer.md).

[<< 返回 Back](cover.md)

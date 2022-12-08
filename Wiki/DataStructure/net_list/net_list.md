# `net_list`

## 重載 Overload $^1$

```c++
net_list(initializer_list<arg> init_list);
```

直接使用有限元素初始化表。\
Initialize list with finite number of element(s) directed.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`init_list`|有限元素列表<br>Finite list of elements|*輸入<br>input*

```c++
using neunet::net_list;
// 初始化一个元素为{1, 3, 5}的列表
// Initialize a list with element {1, 3, 5}
net_list odd = {1, 3, 5};
// 打印
// Print
for (auto temp : odd) cout << temp << '\n;
```

## 重載 Overload $^2$

```c++
net_list();
```

空構造函數。\
Blank constructor.

[<< 返回 Back](cover.md)

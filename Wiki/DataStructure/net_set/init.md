# `init`

```c++
void init(uint64_t alloc_size, bool remain = true);
```

指定分配的内存長度與是否保留原集合元素，初始化集合。\
Specify the memory length for allocating and whether remain previous set elements to initialize set instance.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`alloc_size`|分配的内存長度<br>Memory length for allocation|*輸入<br>input*
`remain`|指定是否保留集合之前的元素，如果這個參數是`true`，參數`alloc_size`的值小於之前集合的長度那麽集合將會被截斷<br>Specify whether remain previous elements of the set, if this parameter is `true`, the set would be truncated for parameter value of `alloc_size` less than previous length of set|*輸入<br>input*

[<< 返回 Back](cover.md)

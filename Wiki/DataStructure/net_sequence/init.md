# `init`

```c++
void init(uint64_t init_size,
          uint64_t alloc_size = 128,
          bool     remain     = true);
```

指定初始元素長度、分配的内存長度與是否保留原序列元素，初始化序列。\
Specify the initial elements length, memory length for allocating and whether remain previous sequence elements to initialize sequence instance.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`init_size`|Initial elements length<br>初始元素長度|輸入<br>input*
`alloc_size`|分配的内存長度<br>Memory length for allocation|*輸入<br>input*
`remain`|指定是否保留序列之前的元素，如果這個參數是`true`，參數`alloc_size`的值小於之前元素的長度那麽序列將會被截斷<br>Specify whether remain previous elements of the sequence, if this parameter is `true`, the sequence would be truncated for parameter value of `alloc_size` less than previous length of sequence element|*輸入<br>input*

[<< 返回 Back](cover.md)

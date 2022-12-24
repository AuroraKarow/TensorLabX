# `reallocate`

```c++
bool reallocate(uint64_t &id,
                uint64_t alloc_len,
                bool     remain = true);
```

为指定的内存ID的内存进行再分配。\
Reallocate the memory of specified memory ID.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`id`|内存ID<br>Memory ID|*輸入 & 输出<br>input & Output*
`alloc_len`|再分配内存长度<br>Reallocated memory length|*輸入<br>input*
`remain`|指定再分配后是否保留内存元素，如果再分配长度小于原长度且该参数为 `true`，将会执行截断操作<br>Specify whether remain elements of the memory after reallocating, if this parameter is `true` and reallocation length less than current memory, it would execute truncation operation|*輸入<br>input*

如果再分配成功返回 `true` 否则为 `false` 。\
Return `true` for reallocating successfully otherwise `false`.

[<< 返回 Back](cover.md)

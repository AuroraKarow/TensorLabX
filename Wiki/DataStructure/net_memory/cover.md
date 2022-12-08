# `net_memory`

```c++
template <typename arg>
class net_memory;

using namespace neunet;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
`uint64_t length`|占用内存長度<br>Current occupied memory length
`uint64_t memory_length`|實際占用内存長度，包含内存碎片<br>Current real occupied memory length, including memory fragment
`uint64_t memory_length_max`|實際分配内存長度<br>Real allocated memory length

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
[`[]`]([].md)|訪問對應内存ID的内存片段<br>Access memory segment corresponding to the memory ID
`=`|内存分配器實例移動或複製賦值操作<br>Move or copy assignment operation for memory allocator instance.

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
`net_memory`|構造函數，初始化内存緩存長度<br>Constructor, initialize memory buffer length
`print_mem_blk_info`|根據内存ID打印内存片段信息，如果參數 `detail` 是 `true` 值那麽將打印更多詳細信息<br>Print memory segment information according to the memory ID, it could print more details if parameter `detail` is `true`
`print_mem_info`|打印整個内存分配器的信息<br>Print information of the whole memory allocator
`allocate`|分配通過參數 `alloc_size` 指定長度的内存并返回内存ID<br>Allocate specified length of memory by input parameter `alloc_size` and return memory ID
[`reallocate`](reallocate.md)|内存片段再分配<br>Reallocate memory segment
`release`|釋放指定内存ID的内存分配器内存<br>release memory of the specified memory ID in memory allocator
`shrink`|重排内存段并清除内存碎片<br>Rearrange memory segment and clear memory fragment(s)
`id_verify`|验证指定内存ID的内存是否存在，如果内存存在则返回`true`，否则返回`false`。<br>Verify if the memory of specified memory ID exist, return `true` for memory existed otherwise `false`
`reset`|手动释放内存分配器的内存<br>Release the memory of memory allocator manually

```c++

```

[<< 返回 Back](../cover.md)

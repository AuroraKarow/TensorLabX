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
#define PRINT_ENTER printf('\n');

using std::cout;
using std::endl;
using neunet::net_memory;

/* 打印内存片段信息 Print memory segment infomation */

/* 定义一个内存序列并分配三块内存。
 * Define a memory sequence and allocate 3 memory blocks.
 */
net_memory<double> test_0;
auto id_0 = test_0.allocate(6),
     id_1 = test_0.allocate(7),
     id_2 = test_0.allocate(3);
/* 打印此时内存序列的内存占用长度、内存长度以及最大内存长度。
 * Print current size of memory sequence.
 * Covered - 16
 * Memory - 16
 * Capacity - 128
 */
cout << "Covered - " << test_0.length << endl;
cout << "Memory - " << test_0.memory_length << endl;
cout << "Capacity - " << test_0.memory_length_max << endl;
/* 打印 id_1 的内存块信息。
 * Print information of memory block id_1.
 * [ID 1][Length 7]
 */
test_0.print_mem_blk_info(id_1); PRINT_ENTER
/* 打印 id_2 的内存块详细信息。
 * Print detail information of memory block id_1.
 * [ID 2][Length 3][Max 3][Predecessor 1][Successor 0]
 */
test_0.print_mem_blk_info(id_2, true); PRINT_ENTER
/* 由于 id_2 内存已经是最后一块内存，因此下一块内存 ID 为-1. 释放内存。
 * Since id_2 memory block is the last one, the next ID is -1. Release the memory.
 */
test_0.release(id_0);
test_0.release(id_1);
test_0.release(id_2);
/* 打印此时内存序列的内存占用长度、内存长度以及最大内存长度。
 * Print current size of memory sequence.
 * Covered - 0
 * Memory - 16
 * Capacity - 128
 */
cout << "Covered - " << test_0.length << endl;
cout << "Memory - " << test_0.memory_length << endl;
cout << "Capacity - " << test_0.memory_length_max << endl;

/* 打印整个分配器的信息 Print information of whole memory allocator */

/* 定义一个内存序列并分配三块内存。
 * Define a memory sequence and allocate 3 memory blocks.
 */
net_memory<double> test_1;
auto id_0 = test_1.allocate(6),
     id_1 = test_1.allocate(7),
     id_2 = test_1.allocate(3);
/* 释放 id_1 的内存，打印内存序列详细信息。
 * Release memory of id_1 and print memory sequence information in detail.
 * [Block 2][Memory 16][Max 128]
 * [ID 0][Length 6][Max 6][Predecessor 2][Successor 1]
 * [ID 1][Length 0][Max 7][Predecessor 0][Successor 2]
 * [ID 2][Length 3][Max 3][Predecessor 1][Successor 0]
 */
test_1.release(id_1);
test_1.print_mem_info(true); PRINT_ENTER
/* 释放 id_0、id_2 的内存，打印序列的信息。
 * Release memory id_0, id_2. Print memory sequence information.
 * [Null]
 */
test_1.release(id_0);
test_1.release(id_2);
test_1.print_mem_info(); PRINT_ENTER

/* 清理内存碎片 Memory fragments clear */

/* 定义一个内存序列并分配三块内存。
* Define a memory sequence and allocate 3 memory blocks.
*/
net_memory<double> test_2;
auto id_0 = test_2.allocate(6),
     id_1 = test_2.allocate(7),
     id_2 = test_2.allocate(3);
/* 释放 id_1 内存并打印内存结构。
* Release memory id_1 and print memory structure.
* [Block 2][Memory 16][Max 128]
* [ID 0][Length 6][Max 6][Predecessor 2][Successor 1]
* [ID 1][Length 0][Max 7][Predecessor 0][Successor 2]
* [ID 2][Length 3][Max 3][Predecessor 1][Successor 0]
*/
test_2.release(id_1);
test_2.print_mem_info(true); PRINT_ENTER
/* 分配内存长度为 4 的内存块，并打印内存结构。
* Allocate memory block with length 4 and print memory structure.
* [Block 3][Memory 16][Max 128]
* [ID 0][Length 6][Max 6][Predecessor 2][Successor 1]
* [ID 1][Length 4][Max 7][Predecessor 0][Successor 2]
* [ID 2][Length 3][Max 3][Predecessor 1][Successor 0]
*/
id_1 = test_2.allocate(4);
test_2.print_mem_info(true); PRINT_ENTER
/* 清理包括内存块内的内存碎片，打印内存结构。
* Clean memory fragment including memory block and print memory structure.
* [Block 3][Memory 13][Max 128]
* [ID 0][Length 6][Max 6][Predecessor 2][Successor 1]
* [ID 1][Length 4][Max 4][Predecessor 0][Successor 2]
* [ID 2][Length 3][Max 3][Predecessor 1][Successor 0]
*/
test_2.shrink();
test_2.print_mem_info(true); PRINT_ENTER
/* 释放所有内存。
* Release all memory.
*/
test_2.release(id_1);
test_2.release(id_0);
test_2.release(id_2);
```

[<< 返回 Back](../cover.md)

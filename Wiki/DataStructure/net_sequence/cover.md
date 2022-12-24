# `net_sequence`

```c++
template <typename arg>
class net_sequence : public net_set<arg>;

using namespace neunet;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
`memory_length`|最大内存長度<br>Max memory length

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
[`net_sequence`](net_sequence.md)|構造函數<br>Constructor
[`init`](init.md)|初始化序列<br>Initialize sequence instance
`shrink`|將序列占用的内存大小收縮到等於當前元素占用内存的大小<br>Shrink the memory of sequence same as the memory length of current elements occupied

```c++
using std::cout;
using std::endl;
using neunet::net_sequence;
// 初始化一个元素为 int 的序列对象并设定操作长度和分配内存分别为 10 和 128
// Instance a sequence and initialize the element operatable size and memory size with 10 and 128.
net_sequence<int> test(10, 128);
// 以下操作合法，打印内容为：
// Operation below is valid
test[2] = 13;
cout << test[2] << endl;
// 以下操作非法
// Operation below is invalid
// test[13] = 13;
// cout << test[13] << endl;
net_sequence<int> even = {0, 2, 4, 6, 8};
// 此时的可操作长度和内存长度均为 5，打印。
// Current operatable size and memory size are the same value, 5. Print.
cout << even[1] << endl;
even.init(3, 5);
cout << even[1] << endl;
// 此时打印伪随机数
// Print a pseudo random number currently
even.init(3, 8);
cout << even[1] << endl;
// 添加元素并打印。
// Insert elements and print.
test.emplace_back(4);
test.emplace_back(1);
cout << test << endl << endl;
//  在内存收缩操作前后分别打印可操作长度以及内存长度。
//  Print the operatable size and memory length before & after the memory shrinking operation.
cout << "Operatable size - " << test.length << endl;
cout << "Memory size - " << test.memory_length << endl << endl;
test.shrink();
cout << "Operatable size - " << test.length << endl;
cout << "Memory size - " << test.memory_length << endl << endl;
```

[<< 返回 Back](../cover.md)

---

*更多繼承自父類的操作符、函數、屬性以及迭代器，請參閲* [`net_set`](../net_set/cover.md)\
*Please refer to* [`net_set`](../net_set/cover.md) *for more about operators, functions, properties and iterator inherited from parent class*

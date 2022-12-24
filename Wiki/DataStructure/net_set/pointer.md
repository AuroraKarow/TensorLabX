# `pointer`

集合的指针引用。它可以获得所有集合元素的指针拷贝，或者将一个给定的指针结构r_value的所有元素分配给一个集合实例，以替换以前的集合元素。这个指针结构包括一个原始的指针和它的长度，所以它应该在生命周期结束后被手动释放。\
The pointer reference of set instance. It can get pointer copy of all set elements or assign all elements of a given pointer structure r_value to a set instance to replace previous set elements. This pointer structure includes a raw pointer and its length so that it should be release manually after the end of life span.

```c++
using std::cout;
using std::endl;
using neunet::net_set;
// 指針結構
// Pointer structure
using neunet::net_ptr_base;
// 初始化一個集合
// Initialize a set
net_set<int> test = {0, 2, 4, 6, 8};
for (auto temp : test) cout << temp << '\n';
// 獲取指針拷貝
// Get pointer copy
auto p_test = test.pointer;
// 打印指針内容
// Print pointer contents
for (auto i = 0ull; i < p_test.len; ++i) cout << p_test.ptr_base[i] << '\n';
// 釋放指針内存
// Release the memory of pointer
p_test.reset();
// 初始化一个具有3个元素长度的原始指针结构并填充一些元素
// Initialize a raw pointer structure with 3 elements length and fill some elements
net_ptr_base<int> p_inst;
p_inst.init(3);
p_inst.ptr_base[0] = 1;
p_inst.ptr_base[1] = 3;
p_inst.ptr_base[2] = 5;
// 賦值這個指針到集合
// Assign this pointer to set instance
test.pointer = std::move(p_inst);
cout << test << endl;
```

[<< 返回 Back](cover.md)

---

*更多請參閲* `net_set` *的[構造函數](net_set.md)。*\
*Please refer to the [constructor](net_set.md) of* `net_set` *for more details.*

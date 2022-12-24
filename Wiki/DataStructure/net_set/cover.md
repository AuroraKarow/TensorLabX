# `net_set`

```c++
template <typename arg>
class net_set;

using namespace neunet;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
[`net_ptr_base<arg> pointer`](pointer.md)|集合的指針引用<br>Pointer reference of set
`uint64_t length`|集合長度<br>Set length
`arg sum`|所有集合元素的和<br>Sum of whole set elements
`arg product`|所有集合元素的積<br>Product of whole set elements

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`[]`|访问相应索引的集合元素引用，索引值应小于集合长<br>Access corresponding index set element reference, index value should less than set length
`=`|集合實例移動或複製賦值操作<br>Move or copy assignment operation for set instance.
`==`|判定兩個集合是否相同<br>Determine if two sets are same
`!=`|判定兩個集合是否不同<br>Determine if two sets are different

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
[`net_set`](net_set.md)|構造函數<br>Constructor
[`init`](init.md)|初始化集合<br>Initialize set instance
[`insert`](insert.md)|對集合插入一个元素<br>Insert an element into the set
`emplace_back`|插入一个元素到集合尾，更多請參閲 [`insert`](insert.md)<br>Insert an Element into the end of the set, please refer to [`insert`](insert.md) for more details.
`push_back`|生成一个指定元素的副本并插入到集合尾<br>Generate a copy of the specified element and insert into the end of the set
`erase`|擦除集合中指定索引对应的目标元素，索引应小于列集合长度，擦除成功返回`true`，否则返回`false`<br>Erase the target element corresponding to the specified index of the set, index should be less than set length and return `true` for erasing successfully otherwise `false`
[`sub_set`](sub_set.md)|子集<br>Get sub set
`unit`|合集<br>Get united set of two sets
`unit_union`|并集<br>Get union set of two sets
`unit_intersect`|交集<br>Get intersection set of two sets
[`find`](find.md)|寻找集合中的目标元素索引集<br>Find target element index set in set
[`cut`](cut.md)|剔除当前集合的前序或後續的所有元素<br>Cut out all elements in predecessor or successor of current set
`reverse`|颠倒集合中的所有元素<br>Reverse all elements of the set
`sort`|按指定参数对所有集合元素进行升序（`true`）或降序（`false`）排序，如果排序成功则返回`true`，否则返回`false`<br>Sort all set element in ascending(`true`) or descending(`false`) order by specified parameter, it would return `true` for sorting successfully otherwise `false`
`shuffle`|对集合中的所有元素进行洗牌，如果洗牌成功将返回`true`，否则返回`false`<br>Shuffle all elements of the set, it would return `true` for shuffling successfully otherwise `false`
[`supersede`](supersede.md)|用指定的值取代集合中的目标元素<br>Supersede target element of the set by specified value
`fill_with`|用目标值替换集合中的所有元素<br>Replace all elements of the set with target value
`reset`|手动释放当前集合分配的内存<br>Release allocated memory of current set manually

**迭代器** Iterator

```c++
for (auto temp : set_var) {
    // set node element
    temp;
}
```

[<< 返回 Back](../cover.md)

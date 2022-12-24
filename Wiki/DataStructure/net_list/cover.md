# `net_list`

```c++
template <typename arg>
class net_list;

using namespace neunet;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
`uint64_t length`|表長<br>List length
`arg sum`|所有表元素的和<br>All list element(s) sum
`arg product`|所有表元素的積<br>All list element(s) product

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`[]`|访问相应索引的表元素引用，索引值应小于表长<br>Access corresponding index list element reference, index value should less than list length
`=`|表實例移動或複製賦值操作<br>Move or copy assignment operation for list instance.
`==`|判定兩個表是否相同<br>Determine if two lists are same
`!=`|判定兩個表是否不同<br>Determine if two lists are different

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
[`net_list`](net_list.md)|構造函數<br>Constructor
[`insert`](insert.md)|對表插入一个元素<br>Insert an element into the list
`emplace_back`|插入一个元素到表尾，更多請參閲 [`insert`](insert.md)<br>Insert an Element into the end of the list, please refer to [`insert`](insert.md) for more details.
`push_back`|生成一个指定元素的副本并插入到表尾<br>Generate a copy of the specified element and insert into the end of the list
`erase`|擦除表中指定索引对应的目标元素，索引应小于列表长度，擦除成功返回`true`，否则返回`false`<br>Erase the target element corresponding to the specified index of the list, index should be less than list length and return `true` for erasing successfully otherwise `false`
[`sub_list`](sub_list.md)|子集<br>Get sub list
`unit`|合集<br>Get united list of two lists
`unit_union`|并集<br>Get union list of two lists
`unit_intersect`|交集<br>Get intersection list of two lists
[`find`](find.md)|寻找表中的目标元素索引集<br>Find target element index set in list
[`cut`](cut.md)|剔除当前表的前序或後續的所有元素<br>Cut out all elements in predecessor or successor of current list
`reverse`|颠倒表中的所有元素<br>Reverse all elements of the list
`set_output`|以 [`net_set`](../net_set/cover.md) 格式输出当前表<br>Output current list as [`net_set`](../net_set/cover.md) format
`set_input`|输入 [`net_set`](../net_set/cover.md) 集合的所有元素并替换当前表的所有元素<br>Input all elements of a [`net_set`](../net_set/cover.md) set and replace all elements of current list
`sort`|按指定参数对所有表元素进行升序（`true`）或降序（`false`）排序，如果排序成功则返回`true`，否则返回`false`<br>Sort all list element in ascending(`true`) or descending(`false`) order by specified parameter, it would return `true` for sorting successfully otherwise `false`
`shuffle`|对表中的所有元素进行洗牌，如果洗牌成功将返回`true`，否则返回`false`<br>Shuffle all elements of the list, it would return `true` for shuffling successfully otherwise `false`
[`supersede`](supersede.md)|用指定的值取代表中的目标元素<br>Supersede target element of the list by specified value
`reset`|手动释放当前表分配的内存<br>Release allocated memory of current list manually

**迭代器** Iterator

```c++
for (auto temp : list_var) {
    // list node element
    temp;
}
```

[<< 返回 Back](../cover.md)

# `net_map`

```c++
template <typename k_arg, typename arg>
class net_map;

using namespace neunet;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
`uint64_t length`|表長<br>Map length
`uint64_t memory_length`|當前内存最大值<br>Current max memory length

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`[]`|通過表已有鍵訪問對應值的引用<br>Access corresponding value reference via existed key in map
`=`|表實例移動或複製賦值操作<br>Move or copy assignment operation for map instance.
`==`|判定兩個表是否相同<br>Determine if two maps are same
`!=`|判定兩個表是否不同<br>Determine if two maps are different

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
[`net_map`](net_map.md)|構造函數<br>Constructor
`key_set`|獲取表所有鍵的鍵集合<br>Get key set of all existed keys of map
`find_key`|獲取表中與指定值相同的值的所有鍵並返回鍵集合<br>Find all keys whose value is same as the specified value in map and return the key set
[`insert`](insert.md)|插入鍵值對到表<br>Insert key-value element(s) into map
[`erase`](erase.md)|從表擦除鍵值對<br>Erase key-value element(s) from map
`reset`|手動釋放表分配的内存<br>Release map allocated memory manually

[<< 返回 Back](../cover.md)

---

*更多關於動態集合請參閲* [`net_set`](../net_set/cover.md)\
*Please refer to* [`net_set`](../net_set/cover.md) *for more about dynamic set*

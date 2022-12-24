# `net_tree`

```c++
template <typename k_arg, typename arg>
class net_tree;

using namespace neunet;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
`uint64_t length`|樹節點數<br>Amount of the node(s) of tree

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`[]`|通過樹已有鍵訪問對應值的引用<br>Access corresponding value reference via existed key in tree
`=`|樹實例移動或複製賦值操作<br>Move or copy assignment operation for tree instance.
`==`|判定兩個樹是否相同<br>Determine if two trees are same
`!=`|判定兩個樹是否不同<br>Determine if two trees are different

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
`net_tree`|構造函數，指定用於生成哈希編碼的哈希函數與可選的有限鍵值對元素初始化樹，更多請參閲 [`net_map` 的構造函數](../net_map/net_map.md)<br>Constructor, initialize tree with hash key function for hash code generation and optional finite key-value elements, please refer to [`net_map` constructor](../net_map/net_map.md) for more details
`key_set`|獲取樹所有鍵的鍵集合<br>Get key set of all existed keys of tree
`find_key`|獲取樹中與指定值相同的值的所有鍵並返回鍵集合<br>Find all keys whose value is same as the specified value in tree and return the key set
[`insert`](insert.md)|向樹中插入鍵值對元素<br>Insert key-value element(s) into the tree
[`erase`](erase.md)|從樹中擦除鍵值對元素<br>Erase key-value element(s) from the tree
`hash_key_verify`|驗證指定的哈希編碼對應鍵是否是樹中存在的鍵值對的鍵<br>Verify the specified hash code is one of the hash codes of hash keys existed on the tree
`get_value`|指定鍵的哈希編碼，獲取樹中對應鍵值對的值引用<br>Get value reference of the key-value element whose hash key code is same as the specified hash code of parameter input
`reset`|手動釋放樹分配的内存<br>Release tree allocated memory manually

**迭代器** Iterator

```c++
// Iterator utility
for (auto kv_temp : tree_var) {
    // key
    kv_temp.key;
    // value
    kv_temp.value; 
}
```

[<< 返回 Back](../cover.md)

---

*更多請參閲* [`net_map`](../net_map/cover.md)\
*Please refer to* [`net_map`](../net_map/cover.md) *for more details*

*更多關於動態集合請參閲* [`net_set`](../net_set/cover.md)\
*Please refer to* [`net_set`](../net_set/cover.md) *for more about dynamic set*

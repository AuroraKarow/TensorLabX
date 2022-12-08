# `erase`

## 重載 Overload $^1$

```c++
net_set<net_kv<k_arg, arg>> erase(const net_set<k_arg> &key_list);
```

擦除表中與指定鍵集合中鍵相同的鍵值對元素。\
Erase key-value elements of the map whose keys are same as the keys in specified key set.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`key_list`|鍵集合<br>Key set|*輸入<br>input*

返回從表中擦除的鍵值對集合，如果指定鍵集合中存在不合法鍵，那麽返回值可能與預期不符。\
Return the key-value element set erased from map. If there are some illegal keys in key set, the return value might not match to the envisage.

## 重載 Overload $^2$

```c++
net_kv<k_arg, arg> erase(const k_arg &key);
```

擦除表中與指定鍵相同鍵的鍵值對元素。\
Erase key-value element in list which keys are same as the specified key.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`key`|指定鍵<br>Specified key|*輸入<br>input*

返回擦除的键值对，如果键不合法将会空返回。\
Return the erased key-value, it would be blank for illegal key.

```c++
// 初始化一個表
// Initialize a map
net_map<int, string> test = {
    {1, "allen" },
    {2, "kay"   },
    {3, "melody"},
    {4, "cerry" },
    {5, "marin" }};
cout << test << '\n';
// 擦除鍵是3的鍵值對元素
// Erase the key-value element whose key is 3 from map
auto e_elem = test.erase(3);
// 打印
// Print
cout << e_elem.key << " -> " << e_elem.value << '\n';
cout << test << '\n';
// 擦除鍵是4和5的鍵值對元素
// Erase the key-value elements whose keys are respectively 4, 5 from map
auto e_elem_set = test.erase({4, 5});
// 打印
// Print
for (auto temp : e_elem_set) cout << temp.key << " -> " << temp.value << '\n';
cout << test << endl;
```

[<< 返回 Back](cover.md)

---

*更多關於動態集合請參閲* [`net_set`](../net_set/cover.md)\
*Please refer to* [`net_set`](../net_set/cover.md) *for more about dynamic set*

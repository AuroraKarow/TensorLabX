# `insert`

## 重載 Overload $^1$

```c++
uint64_t insert(net_set<net_kv<k_arg, arg>> &&elem_list);
```

使用鍵值對集合批量向表中插入元素。\
Insert element batch into map with key-value set.

根據插入操作的情況返回狀態編碼。\
Return status code according to insert operation.

編碼<br>Code|描述<br>Description
-|-
`NEUNET_INSERT_SUCCESS`|全部指定元素插入成功<br>Insert all specified elements successfully
`NEUNET_INSERT_PARTIAL`|部分指定元素插入成功<br>Insert partial specified elements successfully
`NEUNET_INSERT_FAILED`|插入失敗<br>Insert failed

## 重載 Overload $^2$

```c++
uint64_t insert(init_list_kv_t<k_arg, arg> init_list);
```

使用鍵值對列表插入有限個數元素，更多請參閲[重載$^1$](#重載-overload-1) 和 [構造函數](net_map.md)
Insert finit number of elements with key-value list, please refer to [Overload$^1$](#重載-overload-1) and [Constructor](net_map.md) for more details.

## 重載 Overload $^3$

```c++
bool insert(const k_arg &key,
            const arg   &value);
```

插入一個鍵值對到表。\
Insert a key-value element to map.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`key`|鍵<br>Key|*輸入<br>input*
`value`|鍵的值<br>Value of the key|*輸入<br>input*

插入成功返回 `true` 否則為 `false`。\
Return `true` for inserting successfully otherwise `false`.

```c++
using std::cout;
using std::endl;
using std::string;
using neunet::net_map;
// 初始化一個表
// Initialize a map
net_map<int, string> test;
// 插入鍵值 (0, "blank")
// Insert key-value (0, "blank")
test.insert(0, "blank");
/* 插入以下鍵值對集合
 * Insert key-value sets below
 * {(1, "allen"), (2, "kay"), (3, "melody")}
 * {(4, "cerry"), (5, "marin")}
 */
net_set<net_kv<int, string>> set = {
    {1, "allen" },
    {2, "kay"   },
    {3, "melody"}};
test.insert(std::move(set));
test.insert({{4, "cerry"},
             {5, "marin"}});
for (auto kv_temp : test) cout << kv_temp.key << " -> " << kv_temp.value << '\n';
```

[<< 返回 Back](cover.md)

---

*更多關於動態集合請參閲* [`net_set`](../net_set/cover.md)\
*Please refer to* [`net_set`](../net_set/cover.md) *for more about dynamic set*

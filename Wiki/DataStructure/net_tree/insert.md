# `insert`

## 重載 Overload $^1$

```c++
bool insert(uint64_t           hash_key,
            net_kv<k_arg, arg> &&src);
```

指定哈希編碼對樹插入一個鍵值對元素。\
Insert a key-value element into tree with specified code of hash key.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`hash_key`|哈希編碼<br>Code of hash key|*輸入<br>input*
`src`|鍵值對元素<br>Key-value element|*輸入<br>input*

插入成功返回 `true` 否則為 `false`。\
Return `true` for inserting successfully otherwise `false`.

## 重載 Overload $^2$

```c++
uint64_t insert(net_set<net_kv<k_arg, arg>> &&elem_list);
```

使用鍵值對集合批量向樹中插入元素。\
Insert element batch into tree with key-value set.

根據插入操作的情況返回狀態編碼。\
Return status code according to insert operation.

## 重載 Overload $^3$

```c++
uint64_t insert(init_list_kv_t<k_arg, arg> init_list);
```

使用鍵值對列表插入有限個數元素。\
Insert finit number of elements with key-value list.

## 重載 Overload $^4$

```c++
bool insert(const k_arg &key, const arg &value);
```

插入一個鍵值對到樹。\
Insert a key-value element to tree.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`key`|鍵<br>Key|*輸入<br>input*
`value`|鍵的值<br>Value of the key|*輸入<br>input*

插入成功返回 `true` 否則為 `false`。\
Return `true` for inserting successfully otherwise `false`.

[<< 返回 Back](cover.md)

---

更多請參閲 `net_map` 的 [`insert`](../net_map/insert.md) 函數\
Please refer to function [`insert`](../net_map/insert.md) of `net_map` for more details.

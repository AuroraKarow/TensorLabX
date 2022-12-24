# `erase`

## 重載 Overload $^1$

```c++
net_kv<k_arg, arg> erase(uint64_t hash_key);
```

擦除樹中與指定哈希編碼相同的鍵的鍵值對元素。\
Erase the key-value element whose code of hash key is same as the specified code of hash key in tree.

返回擦除的键值对，如果哈希編碼不合法将会空返回。\
Return the erased key-value, it would be blank for illegal code of hash key.

## 重載 Overload $^2$

```c++
net_set<net_kv<k_arg, arg>> erase(const net_set<k_arg> &k_set);
```

擦除樹中與指定鍵集合中鍵相同的鍵值對元素。\
Erase key-value elements of the tree whose keys are same as the keys in specified key set.

返回從樹中擦除的鍵值對集合，如果指定鍵集合中存在不合法鍵，那麽返回值可能與預期不符。\
Return the key-value element set erased from tree. If there are some illegal keys in key set, the return value might not match to the envisage.

## 重載 Overload $^3$

```c++
net_kv<k_arg, arg> erase(const k_arg &key);
```

擦除樹中與指定鍵相同鍵的鍵值對元素。\
Erase key-value element in tree which keys are same as the specified key.

返回擦除的键值对，如果键不合法将会空返回。\
Return the erased key-value, it would be blank for illegal key.

[<< 返回 Back](cover.md)

---

更多請參閲 `net_map` 的 [`erase`](../net_map/erase.md) 函數\
Please refer to function [`erase`](../net_map/erase.md) of `net_map` for more details.

*更多關於動態集合請參閲* [`net_set`](../net_set/cover.md)\
*Please refer to* [`net_set`](../net_set/cover.md) *for more about dynamic set*

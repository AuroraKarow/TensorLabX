# `net_map`

## 重載 Overload $^1$

```c++
net_map(uint64_t         alloc_size       = 128,
        hash_fn_t<k_arg> hash_func        = hash_in_built,
        uint64_t         rehash_load_base = 4,
        long double      upper_factor     = 0.75,
        long double      lower_factor     = 0.1);
```

指定初始内存緩衝長度與哈希函數以及再哈希參數，初始化表。\
Specify initial memory buffer size, hash function and rehash parameter to initialize map.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`alloc_size`|初始化内存緩衝長度<br>Initial memory buffer size|*輸入<br>input*
`hash_func`|用於產生哈希編碼的哈希鍵函數<br>Hash key function for hash code generation|*輸入<br>input*
`rehash_load_base`|再哈希基礎長度<br>Basic length of rehash|*輸入<br>input*
`upper_factor`|哈希因子上界<br>Upper hash factor|*輸入<br>input*
`lower_factor`|哈希因子下界<br>Lower hash factor|*輸入<br>input*

## 重載 Overload $^2$

```c++
net_map(init_list_kv_t<k_arg, arg> init_list,
        uint64_t                   alloc_size       = 128,
        hash_fn_t<k_arg>           hash_func        = hash_in_built,
        uint64_t                   rehash_load_base = 4,
        long double                upper_factor     = 0.75,
        long double                lower_factor     = 0.1)
```

使用有限鍵值對列表初始化表，更多請參閱[重载$^1$](#重載-overload-1)。\
Initial map with list of finite key-value elements, please refer [Overload$^1$](#重載-overload-1) for more details.

```c++
using std::cout;
using std::endl;
using std::string;
using neunet::net_map;
/* 使用以下鍵值對列表初始化表
 * Initialize a map with key-value
 * ("0000-a0aa0-0000", "Blank")
 * ("0000-a0aa0-0001", "Allen")
 * ("0000-a0aa0-0002", "Flora")
 * ("0000-a0aa0-0003", "Kay"  )
 * 初始化内存緩衝長度為256
 * Initial memory buffer size is 256
 */
net_map<string, string> test({
    {"0000-a0aa0-0000", "Blank"},
    {"0000-a0aa0-0001", "Allen"},
    {"0000-a0aa0-0002", "Flora"},
    {"0000-a0aa0-0003", "Kay"  },
}, 256);
cout << test << endl;
```

[<< 返回 Back](cover.md)

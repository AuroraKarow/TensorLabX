# `num_primes`

```c++
ul_ptr num_primes(uint64_t &len,
                  uint64_t upper);
```

指定上界，生成不大於上界的所有素數。\
Specify the upper, generate all primes not greater than the upper.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`len`|素數數組的長度<br>Length of the prime array|*輸出<br>Output*
`upper`|上界，素數數組元素不會大於這個值<br>Upper, the elements of prime array would not greater than this value|*輸入<br>input*

返回生成的素數數組，如果指定上界小於2，那麽將返回空指針。\
Return generated prime array, it would return null pointer for the specified number less than 2.

[<< 返回 Back](cover.md)

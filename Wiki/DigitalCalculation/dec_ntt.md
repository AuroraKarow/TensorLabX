# `dec_ntt`

```c++
bool dec_ntt(uint64_t *&src,
             uint64_t len,
             bool     inverse = false)
```

快速數論變換。\
Fast number theory transform, NTT.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`src`|多項式係數<br>Coefficient of the polynomial|*輸入 & 输出<br>input & Output*
`len`|係數的長度<br>Length of the coefficient|*輸入<br>input*
`inverse`|指定是否執行快速數論逆變換<br>Specify whether execute inverse NTT|*輸入<br>input*

變換成功返回 `true` 否則為 `false`。\
Return `true` for transforming successfully otherwise `false`.

[<< 返回 Back](cover.md)

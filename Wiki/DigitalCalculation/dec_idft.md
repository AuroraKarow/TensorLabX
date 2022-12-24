# `dec_idft`

```c++
bool dec_idft(uint64_t                  *&d,
              std::complex<long double> *&c,
              uint64_t                  len);
```

離散傅里葉逆變換。注意，多項式 `c` 會變成為變換后的形式。\
Inverse discrete Fourier transform. Warning, the polynomial `c` would be changed the format same as after the IDFT.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`d`|結果多項式的係數<br>Coefficient of the result polynomial|*输出<br>Output*
`c`|需要轉換的多項式<br>Polynomial of changed needed.|*輸入 & 输出<br>input & Output*
`len`|多項式 `d` 和 `c` 的長度<br>Length of the polynomial `d` and `c`|*輸入<br>input*

變換成功返回 `true` 否則為 `false`。\
Return `true` for transforming successfully otherwise `false`.

[<< 返回 Back](cover.md)

# `dec_dft`

```c++
bool dec_dft(complex<long double> *&c,
             complex<long double> *&a,
             complex<long double> *&b,
             uint64_t             len);
```

多項式 `a` 和 `b` 進行離散傅里葉變換並存儲到 `c`，注意多項式 `a` 和 `b` 會變動為快速傅里葉變換后的形式。多項式 `a`，`b` 和 `c` 的長度是相等的。\
Decrete Fourier transform polynomial `a`, `b` and store the result into polynomial `c`. Warning, the elements format of polynomial `a` and `b` would be changed as the format after FFT. The length of polynomial `a`, `b` and `c` are same.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`c`|變換的結果多項式係數<br>Result polynomial coefficient of the transformation|*輸入 & 输出<br>input & Output*
`a`|第一個多項式的係數<br>Coefficient of first polynomial|*輸入 & 输出<br>input & Output*
`b`|第二個多項式的係數<br>Coefficient of second polynomial|*輸入 & 输出<br>input & Output*
`len`|多項式長度<br>Length of the polynomial|*輸入<br>input*

變換成功返回 `true` 否則為 `false`。\
Return `true` for transforming successfully otherwise `false`.

[<< 返回 Back](cover.md)

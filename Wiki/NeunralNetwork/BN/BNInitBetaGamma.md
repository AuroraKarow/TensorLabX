# `BNInitBetaGamma`

```c++
callback_matrix neunet_vect BNInitBetaGamma(uint64_t            iChannCnt,
                                            const matrix_elem_t &iPlaceholder = 0);
```

初始化 $shift$ 或 $scale$ 值。\
Initializing $shift$ or $scale$ value.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`iChannCnt`|通道計數<br>Channel count|*輸入<br>input*
`iPlaceholder`|占位初始化數字<br>Initial number of placeholder|*輸入<br>input*

返回 $shift$ 或 $scale$ 矩陣。\
Return $shift$ or $scale$ matrix.

[<< 返回 Back](cover.md)

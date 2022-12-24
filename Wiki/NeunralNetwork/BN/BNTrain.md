# `BNTrain`

```c++
callback_matrix net_set<neunet_vect> BNTrain (BNData<matrix_elem_t>      &BdData,
                                              const net_set<neunet_vect> &setInput,
                                              const neunet_vect          &vecBeta,
                                              const neunet_vect          &vecGamma,
                                              const matrix_elem_t        &dEpsilon = 1e-8l);
```

BN 前向傳播。\
BN forward propagation.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`BdData`|BN層數據<br>BN layer data|*輸入 & 输出<br>input & Output*
`setInput`|輸入矩陣集合<br>Input matrix set|*輸入<br>input*
`vecBeta`|BN $shift$ 值<br>BN $shift$ value|*輸入<br>input*
`vecGamma`|BN $scale$ 值<br>BN $scale$ value|*輸入<br>input*
`dEpsilon`|分母除 `0`<br>Denominator divisor `0`|*輸入<br>input*

返回 BN 輸出矩陣集合。\
Return output matrix set of BN.

[<< 返回 Back](cover.md)

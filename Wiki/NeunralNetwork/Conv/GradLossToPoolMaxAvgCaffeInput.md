# `GradLossToPoolMaxAvgCaffeInput`

```c++
callback_matrix neunet_vect GradLossToPoolMaxAvgCaffeInput(uint64_t                     iPoolType,
                                                           const neunet_vect            &vecGradLossToOutput,
                                                           uint64_t                     iFilterElemCnt,
                                                           const net_set<net_list<pos>> &setCaffeMaxPos)
```

池化反向傳播。\
Backward propagation of pooling.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`iPoolType`|<br>|*輸入<br>input*
`vecGradLossToOutput`|<br>|*輸入<br>input*
`iFilterElemCnt`|<br>|*輸入<br>input*
`setCaffeMaxPos`|<br>|*輸入<br>input*

返回梯度矩陣。\
Return gradient matrix.

[<< 返回 Back](cover.md)

---

*更多關於矩陣元素坐標結構請參閲* [`pos`](../../MatrixCalculation/pos.md) *。*\
*Please refer to* [`pos`](../../MatrixCalculation/pos.md) *for more detail about position data structure of matrix element.*

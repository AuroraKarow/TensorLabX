# `GradLossToPoolAvgChann`

```c++
callback_matrix neunet_vect GradLossToPoolAvgChann(const neunet_vect       &vecGradLossToOutput,
                                                   const net_set<uint64_t> &setCaffeData,
                                                   uint64_t                iFilterElemCnt,
                                                   uint64_t                iChannElemCnt);
```

平均池化反向傳播。\
Backward propagation of average pooling.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecGradLossToOutput`|回傳梯度<br>Gradient of backward propagation|*輸入<br>Output*
`setCaffeData`|Caffe 矩陣數據<br>Caffe matrix data|*輸入<br>input*
`iFilterElemCnt`|下采樣過濾器元素計數<br>Element count of down sampling filter|*輸入<br>input*
`iChannElemCnt`|輸入通道元素個數<br>Element count of input channel|*輸入<br>input*

返回回傳梯度。\
Return gradient of backward propagation.

[<< 返回 Back](cover.md)

---

*更多關於平均池化請參閲* [`PoolAvg`](PoolAvg.md) *。*\
*Please refer to* [`PoolAvg`](PoolAvg.md) *for more details about average pooling.*

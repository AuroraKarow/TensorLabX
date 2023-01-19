# `GradLossToPoolMaxChann`

```c++
callback_matrix neunet_vect GradLossToPoolMaxChann(const neunet_vect                 &vecGradLossToOutput,
                                                   uint64_t                          iChannElemCnt,
                                                   const net_set<net_list<uint64_t>> &setElemIdx);
```

最大池化反向傳播。\
Backward propagation of max pooling.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecGradLossToOutput`|回傳梯度<br>Gradient of backward propagation|*輸入<br>Output*
`iChannElemCnt`|輸入通道元素個數<br>Element count of input channel|*輸入<br>input*
`setElemIdx`|最大池化矩陣元素指針索引<br>Matrix element pointer index of max pooling|*輸入<br>input*

返回回傳梯度。\
Return gradient of backward propagation.

[<< 返回 Back](cover.md)

---

*更多關於最大池化請參閲* [`PoolMax`](PoolMax.md) *。*\
*Please refer to* [`PoolMax`](PoolMax.md) *for more details about max pooling.*

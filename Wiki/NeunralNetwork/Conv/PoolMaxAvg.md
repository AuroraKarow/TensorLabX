# `PoolMaxAvg`

```c++
callback_matrix neunet_vect PoolMaxAvg(uint64_t               iPoolType,
                                       const neunet_vect      &vecCaffeInput,
                                       uint64_t               iChannCnt,
                                       uint64_t               iFilterElemCnt,
                                       net_set<net_list<pos>> &setCaffeMaxPos)
```

最大或平均池化。\
Max or average pooling.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`iPoolType`|池化類型編碼<br>Pooling type code|*輸入<br>input*
`vecCaffeInput`|輸入通道的 Caffe 矩陣<br>Caffe matrix of the input channel|*輸入<br>input*
`iChannCnt`|通道計數<br>Channel count|*輸入<br>input*
`iFilterElemCnt`|單位采樣元素個數<br>Element count of a down sampling unit|*輸入<br>input*
`setCaffeMaxPos`|最大池化 Caffe 矩陣元素坐標集合，平均池化時可以為將亡值<br>Caffe matrix element position set of max pooling, it could be dying value for average pooling|*輸入<br>input*

池化類型編碼，由於全局平均池化有特定函數，因此只需要選擇最大或是平均池化即可。\
Pooling code. Since there is a specific function of global pooling, it just need to select in max or average pooling.

編碼<br>Code|描述<br>Description
-|-
`NEUNET_POOL_MAX`|最大池化<br>Max pooling
`NEUNET_POOL_AVG`|平均池化<br>Average pooling
`NEUNET_POOL_GAG`|全局平均池化<br>Global average pooling

返回采樣輸出矩陣。\
Return down sampling output matrix.

[<< 返回 Back](cover.md)

---

*更多關於矩陣元素坐標結構請參閲* [`pos`](../../MatrixCalculation/pos.md) *。*\
*Please refer to* [`pos`](../../MatrixCalculation/pos.md) *for more detail about position data structure of matrix element.*

*更多關於 Caffe 矩陣請參閲* [`CaffeTransform`](CaffeTransform.md) *。*\
*Please refer to* [`CaffeTransform`](CaffeTransform.md) *for more detail about the Caffe matrix.*

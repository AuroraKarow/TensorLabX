# `PoolAvg`

```c++
callback_matrix neunet_vect PoolAvg(const neunet_vect       &vecChann,
                                    const net_set<uint64_t> &setCaffeData,
                                    uint64_t                iFilterElemCnt,
                                    uint64_t                iCaffeLnCnt);
```

平均池化。\
Average pooling

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecChann`|輸入矩陣<br>Input matrix|*輸入<br>input*
`setCaffeData`|Caffe 矩陣數據<br>Caffe matrix data|*輸入<br>input*
`iFilterElemCnt`|下采樣過濾器元素計數<br>Element count of down sampling filter|*輸入<br>input*
`iCaffeLnCnt`|Caffe 矩陣行計數<br>Line count of Caffe matrix|*輸入<br>input*

返回下采樣矩陣。\
Return down sampling matrix

[<< 返回 Back](cover.md)

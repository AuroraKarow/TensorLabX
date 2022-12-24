# `CaffeTransformData`

```c++
net_set<uint64_t> CaffeTransformData(uint64_t iChannCnt,
                                     uint64_t &iCaffeLnCnt,
                                     uint64_t &iCaffeColCnt,
                                     uint64_t iChannLnCnt,
                                     uint64_t iChannColCnt,
                                     uint64_t &iSampChannLnCnt,
                                     uint64_t &iSampChannColCnt,
                                     uint64_t iFilterLnCnt,
                                     uint64_t iFilterColCnt,
                                     uint64_t iLnStride,
                                     uint64_t iColStride,
                                     uint64_t iLnDilate,
                                     uint64_t iColDilate);
```

獲取 Caffe 矩陣轉換數據，這個數據記錄了 Caffe 矩陣到 Im2Col 矩陣張量的元素坐標映射。\
Get Caffe matrix transformation data. This data is a record of the element position in matrix mapped from Caffe matrix to Im2Col matrix tensor.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`iChannCnt`|通道數<br>Channel count|*輸入<br>input*
`iCaffeLnCnt`|Caffe 矩陣行計數<br>Line count of Caffe matrix|*输出<br>Output*
`iCaffeColCnt`|Caffe 矩陣列計數<br>Column count of Caffe matrix|*输出<br>Output*
`iChannLnCnt`|輸入通道矩陣行計數<br>Line count of input channel matrix|*輸入<br>input*
`iChannColCnt`|輸入通道矩陣列計數<br>Column count of input channel matrix|*輸入<br>input*
`iSampChannLnCnt`|下采樣輸出通道矩陣行計數<br>Line count of down sampling output channel matrix|*输出<br>Output*
`iSampChannColCnt`|下采樣輸出通道矩陣列計數<br>Column count of down sampling output channel matrix|*输出<br>Output*
`iFilterLnCnt`|采樣向量行計數<br>Line count of the sampling filter|*輸入<br>input*
`iFilterColCnt`|采樣向量列計數<br>Column count of the sampling filter|*輸入<br>input*
`iLnStride`|行向步幅<br>Stride of line direction|*輸入<br>input*
`iColStride`|列向步幅<br>Stride of column direction|*輸入<br>input*
`iLnDilate`|輸入通道采樣行間擴張值<br>Filter sampling dilation among input channel lines|*輸入<br>input*
`iColDilate`|輸入通道采樣列間擴張值<br>Filter sampling dilation among input channel columns|*輸入<br>input*

返回隱射坐標集合，用於 Caffe 矩陣轉換。\
Return mapping position set for Caffe matrix transformation.

[<< 返回 Back](cover.md)

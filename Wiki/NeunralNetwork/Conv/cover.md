# 卷積運算 Convolution calculation

捲積層基礎函數。\
Basic function of the convolution layer.

```c++
using namespace neunet::conv;
```

函數<br>Function|描述<br>Description
-|-
[`CaffeTransformData`](CaffeTransformData.md)|獲取 Caffe 矩陣轉換數據<br>Get Caffe matrix transformation data
[`CaffeTransform`](CaffeTransform.md)|Caffe 轉換<br>Caffe transformation
[`InitKernel`](InitKernel.md)|初始化捲積核<br>Convolution kernel initialization
[`Conv`](Conv.md)|捲積前向傳播<br>Forward propagation of convolution
[`GradLossToConvCaffeInput`](GradLossToConvCaffeInput.md)|捲積反向傳播<br>Backward propagation of convolution
[`GradLossToConvKernal`](GradLossToConvKernal.md)|捲積核更新梯度<br>Gradient for convolution kernel updating
`PoolGlbAvg`|全局平均池化<br>Global average pooling
`GradLossToPoolGlbAvgChann`|全局平均池化反向傳播，需要指定損失到全局池化輸出的梯度 `vecGradLossToOutput` 以及輸入矩陣的行 `iChannLnCnt`、列 `iChannColCnt` 計數值<br>Backpropagation of the global average pooling, need to specify the gradient from loss to global pooling output matrix `vecGradLossToOutput`, value of input matrix line count `iChannLnCnt` and column count `iChannColCnt`.
[`PoolMaxAvg`](PoolMaxAvg.md)|最大或平均池化前向傳播<br>Forward porpagation of max or average pooling
[`GradLossToPoolMaxAvgCaffeInput`](GradLossToPoolMaxAvgCaffeInput.md)|最大或平均池化反向傳播<br>Backward porpagation of max or average pooling

[<< 返回 Back](../cover.md)

# 卷積運算 Convolution calculation

捲積層基礎函數。\
Basic function of the convolution layer.

```c++
using namespace neunet::conv;
```

$$ Conv(X_O,K_O) \implies X_O*K_O=Z_O $$
$$ X_O*K_O \backsim Caffe(X)K=Z $$
$$ \frac{dL}{dCaffe(x)}=\frac{dL}{dZ}K^T $$
$$ \begin{align*}
    \frac{dL}{dX}&=\frac{dL}{dCaffe(x)}Caffe'(x)\\
    &=deCaffe(\frac{dL}{dCaffe(x)})\\
    &=deCaffe(\frac{dL}{dZ}K^T)
\end{align*} $$
$$ \frac{dL}{dK}=Caffe(X)^T\frac{dL}{dZ} $$

在反向傳播時，相同位置的 Caffe 矩陣梯度需要相加。\
The Caffe element gradients at same position of Im2Col matrix are need to sum in backward propagation.

全局平均池化\
Global average pooling

$$Pool_{Gavg}(X)=Z$$

平均池化\
Average pooling

$$Z_O=Pool_{Avg}^O(X_O) \backsim Pool_{Avg}(Caffe(X))=Z$$

最大池化\
Max pooling

$$Z_O=Pool_{Max}^O(X_O) \backsim Pool_{Max}(Caffe(X))=Z$$

均值梯度\
Gradient of average value

$$z=Avg(x)=\frac{\sum_{i=0}^nx_i}{n}$$
$$\frac{\partial L}{\partial x_i}=\frac{\partial L}{\partial z}Avg'(x)=\frac{1}{n}\left(\frac{\partial L}{\partial z}\right)$$

最大值梯度\
Gradient of max value

$$z=Max(x)=Max\{x_1,x_2,\dots,x_i,\dots,x_n\}$$
$$\frac{\partial L}{\partial x_i}=\frac{\partial L}{\partial z}Max'(x)=\begin{cases}
    \frac{\partial L}{\partial z} & ,z=x_i \\
    0 & ,z\neq x_i
\end{cases}$$

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
[`PoolMax`](PoolMax.md)|最大池化<br>Max pooling
[`GradLossToPoolMaxChann`](GradLossToPoolMaxChann.md)|最大池化反向傳播<br>Backward propagation of max pooling
[`PoolAvg`](PoolAvg.md)|平均池化<br>Average pooling
[`GradLossToPoolAvgChann`](GradLossToPoolAvgChann.md)|平均池化反向傳播<br>Backward propagation of average pooling

[<< 返回 Back](../cover.md)

# `GradLossToConvCaffeInput`

```c++
callback_matrix neunet_vect GradLossToConvCaffeInput(const neunet_vect &vecGradLossToOutput,
                                                     const neunet_vect &vecKernelTranspose);
```

獲取損失到捲積輸入 Caffe 矩陣的梯度。\
Get gradient from loss to Caffe matrix of the input.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecGradLossToOutput`|損失到捲積輸出矩陣的梯度<br>Gradient from loss to the output of convolution|*輸入<br>input*
`vecKernelTranspose`|捲積核矩陣的轉置<br>Transposition of the kernel matrix|*輸入<br>input*

返回 Caffe 矩陣梯度。\
Return gradient of Caffe matrix.

[<< 返回 Back](cover.md)

---

*更多關於池化類型請參閲* [PoolMaxAvg](PoolMaxAvg.md) *。*\
*Please refer to* [PoolMaxAvg](PoolMaxAvg.md) *for more details about pooling type.*

*更多關於 Caffe 矩陣逆轉換請參閲* [CaffeTransform](CaffeTransform.md) *。*\
*Please refer to* [CaffeTransform](CaffeTransform.md) *for more details about inverse Caffe matrix transformation.*

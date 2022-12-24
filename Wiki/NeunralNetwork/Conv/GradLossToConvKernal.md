# `GradLossToConvKernal`

```c++
callback_matrix neunet_vect GradLossToConvKernal(const neunet_vect &vecGradLossToOutput,
                                                 const neunet_vect &vecCaffeInputTranspose);
```

損失到捲積核的梯度，用於更新捲積核矩陣。\
Gradient from loss to kernel for kernel matrix updating.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecGradLossToOutput`|損失到捲積輸出矩陣的梯度<br>Gradient from loss to the output of convolution|*輸入<br>input*
`vecCaffeInputTranspose`|輸入通道 Caffe 矩陣的轉置<br>Transposition of input channel Caffe matrix|*輸入<br>input*

返回梯度矩陣。\
Return gradient matrix.

[<< 返回 Back](cover.md)

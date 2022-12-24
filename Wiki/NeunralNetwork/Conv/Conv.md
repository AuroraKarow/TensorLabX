# `Conv`

```c++
callback_matrix neunet_vect Conv(const neunet_vect &vecCaffeInput,
                                 const neunet_vect &vecKernelChann);
```

捲積。\
Convolution.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecCaffeInput`|輸入通道的 Caffe 矩陣<br>Caffe matrix of the input channel|*輸入<br>input*
`vecKernelChann`|捲積核矩陣，更多請參閲 [`InitKernel`](InitKernel.md)<br>Convolution kernel matrix, please refer to [`InitKernel`](InitKernel.md) for more details|*輸入<br>input*

返回捲積輸出。\
Return convolution output.

[<< 返回 Back](cover.md)

---

*更多關於 Caffe 矩陣請參閲* [`CaffeTransform`](CaffeTransform.md) *。*\
*Please refer to* [`CaffeTransform`](CaffeTransform.md) *for more detail about the Caffe matrix.*

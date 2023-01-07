# `GradLossToInput`

```c++
callback_matrix neunet_vect GradLossToInput(const neunet_vect &vecGradLossToOutput,
                                            const neunet_vect &vecWeightTranspose)
```

反向傳播，獲取損失到當前網絡層輸入向量的梯度。\
Backward propagation, get the gradient from loss to input vector of current layer.

$$\frac{dL}{dX}=W^T\frac{dL}{dZ}$$

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecGradLossToOutput`|損失到當前網絡層輸出向量的梯度<br>Gradient from loss to output vector vector of current layer|*輸入<br>input*
`vecWeightTranspose`|權重矩陣的轉置<br>Transposition of the weight matrix|*輸入<br>input*

返回反向傳播梯度。\
Return backward propagation gradient.

[<< 返回 Back](cover.md)

# `GradLossToWeight`

```c++
callback_matrix neunet_vect GradLossToWeight(const neunet_vect &vecGradLossToOutput,
                                             const neunet_vect &vecInputTranspose);
```

獲取損失到權重的梯度，用於更新權重。\
Get gradient from loss to weight for weight updating.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecGradLossToOutput`|損失到當前網絡層輸出向量的梯度<br>Gradient from loss to output vector vector of current layer|*輸入<br>input*
`vecInputTranspose`|網絡層輸入向量矩陣的轉置<br>Transposition of the layer input vector|*輸入<br>input*

返回梯度向量。\
Return gradient vector.

[<< 返回 Back](cover.md)

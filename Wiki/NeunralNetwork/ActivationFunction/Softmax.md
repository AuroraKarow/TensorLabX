# $Softmax$

```c++
callback_matrix neunet_vect softmax(const neunet_vect &src);
```

$$Softmax(x_i)=\frac{e^{x_i}}{\Sigma_j{e^{x_j}}}$$

函數 $Softmax$，參數為矩陣，返回矩陣格式的激活神經元。\
Function $ReLU$, parameter is matrix, return activated neurons in matrix format.

```c++
allback_matrix neunet_vect softmax_cec_grad(const neunet_vect &softmax_output,
                                            const neunet_vect &origin)
```

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`softmax_output`|$Softmax$ 輸入矩陣<br>$Softmax$ input matrix|*輸入<br>input*
`origin`|神經網絡輸出標簽矩陣<br>ANN output label matrix|*輸入<br>input*

當作爲輸出層的激活函數時，這個函數將獲得損失到 $Softmax$ 輸入的梯度。
This function would get the vector of gradient from loss to $Softmax$ input as last layer activation function.

[<< 返回 Back](cover.md)

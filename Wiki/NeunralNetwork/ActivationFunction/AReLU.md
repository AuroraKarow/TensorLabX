# $AReLU$

```c++
callback_matrix neunet_vect AReLU(const neunet_vect &src);

callback_matrix neunet_vect AReLU_dv(const neunet_vect &src);
```

$$AReLU(x)=\begin{cases}
    0, & x\le1\\
    x, & x>1
\end{cases},\space x\ge0$$

函數 $AReLU$ 和導函數，即非負 $ReLU$，參數為矩陣，返回矩陣格式的激活神經元\
Function $AReLU$ and derivative function, non-negative $ReLU$ function, parameter is matrix, return activated neurons in matrix format.

```c++
callback_matrix neunet_vect AReLU_loss_grad(const neunet_vect &AReLU_input,
                                            const neunet_vect &AReLU_output,
                                            const neunet_vect &origin);
```

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`AReLU_input`|$AReLU$ 輸入矩陣<br>$AReLU$ input matrix|*輸入<br>input*
`AReLU_output`|$AReLU$ 輸出矩陣<br>$AReLU$ output matrix|*輸入<br>input*
`origin`|神經網絡輸出標簽矩陣<br>ANN output label matrix|*輸入<br>input*

當作爲輸出層的激活函數時，這個函數將獲得損失到 $AReLU$ 輸入的梯度。\
This function would get the vector of gradient from loss to $AReLU$ input as last layer activation function.

[<< 返回 Back](cover.md)

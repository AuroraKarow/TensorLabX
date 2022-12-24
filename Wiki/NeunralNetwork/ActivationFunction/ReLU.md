# $ReLU$

```c++
callback_matrix neunet_vect ReLU(const neunet_vect &src);

callback_matrix neunet_vect ReLU_dv(const neunet_vect &src);
```

$$ReLU(x)=\begin{cases}
    0, & x\le0\\
    x, & x>0
\end{cases}$$

函數 $ReLU$ 和導函數，參數為矩陣，返回矩陣格式的激活神經元。\
Function $ReLU$ and derivative function, parameter is matrix, return activated neurons in matrix format.

[<< 返回 Back](cover.md)

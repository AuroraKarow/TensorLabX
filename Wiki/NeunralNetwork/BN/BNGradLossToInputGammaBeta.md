# `BNGradLossToInputGammaBeta`

```c++
callback_matrix net_set<neunet_vect> BNGradLossToInputGammaBeta(BNData<matrix_elem_t> &BdData,
                                                                neunet_vect           &vecGradGamma,
                                                                neunet_vect           &vecGradBeta,
                                                                net_set<neunet_vect>  &setGradLossToOutput,
                                                                const neunet_vect     &vecGamma,
                                                                long double           dDecay = .9l);
```

BN 反向傳播並獲取用於更新 $shift$ 與 $scale$ 的梯度。\
BN backward propagagtion and getting the gradient for $shift$ and $scale$ updating.

$$\begin{align*}
    \frac{\partial L}{\partial \hat{X_i}}&=\gamma\otimes\frac{\partial L}{\partial Y_i}\\
    \frac{\partial L}{\partial \sigma_\chi^2}&=-\frac{1}{2}V \odot V^2\odot\sum_{i=1}^m\frac{\partial L}{\partial \hat{X_i}}\odot D_i\\
    \frac{\partial L}{\partial \mu_\chi}&=-V\odot\sum_{i=1}^m\frac{\partial L}{\partial \hat{X_i}}-\frac{2}{m}\frac{\partial L}{\partial \sigma_\chi^2}\odot\sum_{i=1}^mD_i\\
    \frac{\partial L}{\partial X_i}&=\frac{\partial L}{\partial \hat{X_i}}\odot V+\frac{\partial L}{\partial \sigma_\chi^2}\odot\frac{2}{m}D_i+\frac{1}{m}\frac{\partial L}{\partial \mu_\chi}\\
    \frac{\partial L}{\partial\gamma}&=\hat{X_i}\odot\sum_{i=1}^m\frac{\partial L}{\partial Y_i}\\
    \frac{\partial L}{\partial\beta}&=\sum_{i=1}^m\frac{\partial L}{\partial Y_i}
\end{align*}$$

$\gamma$ 和 $\beta$ 的梯度需要對每一個通道進行求和。\
The gradient of $\gamma$ and $\beta$ need to get sum of each channel.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`BdData`|BN層數據<br>BN layer data|*輸入<br>input*
`vecGradGamma`|損失到 $scale$ 的梯度向量，用於更新<br>Vector of gradient from loss to $scale$ for updating|*输出<br>Output*
`vecGradBeta`|損失到 $shift$ 的梯度向量，用於更新<br>Vector of gradient from loss to $shift$ for updating|*输出<br>Output*
`setGradLossToOutput`|損失到 BN 輸出的梯度矩陣，這個值得内存將在計算后被回收<br>Matrix of gradient from loss to BN output, the allocated memory of this value would be collected after calculation|*輸入 & 输出<br>input & Output*
`vecGamma`|BN $scale$ 值<br>BN $scale$ value|*輸入<br>input*
`dDecay`|衰退率<br>Decay rate|*輸入<br>input*

返回損失到 BN 輸入的梯度矩陣。\
Return matrix of the gradient from loss to BN input.

[<< 返回 Back](cover.md)

___

*更多請參閲* [`BNTrain`](BNTrain.md) *。*\
*Please refer to*  [`BNTrain`](BNTrain.md) *for more details.*

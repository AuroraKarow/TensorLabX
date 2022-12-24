# `BNGradLossToInputGammaBeta`

```c++
callback_matrix net_set<neunet_vect> BNGradLossToInputGammaBeta(neunet_vect                 &vecGradGamma,
                                                                neunet_vect                 &vecGradBeta,
                                                                net_set<neunet_vect>        &setGradLossToOutput,
                                                                const BNData<matrix_elem_t> &BdData,
                                                                const neunet_vect           &vecGamma,
                                                                const matrix_elem_t         &dEpsilon = 1e-8l);
```

BN 反向傳播並獲取用於更新 $shift$ 與 $scale$ 的梯度。\
BN backward propagagtion and getting the gradient for $shift$ and $scale$ updating.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecGradGamma`|損失到 $scale$ 的梯度向量，用於更新<br>Vector of gradient from loss to $scale$ for updating|*输出<br>Output*
`vecGradBeta`|損失到 $shift$ 的梯度向量，用於更新<br>Vector of gradient from loss to $shift$ for updating|*输出<br>Output*
`setGradLossToOutput`|損失到 BN 輸出的梯度矩陣，這個值得内存將在計算后被回收<br>Matrix of gradient from loss to BN output, the allocated memory of this value would be collected after calculation|*輸入 & 输出<br>input & Output*
`BdData`|BN層數據<br>BN layer data|*輸入<br>input*
`vecGamma`|BN $scale$ 值<br>BN $scale$ value|*輸入<br>input*
`dEpsilon`|分母除 `0`<br>Denominator divisor `0`|*輸入<br>input*

返回損失到 BN 輸入的梯度矩陣。\
Return matrix of the gradient from loss to BN input.

[<< 返回 Back](cover.md)

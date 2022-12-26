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

# `BNDeduce`

```c++
callback_matrix neunet_vect BNDeduce(const BNData<matrix_elem_t> &BdData,
                                     const neunet_vect           &vecInput,
                                     const neunet_vect           &vecBeta,
                                     const neunet_vect           &vecGamma);

```

BN 推測前向傳播。\
BN forward propagation of deducing.

在推測過程中使用的期望和方差需要使用滑動均值。\
In the deducing process, it need moving average value of the expectation and variance.

$$\begin{align*}
    E[X]_t&=\rho E[X]_{t-1}+(1-\rho){\mu_\chi}_t\\
    Var[X]_t&=\rho Var[X]_{t-1}+(1-\rho){\sigma_\chi^2}_t\\
    \hat{X}&=(X-E[X])\odot Var[V]\\
    Y&=\beta\oplus(\gamma\otimes\hat{X})
\end{align*}$$

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`BdData`|BN層數據<br>BN layer data|*輸入 & 输出<br>input & Output*
`setInput`|輸入矩陣<br>Input matrix|*輸入<br>input*
`vecBeta`|BN $shift$ 值<br>BN $shift$ value|*輸入<br>input*
`vecGamma`|BN $scale$ 值<br>BN $scale$ value|*輸入<br>input*

返回 BN 推測輸出矩陣。\
Return output matrix of BN deducing.

[<< 返回 Back](cover.md)

---

*更多請參閲* [`BNTrain`](BNTrain.md) *。*\
*Please refer to*  [`BNTrain`](BNTrain.md) *for more details.*

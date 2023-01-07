# `BNTrain`

```c++
callback_matrix net_set<neunet_vect> BNTrain (BNData<matrix_elem_t>      &BdData,
                                              const net_set<neunet_vect> &setInput,
                                              const neunet_vect          &vecBeta,
                                              const neunet_vect          &vecGamma);

```

BN 前向傳播。\
BN forward propagation.

$$\begin{align*}
\chi&=\{X_1,X_2,\dots,X_i,\dots,X_m\}\\
\mu_\chi&=\frac{1}{m}\sum\chi\\
D_i&=X_i-\mu_\chi\\
\sigma_\chi^2&=\frac{1}{m}\sum_{i=1}^m\left(D_i \odot D_i\right)\\
V \Rightarrow v_{ij}&=\frac{1}{\sqrt{(\sigma_\chi^2)_{ij}+\epsilon}}\\
V^2 \Rightarrow v_{ij}^2&=\frac{1}{(\sigma_\chi^2)_{ij}+\epsilon}\\
\hat{X_i}&=V\odot\left(X_i-\mu_\chi\right)\\
A &= \begin{pmatrix}a_0&a_1&\dots&a_i&\dots&a_n\end{pmatrix}\\
B &= \begin{pmatrix}
     b_{00}&b_{01}&\cdots&b_{0i}&\cdots&b_{0n}\\
     \vdots&\vdots&\ddots&\vdots&\ddots&\vdots\\
     b_{m0}&b_{m1}&\cdots&b_{mi}&\cdots&b_{mn}
     \end{pmatrix} \\
A \otimes B &= \begin{pmatrix}
                    a_0b_{00}&a_1b_{01}&\cdots&a_ib_{0i}&\cdots&a_nb_{0n}\\
                    \vdots&\vdots&\ddots&\vdots&\ddots&\vdots\\
                    a_0b_{m0}&a_1b_{m1}&\cdots&a_ib_{mi}&\cdots&a_nb_{mn}
                \end{pmatrix}\\
A \oplus  B &= \begin{pmatrix}
                    a_0+b_{00}&a_1+b_{01}&\cdots&a_i+b_{0i}&\cdots&a_n+b_{0n}\\
                    \vdots&\vdots&\ddots&\vdots&\ddots&\vdots\\
                    a_0+b_{m0}&a_1+b_{m1}&\cdots&a_i+b_{mi}&\cdots&a_n+b_{mn}
               \end{pmatrix}\\
Y_i&=\beta\oplus\left(\gamma\otimes\hat{X_i}\right)
\end{align*}$$

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`BdData`|BN層數據<br>BN layer data|*輸入 & 输出<br>input & Output*
`setInput`|輸入矩陣集合<br>Input matrix set|*輸入<br>input*
`vecBeta`|BN $shift$ 值<br>BN $shift$ value|*輸入<br>input*
`vecGamma`|BN $scale$ 值<br>BN $scale$ value|*輸入<br>input*

返回 BN 輸出矩陣集合。\
Return output matrix set of BN.

[<< 返回 Back](cover.md)

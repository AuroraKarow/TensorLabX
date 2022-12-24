# `CaffeTransform`

## 重載 Overload $^1$

```c++
callback_matrix neunet_vect CaffeTransform(const neunet_vect       &vecChann,
                                           const net_set<uint64_t> &setCaffeData,
                                           uint64_t                iCaffeLnCnt,
                                           uint64_t                iCaffeColCnt);
```

將 Im2Col 矩陣張量轉換為 Caffe 矩陣，用於捲積與池化計算。\
Transform Im2Col matrix tensor to Caffe matrix for convolution and pooling calculaation.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecChann`|通道矩陣<br>Channel matrix|*輸入<br>input*
`setCaffeData`|Caffe 矩陣數據<br>Caffe matrix data|*輸入<br>input*
`iCaffeLnCnt`|Caffe 矩陣的行計數<br>Line count of Caffe matrix|*輸入<br>input*
`iCaffeColCnt`|Caffe 矩陣的列計數<br>Column count of Caffe matrix|*輸入<br>input*

返回 Caffe 矩陣。\
Return Caffe matrix.

## 重載 Overload $^2$

```c++
callback_matrix neunet_vect CaffeTransform(const neunet_vect       &vecCaffe,
                                           const net_set<uint64_t> &setCaffeData,
                                           uint64_t                iIm2ColLnCnt,
                                           uint64_t                iIm2ColColCnt,
                                           bool                    bIsGrad);
```

通過 Caffe 矩陣數據獲取 Caffe 矩陣的 Im2Col矩陣。\
Get Im2Col matrix of Caffe matrix by Caffe matrix data.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecCaffe`|Caffe 矩陣<br>Caffe matrix|*輸入<br>input*
`setCaffeData`|Caffe 矩陣數據<br>Caffe matrix data|*輸入<br>input*
`iIm2ColLnCnt`|Im2Col 矩陣的行計數<br>Line count of Im2Col matrix|*輸入<br>input*
`iIm2ColColCnt`|Im2Col 矩陣的列計數<br>Column count of Im2Col matrix|*輸入<br>input*
`bIsGrad`|如果為 `true`，Im2Col 矩陣同一個位置的多個 Caffe 矩陣元素映射將會求和<br>Get sum of multiple Caffe matrix elements mapping to a same position in Im2Col matrix, if this parameter value is `true`|*輸入<br>input*

返回 Im2Col 矩陣。\
Return Im2Col matrix.

給定通道張量 $A$，尺寸為 $2^2$ 的捲積核張量 $B$ 的數量為 $2$，行列向步幅為 $1$，行列間擴張值為 $0$，求 $A$ 的 Caffe 矩陣 $C$。\
Given channel tensor $A$, the amount of kernel tensor $B$ with shape of $2^2$ is $2$, Stride of line and column direction is $1$, dilation between lines and columns is $0$, find Caffe matrix $C$ of $A$.

$$
A=\begin{bmatrix} \begin{pmatrix}
    1 & 2 & 0 \\
    1 & 1 & 3 \\
    0 & 2 & 2
\end{pmatrix} & \begin{pmatrix}
    0 & 2 & 1 \\
    0 & 3 & 2 \\
    1 & 1 & 0
\end{pmatrix} & \begin{pmatrix}
    1 & 2 & 1 \\
    0 & 1 & 3 \\
    3 & 3 & 2
\end{pmatrix} \end{bmatrix}
$$

```c++
using namespace std;
using namespace neunet;
using namespace conv;
net_set<vect> A = {
    // Chann 0
    {{1, 2, 0},
     {1, 1, 3},
     {0, 2, 2}},
    // Chann 1
    {{0, 2, 1},
     {0, 3, 2},
     {1, 1, 0}}
    // Chann 2
    {{1, 2, 1},
     {0, 1, 3},
     {3, 3, 2}}
};
// Caffe 矩陣行列計數
// Line & column count of Caffe matrix
uint64_t caffe_A_ln_cnt  = 0,
         caffe_A_col_cnt = 0,
// 下采樣輸出矩陣的行列計數
// Line & column count of down sampling output matrix
         A_samp_ln_cnt   = 0,
         A_samp_col_cnt  = 0;
// 通道張量 A 的 Im2Col 矩陣張量
// Im2Col matrix tensor of channel tensor A
auto     im2col_A        = im2col_trans(A);
// 獲取 Caffe 矩陣轉換數據
// Get Caffe matrix transformation data
auto     caffe_A_data    = CaffeTransformData(A.length,
                                              caffe_A_ln_cnt,
                                              caffe_A_col_cnt,
                                              A[0].line_count,
                                              A[0].column_count,
                                              A_samp_ln_cnt,
                                              A_samp_col_cnt,
                                              2, 2, 1, 1, 0, 0);
// 打印通道張量在給定條件下的 Caffe 矩陣
// Print Caffe matrix of channel tensor A with given condition
cout << CaffeTransform(im2col_A, caffe_A_data, caffe_A_ln_cnt, caffe_A_col_cnt) << endl;
```

[<< 返回 Back](cover.md)

---

*更多關於通道矩陣與 Im2Col 矩陣轉換，請參閲* [`im2col_trans`](../ToolFunction/im2col_trans.md) *。*\
*Please refer to* [`im2col_trans`](../ToolFunction/im2col_trans.md) *for more details about the transformation between channel matrix set and Im2Col matrix.*

*更多關於 Caffe 矩陣數據請參閲* [`CaffeTransformData`](CaffeTransformData.md) *。*\
*Please refer to* [`CaffeTransformData`](CaffeTransformData.md) *for more detail about the Caffe matrix data.*

# `im2col_trans`

## 重載 Overload $^1$

```c++
callback_matrix neunet_vect im2col_trans(const net_set<neunet_vect> &chann_vec);
```

將通道矩陣集合轉換為 Im2Col 矩陣。\
Trnsform channel matrix set to Im2Col matrix.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`chann_vec`|通道矩陣集合，更多請參閲頭文件 [`bmio`](../../DataStructure/bmio/cover.md)<br>Channel matrix set, please refer to head file [`bmio`](../../DataStructure/bmio/cover.md)|*輸入<br>input*

返回 Im2Col 矩陣，張量。\
Return Im2Col matrix, the tensor.

## 重載 Overload $^2$

```c++
callback_matrix net_set<neunet_vect> im2col_trans(const neunet_vect &chann,
                                                  uint64_t          chann_ln_cnt,
                                                  uint64_t          chann_col_cnt);
```

將 Im2Col 矩陣轉換為通道矩陣集合。\
Trnsform Im2Col matrix to channel matrix set.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`chann`|Im2Col 張量矩陣<br>Im2Col tensor matrix|*輸入<br>input*
`chann_ln_cnt`|通道矩陣行計數<br>Line count of the channel matrix|*輸入<br>input*
`chann_col_cnt`|通道矩陣列計數<br>Column count of the channel matrix|*輸入<br>input*

返回通道矩陣集合。\
Return channel matrix set.

```c++
using namespace std;
using namespace neunet;
// 初始化通道
// Initialize the channel
net_set<vect> chann = {
    // 0
    {{1, 2},
     {3, 4}},
    // 1
    {{2, 4},
     {6, 8}},
    // 2
    {{1, 3},
     {5, 7}}};
// 轉換為 ImCol 矩陣
// Transform to Im2Col matrix
auto chann_im2col = im2col_trans(chann);
/* Im2Col 矩陣張量 matrix tensor
 * 1 2 1
 * 2 4 3
 * 3 6 5
 * 4 8 7
 */
cout << chann_im2col << endl;
```

[<< 返回 Back](cover.md)

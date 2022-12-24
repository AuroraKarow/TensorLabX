# `broadcast_addition`

```c++
template<typename matrix_elem_para,
         typename matrix_elem_para_v> net_matrix
         broadcast_addition(const matrix_elem_para &para,
                            bool                   subtract = false,
                            bool                   para_fst = false);
```

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`para`|廣播參數<br>Broadcasting parameter for addition of each matrix element|*輸入<br>input*
`subtract`|指定是否進行減法運算<br>Specify whether subtracting or not|*輸入<br>input*
`para_fst`|是否將參數置前<br>Put parameter to front or not|*輸入<br>input*

返回結果矩陣。\
Return result matrix.

[<< 返回 Back](cover.md)

---

*更多請參閲* [`elem_wise_opt`](elem_wise_opt.md) *。*\
*Please refer to* [`elem_wise_opt`](elem_wise_opt.md) *for more details.*

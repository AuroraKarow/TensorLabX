# `strassen_mult`

```c++
callback_matrix matrix_ptr strassen_mult(const matrix_ptr left, 
                                         uint64_t         left_ln_cnt,
                                         uint64_t         left_col_cnt,
                                         const matrix_ptr right,
                                         uint64_t         right_col_cnt,
                                         uint64_t         recursive_gate = 32);
```

Strassen 矩陣乘法算法。\
Strassen matrix multiplication algorithm.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`left`|乘法左矩陣指針，矩陣元素按照行進行排列<br>Left matrix pointer of multiplication, matrix elements are arranged by line direction|*輸入<br>input*
`left_ln_cnt`|左矩陣行計數<br>Line count of left matrix|*輸入<br>input*
`left_col_cnt`|左矩陣列計數<br>Column count of left matrix|*輸入<br>input*
`right`|乘法右矩陣指針，矩陣元素按照行進行排列<br>Left matrix pointer of multiplication, matrix elements are arranged by line direction|*輸入<br>input*
`right_col_cnt`|右矩陣列計數<br>Column count of right matrix|*輸入<br>input*
`recursive_gate`|遞歸出口值<br>Recursion exit value|*輸入<br>input*

返回矩陣乘法結果矩陣指針。\
Return result pointer of matrix multiplication.

[<< 返回 Back](cover.md)

# `init`

## 重載 Overload $^1$

```c++
callback_matrix matrix_ptr init(uint64_t elem_cnt);
```

指定矩陣元素模板類型和數量，初始化矩陣指針。\
Specify the element type and amount of the matrix element, initialize matrix pointer.

返回 $O$ 矩陣指針。\
Return $O$ matrix pointer.

## 重載 Overload $^2$

```c++
callback_matrix_n auto init(initializer_list<initializer_list<matrix_elem_t>> src,
                            uint64_t &ln_cnt,
                            uint64_t &col_cnt);
```

直接初始化矩陣指針並獲取矩陣的行列計數值。\
Initial matrix pointer directly, get line and column count value.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`src`|矩陣元素列，行排列<br>Matrix element list, line direct|*輸入<br>input*
`ln_cnt`|行計數<br>Line count|*輸出<br>Output*
`col_cnt`|列計數<br>Column count|*輸出<br>Output*

返回初始化矩陣指針。\
Return initial matrix pointer.

```c++
using namespace neunet;
// 初始化内存指針
// Initialize matrix pointers
uint64_t ln_cnt  = 0,
         col_cnt = 0;
auto mat_0 = matrix::init<double>(4),
     mat_1 = matrix::init({{0., 1.},
                           {2., 3.}}, ln_cnt, col_cnt);
// 打印
// Print
matrix::print(mat_0, ln_cnt, col_cnt);
std::cout << std::endl;
matrix::print(mat_1, ln_cnt, col_cnt);
// 釋放内存
// Release the memory
matrix::recycle(mat_0, mat_1);
```

[<< 返回 Back](cover.md)

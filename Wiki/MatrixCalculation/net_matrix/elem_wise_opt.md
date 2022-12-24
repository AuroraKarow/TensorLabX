# `elem_wise_opt`

## 重載 Overload $^1$

```c++
net_matrix elem_wise_opt(const net_matrix &val,
                         uint64_t         operation,
                         bool             elem_swap = false,
                         long double      epsilon   = 1e-8)
```

矩陣與矩陣對應位置元素之間運算。\
Operation between each two elements pair at corresponding position in respective matrix.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`val`|目標矩陣，該矩陣的尺寸應與當前矩陣相同<br>Target matrix whose shape should be same as current matrix|*輸入<br>input*
`operation`\*|操作代碼<br>Operation code|*輸入<br>input*
`elem_swap`|是否交換操作中兩個元素運算符左右的位置<br>Swap the elements left-right position of the operator in operation or not|*輸入<br>input*
`epsilon`|避免除 `0`<br>Avoid `0` divisor|*輸入<br>input*

返回結果矩陣。\
Return result matrix.

## 重載 Overload $^2$

```c++
template<typename matrix_elem_para,
         typename matrix_elem_para_v> net_matrix
         elem_wise_opt(const matrix_elem_para &para, 
                       uint64_t                operation,
                       bool                    para_fst = false,
                       long double             epsilon = 1e-8)
```

矩陣元素廣播式處理。\
Broadcasting operating the matrix elements.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`para`|廣播操作參數<br>Broadcasting operation parameter|*輸入<br>input*
`operation`\*|操作代碼<br>Operation code|*輸入<br>input*
`para_fst`|是否將參數置前<br>Put parameter to front or not|*輸入<br>input*
`epsilon`|避免除 `0`<br>Avoid `0` divisor|*輸入<br>input*

返回結果矩陣。\
Return result matrix.

```c++
using namespace std;
using namespace neunet;
vect a = {{1, 3},
          {5, 7}},
     b = {{2, 4},
          {6, 8}};
/* 獲取矩陣 a，b 的哈達瑪積
 * Get Hadamard product of matrix a, b.
 *
 *  2 12
 * 30 56
 */
cout << a.elem_wise_opt(b, MATRIX_ELEM_MULT) << endl;
cout << endl;
/* 以 2 為底數 矩陣 b 的元素為次數，計算元素乘方矩陣
 * Get power matrix with 2 as base and elements of matrix b as times
 *
 *  4  16
 * 64 256
 */
cout << b.elem_wise_opt(2, MATRIX_ELEM_POW, true) << endl;
```

[<< 返回 Back](cover.md)

---
\* 操作編碼 Operation code
編碼<br>Code|描述<br>Description
-|-
`MATRIX_ELEM_MULT`|乘<br>Multiplication
`MATRIX_ELEM_DIV`|除<br>Division
`MATRIX_ELEM_POW`|乘方<br>Power

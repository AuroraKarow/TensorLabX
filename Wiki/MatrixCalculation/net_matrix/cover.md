# `net_matrix`

```c++
matrix_declare class net_matrix;

#define neunet_vect matrix::net_matrix<matrix_elem_t>

typedef matrix::net_matrix<long double> vect;
typedef matrix::net_matrix<net_decimal> vect_dec;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
`line_count`|行數<br>Matrix line count
`column_count`|列數<br>Matrix column count
`element_count`|元素數<br>Matrix element count
`verify`|驗證當前矩陣，有效結果為 `true` 否則為 `false`<br>Verify current matrix, it would be `true` for valid result otherwise `false`
`determinant`|當前方陣行列式<br>Determinant of current phalanx
`inverse`|當前方陣求逆，屬性值 `determinant` 不能為 `0`<br>Inversor of current phalanx, the property `determinant` value should not be `0`
`transpose`|轉置<br>Transposition matrix
`atom`|有一個元素矩陣的元素<br>Element value of the matrix with only one element
`abs`|絕對值矩陣<br>Absolute matrix
`LU_decompse`|LU分解的結果矩陣，屬性值 `determinant` 不能為 `0`<br>Result phalanx of LU decomposition, the property `determinant` value should not be `0`
`adjugation`|方陣的伴隨矩陣<br>Adjugation of current phalanx
`ranking`|當前矩陣的秩<br>Ranking of current matrix
`float_format`|浮點類型元素的當前矩陣<br>Current matrix with float point type elements
`decimal_format`|高精度十進制數類型元素的當前矩陣<br>Current matrix with high precision deciaml type elements

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`+` `+=`|加<br>Addition
`-` `-=`|減<br>Subtraction
`*` `*=`|乘<br>Multiplication
`==`|等于<br>Equal to
`!=`|不等于<br>Not equal to
`=`|複製或移動賦值<br>Copy or move assignment
`[][]`|矩陣中指定行 `ln` 列 `col` 元素的引用<br>Reference of the element at specified line `ln` and column `col` in matrix

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
[`net_matrix`](net_matrix.md)|構造函數<br>Constructor
`fill_elem`|使用指定值替換所有矩陣元素<br>Replace all elements of matrix by specified value
`extremum_position`|獲取矩陣或[子矩陣](child.md)的極值[位置](../pos.md)列表，通過參數 `get_max` 指定 `true` 或 `fasle` 取極大值或極小值<br>Get element [position](../pos.md) list of the matrix or [child matrix](child.md) extremum, get maximum or minimum by specifying `true` or `false` of parameter `get_max`.
[`child`](child.md)|子矩陣<br>Child matrix
[`rotate_rectangle`](rotate_rectangle.md)|旋轉矩陣元素的排列<br>Rotate the matrix elements arrangement
[`mirror_flipping`](mirror_flipping.md)|鏡像對稱翻轉矩陣元素的排列<br>Flip the matrix elements arrangement mirror-symmetrically
`shape_verify`|验证矩阵的形状是否与指定的行和列计数值或目標矩阵相同<br>Verify the matrix shape whether same as the specified line and column count value or target matrix
`reshape`|重塑矩陣，使之與指定的行列計數值或目標矩陣形狀相同，矩陣元素應與指定行列計數值或目標矩陣元素計數值相同<br>Reshape the matrix same as either specified line and column count value or the shape of target matrix, the element amount of matrix should be equal to the product of specified line and column count value or the element count value of target matrix
`elem_swap`|以指定的兩個行或列索引值進行初等行列變換，如果參數 `ln_swap` 為 `true` 進行行變換否則為列變換<br>Elementary line or column transform with specified two line or column indexes, the line direction swapping would be executed for `true` of parameter `ln_swap` otherwise column direction
`elem_sum`|矩陣或[子矩陣](child.md)所有元素的和<br>Elements sum of all matrix or [child matrix](child.md)
[`elem_wise_opt`](elem_wise_opt.md)|矩陣元素操作<br>Matrix element wised operation
[`broadcast_addition`](broadcast_addition.md)|元素廣播式加<br>Elements broadcast addition
[`padding`](padding.md)|擴展矩陣<br>Padding the matrix
`cropping`|裁切矩陣，更多請參閲 [`padding`](padding.md)<br>Cropping the matrix, please refer to [`padding`](padding.md) for more details
`index`\*|訪問指定索引的矩陣指針元素引用<br>Access the reference of matrix pointer element at specified index
`reset`|手動釋放矩陣分配的内存<br>Release allocated memory of the matrix manually

[<< 返回 Back](../cover.md)

---

\* 參數 parameter `idx = ln * column_count + col`\
&ensp;更多請參閲運算符 `[][]` 和屬性 `column_count`\
&ensp;Please refer to operator `[][]` and property `column_count`

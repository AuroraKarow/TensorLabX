# `child`

## 重載 Overload $^1$

```c++
net_matrix child(uint64_t from_ln,
                 uint64_t to_ln,
                 uint64_t from_col,
                 uint64_t to_col,
                 uint64_t ln_dilate,
                 uint64_t col_dilate);
```

指定元素坐標，獲取子矩陣。對應參數不合法將會捨去部分操作。\
Get child matrix by specifying element position. It would remove some operation for corresponding parameters.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`from_ln`|父矩陣中子矩陣元素行方向的起始坐標，這個值不能大于參數 `to_ln` 的值<br>Beginning line direction position of the child matrix in parent matrix, this value should not be greater than the value of parameter `to_ln`|*輸入<br>input*
`to_ln`|父矩陣中子矩陣元素行方向的結束坐標，這個值不能大于屬性 `line_count` 的值<br>Ending line direction position of the child matrix in parent matrix, this value should not be greater than the value of property `line_count`|*輸入<br>input*
`from_col`|父矩陣中子矩陣元素列方向的起始坐標，這個值不能大于參數 `to_col` 的值<br>Beginning column direction position of the child matrix in parent matrix, this value should not be greater than the value of parameter `to_col`|*輸入<br>input*
`to_col`|父矩陣中子矩陣元素行方向的結束坐標，這個值不能大于屬性 `column_count` 的值<br>Ending column direction position of the child matrix in parent matrix, this value should not be greater than the value of property `column_count`|*輸入<br>input*
`ln_dilate`|行方向子矩陣取父矩陣元素的元素間隔<br>Line direction element dilation of child matrix getting parent matrix elements|*輸入<br>input*
`ln_dilate`|列方向子矩陣取父矩陣元素的元素間隔<br>Column direction element dilation of child matrix getting parent matrix elements|*輸入<br>input*

返回子矩陣。\
Return the child matrix.

## 重載 Overload $^2$

```c++
net_matrix child(uint64_t from_ln,
                 uint64_t c_ln_cnt,
                 uint64_t from_col,
                 uint64_t c_col_cnt);
```

指定父矩陣起始坐標與子矩陣行列計數，獲取子矩陣。對應參數不合法將會捨去部分操作。\
Get child matrix by specifying the beginning loaction in parent matrix with line and column count value. It would remove some operation for corresponding parameters.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`from_ln`|父矩陣中子矩陣元素行方向的起始坐標，這個值與參數 `c_ln_cnt` 的和不能大于屬性 `line_count` 的值<br>Beginning line direction position of the child matrix in parent matrix, the sum of this value and the value of parameter `c_ln_cnt` should not be greater than the value of property `line_count`|*輸入<br>input*
`c_ln_cnt`|子矩陣行計數值<br>Line count value of child matrix|*輸入<br>input*
`from_col`|父矩陣中子矩陣元素列方向的起始坐標，這個值與參數 `c_col_cnt` 的和不能大于屬性 `column_count` 的值<br>Beginning column direction position of the child matrix in parent matrix, the sum of this value and the value of parameter `c_col_cnt` should not be greater than the value of property `column_count`|*輸入<br>input*
`c_col_cnt`|子矩陣列計數值<br>Column count value of child matrix|*輸入<br>input*

返回子矩陣。\
Return the child matrix.

```c++
using namespace std;
using namespace neunet;
vect test(8, 8, true, 0, 10);
cout << test << endl << endl;
// 獲取子矩陣 a, b
// Get child matrix a, b
/* a
 * 行 [2, 8] 列 [4, 8] 擴張 [1 行 1 列]
 * line [2, 8] column [4, 8] dialtion (1 ln 1 col)
 * b
 * 左上 (3, 2) 行列 (2 行 2 列)
 * left top (3, 2) line & column count (2 ln 2 col)
 */
auto a = test.child(2, 7, 4, 7, 1, 1),
     b = test.child(3, 2, 2, 2);
cout << a <<  endl;
cout << endl;
cout << b <<  endl;
```

[<< 返回 Back](cover.md)

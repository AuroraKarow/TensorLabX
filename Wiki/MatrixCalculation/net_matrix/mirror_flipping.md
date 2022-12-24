# `mirror_flipping`

```c++
net_matrix mirror_flipping(bool vertical_symmetry = true);
```

指定水平或垂直對稱，對矩陣元素排佈進行鏡面翻轉。\
Specifying horizontal or vertical symmetry mirrored flip the element arrangement of matrix.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vertical_symmetry`|是否垂直對稱翻轉<br>Rotates clockwise or not|*輸入<br>input*

返回翻轉后的矩陣。\
Return the flipped matrix.

```c++
using namespace neunet;
/* 分別水平垂直對稱鏡像翻轉矩陣元素的排佈
 * Horizontal and vertical symmetry mirroed flip the element arrangement of matrix respectivel
 *  0  2  4  6
 *  8 10 12 14
 * 16 18 20 22
 */
vect test = {{ 0,  2,  4,  6},
             { 8, 10, 12, 14},
             {16, 18, 20, 22}};
// 垂直
// Vertical
cout << test.mirror_flipping() << endl;
cout << endl;
// 水平
// Horizontal
cout << test.mirror_flipping(false) << endl;
```

[<< 返回 Back](cover.md)

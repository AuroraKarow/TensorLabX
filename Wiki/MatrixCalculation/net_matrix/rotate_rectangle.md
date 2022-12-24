# `rotate_rectangle`

```c++
net_matrix rotate_rectangle(bool clockwise = true);
```

矩陣元素排列旋轉。\
Rotate the arrangement of matrix

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`clockwise`|是否順時針旋轉<br>Rotates clockwise or not|*輸入<br>input*

返回操作后的矩陣。\
Return the matrix after operation.

```c++
using namespace neunet;
/* 逆時針旋轉給定矩陣的元素排佈
 * Counterclockwise rotate given matrix element arrangement
 * 0 1 2
 * 3 4 5
 * ->
 * 2 5
 * 1 4
 * 0 3
 */
vect a = {{0, 1, 2},
          {3, 4, 5}};
cout << a << endl << endl;
cout << a.rotate_rectangle(false) << endl;
```

[<< 返回 Back](cover.md)

# 矩陣計算 Matrix Calculation

```c++
#include "net_matrix"

using namespace neunet::matrix;
```

以下列表列出了可用的類。\
Available class(es) list below.

類型<br>Class or type|描述<br>Description
-|-
[`net_matrix`](net_matrix/cover.md)|通用矩陣<br>Generic matrix
[`pos`](pos.md)|矩陣元素位置<br>Position of matrix element

以下列表列出了可用的函數。\
Available function(s) list below.

函數<br>Function|描述<br>Description
-|-
[`strassen_mult`](strassen_mult.md)|Strassen 矩陣乘法<br>Strassen matrix multiplication
`vect_sum`|獲取一個矩陣集合的和<br>Get sum of a matrix set
[`init`](init.md)|初始化矩陣指針<br>Matrix pointer initialization
`recycle`|回收0至多個矩陣指針内存，更多請參閲 [`init`](init.md)<br>Collect 0 or multiple matrix pointer memory, please refer to [`init`](init.md) for more details
`print`|打印指針矩陣，更多請參閲 [`init`](init.md)<br>Print matrix pointer, please refer to [`init`](init.md) for more details

[<< 返回 Back](../../README.md)

# `raw`

```c++
callback_matrix_n net_set<net_matrix<matrix_elem_t>> raw();
```

獲取位圖的通道矩陣集合。\
Get channel matrices of the bitmap.

返回對應數值類型矩陣的集合。\
Return matrix set with corresponding number type.

```c++
using namespace neunet::bmio;
using namespace neunet::matrix;
bitmap yuki("src/yuki.jpg");
// 以 double 類型獲取位圖通道原格式矩陣集合
// Get raw matrix set of bitmap channels, with double type
neunet::net_set<net_matrix<double>> raw_chann = yuki.raw<double>();
// 矩陣的元素類型將是 double
// The type of matrix element would be double
```

[<< 返回 Back](cover.md)

---

*更多關於矩陣請參閲* [`net_matrix`](../../../MatrixCalculation/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../../../MatrixCalculation/net_matrix/cover.md) *for more about matrix*

*更多關於動態集合請參閲* [`net_set`](../../net_set/cover.md)\
*Please refer to* [`net_set`](../../net_set/cover.md) *for more about dynamic set*

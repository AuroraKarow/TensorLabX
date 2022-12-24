# `net_matrix`

## 重載 Overload $^1$

```c++
net_matrix(uint64_t            mtx_ln_cnt,
           uint64_t            mtx_col_cnt,
           bool                rand_init = false,
           const matrix_elem_t &fst_rng  = 0,
           const matrix_elem_t &snd_rng  = 0,
           uint64_t            acc       = 5);
```

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`mtx_ln_cnt`|初始化行計數<br>Initial line count|*輸入<br>input*
`mtx_col_cnt`|初始化列計數<br>Initial column count|*輸入<br>input*
`rand_init`|指定是否使用僞隨機數初始化元素<br>Specify whether initializing the element with pseudo random number|*輸入<br>input*
`fst_rng`|僞隨機數範圍區間的第一個端點<br>First endpoint of the pseudo number generation interval|*輸入<br>input*
`snd_rng`|僞隨機數範圍區間的第二個端點<br>Second endpoint of the pseudo number generation interval|*輸入<br>input*
`acc`|隨機數生成精度<br>Generation precision of the pseudo random number|*輸入<br>input*

## 重載 Overload $^2$

```c++
template<typename matrix_elem_para,
         typename matrix_elem_para_v>
         net_matrix(const i_arg &src);
```

初始化原子矩陣。這個矩陣只有一個元素。\
Initialize atom matrix. This matrix has only one element.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`src`|原子值<br>Atom value|*輸入<br>input*

## 重載 Overload $^3$

```c++
net_matrix(matrix_ptr &&src       = nullptr,
           uint64_t   mtx_ln_cnt  = 0,
           uint64_t   mtx_col_cnt = 0);
```

矩陣指針初始化矩陣。\
Initialize matrix with matrix pointer.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`src`|矩陣指針<br>Matrix pointer|*輸入<br>input*
`mtx_ln_cnt`|初始化行計數<br>Initial line count|*輸入<br>input*
`mtx_col_cnt`|初始化列計數<br>Initial column count|*輸入<br>input*

## 重載 Overload $^4$

```c++
net_matrix(initializer_list<initializer_list<net_matrix>> _vect);
```

使用子矩陣組合初始化矩陣。\
Initialize matrix with child matrix combination.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`_vect`|子矩陣2D序列<br>Child matrix 2D list|*輸入<br>input*

## 重載 Overload $^5$

```c++
net_matrix(const net_sequence<net_matrix> &_vect,
           const net_sequence<pos>        &left_top_pos);
```

子矩陣序列以及子矩陣第一個元素在父矩陣中左上坐標序列初始化矩陣。\
Initialize matrix with child matrix sequence and parent matrix left-top position list of first element of each child matrix.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`_vect`|子矩陣序列<br>Child matrix sequence|*輸入<br>input*
`left_top_pos`|左上角父矩陣坐標序列<br>Parent matrix left top position sequence|*輸入<br>input*

```c++
// 初始化矩陣
// Initializing matrix
using namespace std;
using namespace neunet;
// O 矩陣，2行4列
// O Matrix, 2 rows 4 columns
vect test(2, 4);
cout << test << endl;
// 僞隨機元素，範圍[-6, 9]
// Pesudo random elements, range interval [-6, 9]
test = vect(2, 4, true, -6, 9);
cout << test << endl;
// 組合矩陣
// Matrix combination
vect a = {{0},
          {1},
          {2},
          {3}},
     b = {{0, 1, 2},
          {3, 4, 5},
          {6, 7, 8}},
     c = {{0, 1},
          {2, 3}},
     d = {{0, 1}},
     e = {{0, 1, 2, 3}},
     f = {{0}},
     g = {{0, 1, 2}},
     h = {{0, 1, 2}},
     i = {{a, {{b, {{c},
                    {d}}},
               {e,   f}}},
          {g,        h}};
vect ls[] = {a, b, c, d, e, f, g, h, i};
for (auto temp : ls) cout << temp << '\n' << endl;
```

[<< 返回 Back](cover.md)

---

*更多關於生成僞隨機數請參閲* [`num_rand`](../../DigitalCalculation/num_rand.md) *。*\
*Please refer to* [`num_rand`](../../DigitalCalculation/num_rand.md) *for more details about the pseudo random number genration.*

*更多請參閲* [`init`](../init.md) *和* [`pos`](../pos.md) *。*\
*Please refer to* [`init`](../init.md) *and* [`pos`](../pos.md) *for more details.*

*更多關於動態序列請參閲* [`net_sequence`](../../DataStructure/net_sequence/cover.md)\
*Please refer to* [`net_sequence`](../../DataStructure/net_sequence/cover.md) *for more about dynamic sequence*

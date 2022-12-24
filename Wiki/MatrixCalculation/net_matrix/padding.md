# `padding`

```c++
net_matrix padding(uint64_t top      = 0,
                   uint64_t right    = 0,
                   uint64_t bottom   = 0,
                   uint64_t left     = 0,
                   uint64_t ln_dist  = 0,
                   uint64_t col_dist = 0);
```

使用 `0` 元素擴展矩陣，使矩陣尺寸變大。\
Padding matrix with element `0` makes matrix shape larger.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`top`|頂部擴展長度<br>Top padding length|*輸入<br>input*
`right`|右邊擴展長度<br>Right padding length|*輸入<br>input*
`bottom`|底部擴展長度<br>Bottom padding length|*輸入<br>input*
`right`|左邊擴展長度<br>Left padding length|*輸入<br>input*
`ln_dist`|行擴展長度<br>Padding length between two lines|*輸入<br>input*
`col_dist`|列擴展長度<br>Padding length between two columns|*輸入<br>input*

返回擴展后的矩陣。\
Return padded matrix.

```c++
using namespace std;
using namespace neunet;
vect test = {{1, 2, 3, 4},
             {5, 6, 7, 8}};
/* 擴展矩陣（頂 1 右 2 底 1 左 2 行間距 3 列間距 2）
 * Padding matrix (top 1 right 2 bottom 1 left 2 line distance 3 column distance 2)
 * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 * 0 0 1 0 0 2 0 0 3 0 0 4 0 0
 * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 * 0 0 5 0 0 6 0 0 7 0 0 8 0 0
 * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 */
cout << test.padding(1, 2, 1, 2, 3, 2) << endl;
```

[<< 返回 Back](cover.md)

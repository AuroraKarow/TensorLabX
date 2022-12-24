# `lbl_orgn`

## 重載 Overload $^1$

```c++
vect lbl_orgn(uint64_t lbl_val,
              uint64_t type_cnt);
```

獲取單個標簽矩陣。\
Get individual label matrix.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`lbl_val`|標簽值<br>Label value|*輸入<br>input*
`type_cnt`|分類數量<br>Type count|*輸入<br>input*

返回標簽布爾矩陣。\
Return label bool matrix.

## 重載 Overload $^2$

```c++
net_set<vect> lbl_orgn(const net_set<uint64_t> &lbl_set,
                       uint64_t                type_cnt);
```

獲取標簽集合的矩陣集合。\
Get matrix set of label set.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`lbl_set`|標簽集合<br>Label set|*輸入<br>input*
`type_cnt`|分類數量<br>Type count|*輸入<br>input*

返回標簽布爾矩陣集合。\
Return label bool matrix set.

```c++
using namespace std;
using namespace neunet;
// 標簽和分類數目
// label and type count
uint64_t lbl_val  = 1,
         type_cnt = 5;
/* 0
 * 1
 * 0
 * 0
 * 0
 */
cout << lbl_orgn(lbl_val, type_cnt) << endl;
```

[<< 返回 Back](cover.md)

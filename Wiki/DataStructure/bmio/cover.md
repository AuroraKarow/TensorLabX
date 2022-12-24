# 位圖處理 Bitmap process

```c++
#include "bmio"

using namespace neunet::bmio;
```

以下列表列出了可用的類。\
Available class(es) list below.

類型<br>Class or type|描述<br>Description
-|-
[`bitmap`](bitmap/cover.md)|封裝類，位圖輸入輸出與處理。<br>Encapsulated class, bitmap I/O and process.
`bmio_bitmap`\*|位圖像素矩陣數組，長度為4且尺寸相同。<br>Pixel matrix array of bitmap, length is 4 and the shape of these matrices are same. Raw format.
`bmio_chann`\*|位圖通道矩陣<br>Channel matrix of bitmap

以下列出了頭文件包含的編碼。\
The list of code including in head file below.

編碼<br>Code|描述<br>Description
-|-
`bmio_rgb`|RGB通道數，3<br>RGB channels count, 3
`bmio_rgba`|RGBA通道數，4<br>RGBA channels count, 4
`bmio_r`|紅色通道<br>Red channel
`bmio_g`|綠色通道<br>Green channel
`bmio_b`|藍色通道<br>Blue channel
`bmio_a`|Alpha通道<br>Alpha channel

[<< 返回 Back](../cover.md)

---

\* *原型*<br>&emsp;*prototype*

```c++
typedef vect bmio_bitmap[4];
typedef vect bmio_chann;
```

---

*更多關於類型* `vect` *請參閲* [`net_matrix`](../../MatrixCalculation/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../../MatrixCalculation/net_matrix/cover.md) *for more about type* `vect`

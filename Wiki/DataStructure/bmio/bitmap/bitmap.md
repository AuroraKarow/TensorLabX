# `bitmap`

## 重載 Overload $^1$

```c++
bitmap(bmio_bitmap &&src);
```

使用位圖像素矩陣數組初始化位圖。\
Initialize bitmap with bitmap pixel matrix array.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`src`|位圖通道矩陣數組<br> Bitmap channels matrix array|*輸入 input*

```c++
using namespace neunet::bmio;
// 初始化一個位圖通道矩陣數組，位圖大小為20 * 20
// Initialize a bitmap channel matrix array, the shape of the bitmap is 20 * 20
bmio_bitmap map_raw;
// 創建紅色純色圖
// Create pure red color image
for (auto i = 0; i < bmio_rgb; ++i) {
    map_raw[i] = bmio_chann(20, 20);
    // 如果是紅色通道，填充當前通道矩陣元素的數值
    // Fill value to current channel matrix if red channel
    if (i == bmio_r) map_raw.fill_elem(255);
}
// 創建位圖實例
// Create bitmap instance
bitmap map(std::move(map_raw));
```

## 重載 Overload $^2$

```c++
bitmap(uint64_t ln_cnt  = 0,
       uint64_t col_cnt = 0,
       bool     alpha   = false);
```

指定位圖尺寸初始化空白位圖。\
Initialize blank bitmap with the shape size.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`ln_cnt`|位圖的高度<br>Height of the bitmap|*輸入 input*
`col_cnt`|位圖的寬度<br>Width of the bitmap|*輸入 input*
`alpha`|指定是否包含Alpha通道<br>Specify whether to include the Alpha channel|*輸入 input*

## 重載 Overload $^3$

```c++
bitmap(const char *dir,
       bool       alpha = false);
```

從本地目錄加載圖片初始化位圖。\
Initialize bitmap by loading image from local directory.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`dir`|本地文件的目錄, ASCII格式<br>Directory of local file, ASCII format|*輸入 input*
`alpha`|指定是否包含Alpha通道<br>Specify whether to include the Alpha channel|*輸入 input*

```c++
using namespace neunet::bmio;
// 加載本地文件 "src\yuki.jpg" 到位圖，並加載Alpha通道。
// Load local file "src\yuki.jpg" to bitmap and load Alpha channel.
bitmap map_yuki("src\\yuki.jpg", true);
// 加載本地文件 "src\mei.png" 到位圖
// Load local file "src\mei.png" to bitmap.
bitmap map_mei(L"src/mei.png");
```

## 重載 Overload $^4$

```c++
bitmap(const wchar_t *dir,
       bool          alpha = false);
```

使用寬字符格式目錄加載本地文件初始化位圖，參閲[重載 $^3$](#重載-overload-3)。\
Initialize bitmap by loading image from local directory, wide character format, please refer to [Overload $^3$](#重載-overload-3).

[<< 返回 Back](cover.md)

---

*更多關於矩陣請參閲* [`net_matrix`](../../../MatrixCalculation/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../../../MatrixCalculation/net_matrix/cover.md) *for more about matrix*

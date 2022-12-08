# `save`

## 重載 Overload $^1$

```c++
bool save(const char *dir_root,
          const char *name,
          uint64_t   ex_name,
          char       backslash = '\\');
```

指定位圖格式並保存為本地文件到目標目錄。\
Specify the format of bitmap and save as local file to target directory.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`dir_root`|根目錄<br>Root of target directory|*輸入<br>input*
`name`|文件名<br>File name|*輸入<br>input*
`ex_name`*|擴展名編碼<br>Extension name code|*輸入<br>input*
`backslash`|目錄斜杠格式，\'\\\' 或 \'/\'<br>Backslash format for directory, \'\\\' or \'/\'|*輸入<br>input*

保存成功返回`true`否則`false`\
Return `true` for saved successfully otherwise `false`

```c++
using namespace neunet::bmio;
bitmap hatsune("src/hatsune.png");
// 保存位圖 "hatsune" 為 *.jpg 文件到目錄 "src/special"，命名爲 hatsune
// Save bitmap "hatsune" as *.jpg file to directory "src/special", named hatsune
auto png_status = hatsune.save("src\\special", "hatsune", bmio_png);
// 保存一個副本為 *.bmp 文件到相同目錄
// Save a copy as *.bmp file at same directory
auto bmp_status = hatsune.save(L"src/special", L"hatsune", bmio_bmp, L'/');
```

## 重載 Overload $^2$

```c++
bool save(const wchar_t *dir_root,
          const wchar_t *name,
          uint64_t      ex_name,
          wchar_t       backslash = L'\\');
```

使用寬體字符作爲參數傳入函數，參閲[重載 $^1$](#重載-overload-1)。\
Using wide character as parameter of function input, please refer to [Overload $^1$](#重載-overload-1).

[<< 返回 Back](cover.md)

---

\* *文件擴展名編碼<br>&emsp;Extension name code*

編碼<br>Code|擴展名<br>Extension name
-|-
bmio_png|\*.png
bmio_jpg|\*.jpeg / \*.jpg
bmio_bmp|\*.bmp
bmio_gif|\*.gif
bmio_tif|\*.tiff

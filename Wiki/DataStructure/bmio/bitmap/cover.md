# `bitmap`

```c++
class bitmap final;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
`uint64_t height`*|位圖高度<br>Bitmap height
`uint64_t width`*|位圖寬度<br>Bitmap width
`bool verify`**|位圖有效驗證<br>Bitmap verification
`bool alpha_verify`**|位圖Alpha通道驗證<br>Bitmap Alpha channel verification
`bmio_chann gray`|位圖的灰度圖<br>Gray image of bitmap

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`=`|位圖實例移動或複製賦值操作<br>Move or copy assignment operation for bitmap instance.
`==`|判定兩個位圖是否相同<br>Determine if two bitmaps are same
`!=`|判定兩個位圖是否不同<br>Determine if two bitmaps are different
[`[][]`]([][].md)|獲取目標位置的通道像素值引用<br>Get channels pixel value reference of target position on bitmap

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
[`bitmap`](bitmap.md)|構造函數<br>Constructor
`load`|加載本地圖片文件，參閲 [`bitmap`](bitmap.md)<br>Load local image file, refer to [`bitmap`](bitmap.md)
[`save`](save.md)|保存位圖到本地目錄<br>Save bitmap to local directory
[`raw`](raw.md)|获取原始格式位图副本<br>Get raw format copy of bitmap
`reset`|手动释放内存<br>Release memory manually

[<< 返回 Back](../cover.md)

---

\* *單位 px*<br>&emsp;*unit px*

\*\* *如果驗證通過則為* `true` *否則為* `false`<br>&emsp;`true` *if verification passed otherwise* `false`

# 工具函數 Tool function

```c++
using namespace neunet;
```

名稱<br>Name|描述<br>Description
-|-
`vec_travel`|使用函數指針對矩陣所有元素進行廣播處理<br>Broadcasting operate all elements of matrix with function pointer
`divisor_dominate`|將矩陣所有 `0` 元素使用參數 `epsilon` 的值替換<br>Substitute all `0` elements of matrix with parameter `epsilon`
[`im2col_trans`](im2col_trans.md)|通道矩陣集合與Im2Col矩陣轉換<br>Transform between channel matrix set and Im2Col matrix
`chann_vec_pad`|擴展Im2Col矩陣的所有通道，更多請參閲 [`im2col_trans`](im2col_trans.md) 和 [`net_matrix::padding`](../MatrixCalculation/net_matrix/padding.md)<br>Padding all channels of Im2Col matrix, please refer to [`im2col_trans`](im2col_trans.md) and [`net_matrix::padding`](../MatrixCalculation/net_matrix/padding.md) for more details
`chann_vec_crop`|裁剪Im2Col矩陣的所有通道，更多請參閲 [`im2col_trans`](im2col_trans.md) 和 [`net_matrix::padding`](../MatrixCalculation/net_matrix/padding.md)<br>Cropping all channels of Im2Col matrix, please refer to [`im2col_trans`](im2col_trans.md) and [`net_matrix::padding`](../MatrixCalculation/net_matrix/padding.md) for more details
`print_train_progress`|提供當前進度、進度總和、準確度、召回率和耗時，打印訓練狀態<br>Provide current progress, full progress, accuracy, recall rate and duration, print training status
`print_epoch_status`|提供當前 epoch 值、準確度、召回率和耗時，打印 Epoch 狀態<br>Provide current epoch value, accuracy, recall rate and duration, print Epoch status
`print_output_status`|提供網絡批量輸出和對應標簽集合，打印輸出狀體<br>Provide network batch output and correspondinig label set print output status
[`lbl_orgn`](lbl_orgn.md)|獲取標簽矩陣<br>Get label matrix
[`output_acc_rc`](output_acc_rc.md)|異步獲取網絡輸出的準確數和召回數<br>Get network output accurate and recall count asynchronously

[<< 返回 Back](../cover.md)

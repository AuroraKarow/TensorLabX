# `output_acc_rc`

```c++
callback_matrix void output_acc_rc(const neunet_vect &output,
                                   long double       train_acc,
                                   uint64_t          lbl,
                                   atomic_uint64_t   &acc_cnt,
                                   atomic_uint64_t   &rc_cnt);
```

異步任務中獲取神經網絡輸出矩陣的準確值與召回值計數。\
Get accurate and recall count of ANN output matrix in asynchronous task.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`output`|異步任務神經網絡輸出<br>ANN output matrix in asynchronous task|*輸入<br>input*
`train_acc`|網絡訓練準確率<br>Accuracy of network training accuracy|*輸入<br>input*
`lbl`|標簽值<br>Label value|*輸入<br>input*
`acc_cnt`|准確值計數<br>Accurate value count|*輸入 & 输出<br>input & Output*
`rc_cnt`|召回值計數<br>Recall value count|*輸入 & 输出<br>input & Output*

[<< 返回 Back](cover.md)

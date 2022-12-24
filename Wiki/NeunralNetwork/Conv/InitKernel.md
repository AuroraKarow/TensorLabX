# `InitKernel`

```c++
callback_matrix neunet_vect InitKernel(uint64_t            iAmt,
                                       uint64_t            iChannCnt,
                                       uint64_t            iLnCnt,
                                       uint64_t            iColCnt,
                                       const matrix_elem_t &dFstRng = 0,
                                       const matrix_elem_t &dSndRng = 0,
                                       uint64_t            iAcc = 8)
```

偽隨機數初始化捲積核。\
Initializing convolution kernels with pseudo random number.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`iAmt`|卷幾個數量<br>Kernel amount|*輸入<br>Output*
`iChannCnt`|捲積核通道數<br>Kernel channel count|*輸入<br>input*
`iLnCnt`|捲積核通道矩陣行計數<br>Line count of kernel channel matrix|*輸入<br>input*
`iColCnt`|捲積核通道矩陣列計數<br>Column count of kernel channel matrix|*輸入<br>input*
`dFstRng`|僞隨機數範圍區間的第一個端點<br>First endpoint of the pseudo number generation interval|*輸入<br>input*
`dSndRng`|僞隨機數範圍區間的第二個端點<br>Second endpoint of the pseudo number generation interval|*輸入<br>input*
`iAcc`|隨機數生成精度<br>Generation precision of the pseudo random number|*輸入<br>input*

返回捲積核矩陣張量，這個矩陣與 Caffe 和 Im2Col 矩陣格式均不相同。\
Return kenel matrix tensor, this matrix format is different from both Caffe and Im2Col matrix.

[<< 返回 Back](cover.md)

---

*更多關於生成僞隨機數請參閲* [`num_rand`](../../DigitalCalculation/num_rand.md) *。*\
*Please refer to* [`num_rand`](../../DigitalCalculation/num_rand.md) *for more details about the pseudo random number genration.*

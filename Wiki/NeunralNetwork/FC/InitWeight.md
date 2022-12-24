# `InitWeight`

```c++
callback_matrix neunet_vect InitWeight(uint64_t            iInputLnCnt,
                                       uint64_t            iOutputLnCnt,
                                       const matrix_elem_t &dFstRng = 0,
                                       const matrix_elem_t &dSndRng = 0,
                                       uint64_t            iAcc = 8);
```

初始化全連接層權重矩陣。\
Initialize the weight of fully connection layer.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`iInputLnCnt`|輸入向量的行數，輸入神經元<br>Line count of the input vector, input neurons|*輸入<br>input*
`iOutputLnCnt`|輸出向量的行數，輸出神經元<br>Line count of the output vector, output neurons|*輸入<br>input*
`dFstRng`|僞隨機數範圍區間的第一個端點<br>First endpoint of the pseudo number generation interval|*輸入<br>input*
`dSndRng`|僞隨機數範圍區間的第二個端點<br>Second endpoint of the pseudo number generation interval|*輸入<br>input*
`iAcc`|隨機數生成精度<br>Generation precision of the pseudo random number|*輸入<br>input*

返回權重矩陣。\
Return weight matrix.

[<< 返回 Back](cover.md)

---

*更多關於生成僞隨機數請參閲* [`num_rand`](../../DigitalCalculation/num_rand.md) *。*\
*Please refer to* [`num_rand`](../../DigitalCalculation/num_rand.md) *for more details about the pseudo random number genration.*

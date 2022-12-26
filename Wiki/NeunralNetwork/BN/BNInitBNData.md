# `BNInitBNData`

```c++
callback_matrix void BNInitBNData(BNData<matrix_elem_t> &BdData,
                                  uint64_t              iTrainBatchSize,
                                  uint64_t              iTrainBatchCnt);

```

BN 數據初始化。\
BN data initialization.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`BdData`|BN層數據<br>BN layer data|*輸入 & 输出<br>input & Output*
`iTrainBatchSize`|訓練數據批次大小<br>Train data batch size|*輸入<br>input*
`iTrainBatchCnt`|訓練數據批量，訓練數據總量與訓練數據批次大小的商<br>Train data batch count, quotient of train data quantity and batch size|*輸入<br>input*

[<< 返回 Back](cover.md)

# `BNData`

```c++
matrix_declare struct BNData final;
```

這個數據結構用於 BN 層的數據存儲，只需要手動初始化即可。\
This data structure is used to store the data of BN layer. It only need to initialize manually.

```c++
bool BNDataInit(uint64_t iTrainBatchSize = 1,
                uint64_t iTrainBatchCnt  = 1);
```

實例用於初始化的函數。\
Function for instance initialization.

參數<br>Paramters|描述<br>Description
-|-
`iTrainBatchSize`|訓練批大小<br>Batch size of training
`iTrainBatchCnt`|訓練批數量<br>Batch count of training

[<< 返回 Back](cover.md)

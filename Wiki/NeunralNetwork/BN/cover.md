# `批歸一化 Batch normalization`

```c++
using namespace neunet;
```

以下列表列出了可用的數據結構。\
Available data structure(s) list below.

名稱<br>Name|描述<br>Description
-|-
[`BNData`](BNData.md)|BN 數據存儲結構<br>BN data structure for storage

以下列表列出了可用的函數。\
Available function(s) list below.

函數<br>Function|描述<br>Description
-|-
[`BNInitBetaGamma`](BNInitBetaGamma.md)|$shift$ 和 $scale$ 值初始化<br>$shift$ and $scale$ value initialization
[`BNTrain`](BNTrain.md)|BN 層前向傳播<br>BN layer forward propagation
[`BNGradLossToInputGammaBeta`](BNGradLossToInputGammaBeta.md)|BN 層反向傳播<br>BN layer backward propagation
[`BNDeduce`](BNDeduce.md)|用于网络测试BN推测<br>BN deducing for network testing

[<< 返回 Back](../cover.md)

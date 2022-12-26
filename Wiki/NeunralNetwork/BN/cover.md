# `批歸一化 Batch normalization`

```c++
using namespace neunet;
```

函數<br>Function|描述<br>Description
-|-
[`BNInitBetaGamma`](BNInitBetaGamma.md)|$shift$ 和 $scale$ 值初始化<br>$shift$ and $scale$ value initialization
[`BNInitBNData`](BNInitBNData.md)|BN 數據初始化<br>BN data initialization
[`BNTrain`](BNTrain.md)|BN 層前向傳播<br>BN layer forward propagation
[`BNGradLossToInputGammaBeta`](BNGradLossToInputGammaBeta.md)|BN 層反向傳播<br>BN layer backward propagation
[`BNDeduce`](BNDeduce.md)|用于网络测试BN推测<br>BN deducing for network testing

[<< 返回 Back](../cover.md)

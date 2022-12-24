# 神經網絡實驗 Neural network experiment

```c++
#include "neunet"
```

神經網絡實驗包括了封裝的矩陣運算函數、激活函數以及網絡層的基礎函數。這個部分將介紹如何使用這些函數進行基礎網絡的搭建。\
ANN experiment includes encapsulated matrix calculation function, activation function and network layer basic function. This section will introduce how to build a simple ANN structure with these functions.

運算使用 Im2Col 格式的張量矩陣。提及的網絡運算矩陣屬性如通道計數、行計數或列計數等是原始格式矩陣的而非 Im2Col 格式。\
The calculation format of the tensor matrix is Im2Col. The mentioned properties of the network calculation matrix like channel count, line count or column count etc. are in original format matrix not Im2Col.

- [激活函數 Activation function](ActivationFunction/cover.md)
- [工具函數 Tool function](ToolFunction/cover.md)
- [梯度下降 Gradient descent](GradientDescent/cover.md)
- [全連接運算 Fully connection calculation](FC/cover.md)
- [卷積運算 Convolution calculation](Conv/cover.md)
- [批歸一化 Batch normalization](BN/cover.md)

通過基礎函數，可以擴展神經網絡的基礎運算内容，可以滿足更多的需求。通過封裝這些函數為數據結構或類，可以更快的完成網絡結構的搭建。\
It can extend the basic calculation content of ANN and satisfy more necessity via these basic functions. By encapsulating these function to data structure or class, the ANN structure building could be completed more swift.

- [基礎函數搭建神經網絡 Build ANN with basic functions](FunctionSample.md)
- [封裝神經網絡模塊 Encapsulate ANN module](EncapsulateSample.md)

[<< 返回 Back](../../README.md)

---

*更多關於矩陣請參閲* [`net_matrix`](../MatrixCalculation/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../MatrixCalculation/net_matrix/cover.md) *for more about matrix*

*更多關於動態集合請參閲* [`net_set`](../DataStructure/net_set/cover.md)\
*Please refer to* [`net_set`](../DataStructure/net_set/cover.md) *for more about dynamic set*

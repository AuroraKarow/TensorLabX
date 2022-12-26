# 激活函數 Activation function

```c++
using namespace neunet;

#define NEUNET_SIGMOID    0x0001
#define NEUNET_RELU       0x0002
#define NEUNET_ARELU      0x0003
#define NEUNET_ARELU_LOSS 0x0004
#define NEUNET_SOFTMAX    0x0005
```

- [$Sigmoid$](Sigmoid.md)
- [$ReLU$](ReLU.md)
- [$AReLU$](AReLU.md)
- [$Softmax$](Softmax.md)

以上為内置的激活函數，當然可以參照函數聲明自定義激活函數。\
Function above is built-in activation function, it could be also customized referring the built-in function statement.

[<< 返回 Back](../cover.md)

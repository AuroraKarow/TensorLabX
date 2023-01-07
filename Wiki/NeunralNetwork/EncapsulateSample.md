# 封裝函數部署 ANN<br>Encapsulate functions to deploy ANN

使用内置的封裝類，直接生成 LeNet-5 網絡對 mnist 數據集進行訓練測試。\
Generate Lenet-5 network directively by using built-in encapsulated classes to train and test the mnist dataset.

源文件為 [`main.cpp`](../../main.cpp)\
The source code file is [`main.cpp`](../../main.cpp)

網絡層類型<br>Network layer type|描述<br>Description|初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-|-|-
`NetLayerAct`|激活層<br>Activation layer|`iCurrActType`|激活函數類型<br>Activation function type
`NetLayerFlat`|攤平層<br>Flatting layer|`void`|NULL
`NetLayerPC`|擴展裁切層<br>Padding & cropping layer|`iTopCnt`|
|||`iRightCnt`|
|||`iBottomCnt`|
|||`iLeftCnt`|
|||`iLnDistCnt`|
|||`iColDistCnt`|
`NetLayerBias`|偏倚層<br>Bias vector layer|`dInitLearnRate`|
|||`dRandFstRng`|
|||`dRandSndRng`|
|||`iRandAcc`|
`NetLayerFC`|全連接層<br>Fully connection layer|`iCurrOutputLnCnt`|
|||`dInitLearnRate`|
|||`dRandFstRng`|
|||`dRandSndRng`|
|||`iRandAcc`|
`NetLayerConv`|捲積層<br>Convolution layer|`iCurrKernelAmt`|
|||`iCurrKernelLnCnt`|
|||`iCurrKernelColCnt`|
|||`iCurrLnStride`|
|||`iCurrColStride`|
|||`iCurrLnDilate`|
|||`iCurrColDilate`|
|||`dInitLearnRate`|
|||`dRandFstRng`|
|||`dRandSndRng`|
|||`iRandAcc`|
`NetLayerPool`|池化層<br>Pooling layer|`iCurrPoolType`|
|||`iCurrFilterLnCnt`|
|||`iCurrFilterColCnt`|
|||`iCurrLnStride`|
|||`iCurrColStride`|
|||`iCurrLnDilate`|
|||`iCurrColDilate`|
`NetLayerBN`|批歸一化層<br>BN layer|`dShift`|
|||`dScale`|
|||`dInitLearnRate`|
|||`dDmt`|

```c++
#pragma once

#include <iostream>
#include "neunet"
#include "dataset"

using namespace std;
using namespace neunet;
using namespace layer;
using namespace dataset;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.\n" << endl;
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;

    // mnist data loading

    std::string root = "...\\MNIST\\";
    mnist train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
    test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());

    // network declaration

    auto dLearnRate = .4l;
    NeunetCore net_core(125, 125, .1l);

    // layers adding
    
    AddLayer<NetLayerConv>(net_core, 20, 5, 5, 1, 1, 0, 0, dLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, 1e-5l);
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    AddLayer<NetLayerConv>(net_core, 50, 5, 5, 1, 1, 0, 0, dLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, 1e-5l);
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    AddLayer<NetLayerFlat>(net_core);
    AddLayer<NetLayerFC>(net_core, 500, dLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, 1e-5l);
    AddLayer<NetLayerAct>(net_core, NEUNET_SIGMOID);
    AddLayer<NetLayerFC>(net_core, 10, dLearnRate);
    AddLayer<NetLayerAct>(net_core, NEUNET_SOFTMAX);

    // shape initialization

    RunInit(net_core, train.element_count, test.element_count, train.element_line_count, train.element_column_count, 1);

    // threads

    TrainDeduceThread(net_core, train.elem, train.lbl, test.elem, test.lbl, mnist_orgn_size);
    DataShowThread(net_core, train.element_count, test.element_count);

    auto chrono_end = NEUNET_CHRONO_TIME_POINT;
    cout << '\n' << (chrono_end - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}
```

[<< 返回 Back](cover.md)

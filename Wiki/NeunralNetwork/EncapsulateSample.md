# 封裝函數部署 ANN<br>Encapsulate functions to deploy ANN

使用内置的封裝類，直接生成 LeNet-5 網絡對 mnist 數據集進行訓練測試。\
Generate Lenet-5 network directively by using built-in encapsulated classes to train and test the mnist dataset.

源文件為 [`main.cpp`](../../main.cpp)\
The source code file is [`main.cpp`](../../main.cpp)

## 内置網絡層<br>Built-in network layer

### `NetLayerAct`

激活層\
Activation layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`iCurrActType`|激活函數類型<br>Activation function type

### `NetLayerFlat`

攤平層\
Flatting layer

### `NetLayerPC`

擴展裁切層\
Padding & cropping layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`iTopCnt`|矩陣頂部擴展或裁切值<br>Padding or cropping element count of the top of matrix
`iRightCnt`|矩陣右邊擴展或裁切值<br>Padding or cropping element count of matrix right side
`iBottomCnt`|矩陣底部擴展或裁切值<br>Padding or cropping element count of the bottom of matrix
`iLeftCnt`|矩陣左邊擴展或裁切值<br>Padding or cropping element count of matrix left side
`iLnDistCnt`|行間元素的擴展或裁切值<br>Padding ot cropping element count between two lines
`iColDistCnt`|列間元素的擴展或裁切值<br>Padding ot cropping element count between two columns

### `NetLayerBias`

偏倚層\
Bias vector layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`dInitLearnRate`|初始化學習率<br>Initial learn rate
`dRandFstRng`|僞隨機數初始化區間的第一個端點<br>First interval endpoint of pseudo random number generation
`dRandSndRng`|僞隨機數初始化區間的第二個端點<br>Second interval endpoint of pseudo random number generation
`iRandAcc`|僞隨機數產生精度<br>Accuracy of pseudo random number generation

### `NetLayerFC`

全連接層\
Fully connection layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`iCurrOutputLnCnt`|當前層輸出向量的行計數<br>Line count of current layer output vector
`dInitLearnRate`|初始化學習率<br>Initial learn rate
`dRandFstRng`|僞隨機數初始化區間的第一個端點<br>First interval endpoint of pseudo random number generation
`dRandSndRng`|僞隨機數初始化區間的第二個端點<br>Second interval endpoint of pseudo random number generation
`iRandAcc`|僞隨機數產生精度<br>Accuracy of pseudo random number generation

### `NetLayerConv`

捲積層\
Convolution layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`iCurrKernelAmt`|捲積核數量<br>Convolution kernel amount
`iCurrKernelLnCnt`|捲積核行計數<br>Line count of convolution kernel
`iCurrKernelColCnt`|捲積核列計數<br>Column count of convolution kernel
`iCurrLnStride`|捲積行向步幅<br>Line direction convolution stride
`iCurrColStride`|捲積列向步幅<br>Column direction convolution stride
`iCurrLnDilate`|捲積行向擴張值<br>Line direction convolution dilation
`iCurrColDilate`|捲積列向擴張值<br>Column direction convolution dilation
`dInitLearnRate`|初始化學習率<br>Initial learn rate
`dRandFstRng`|僞隨機數初始化區間的第一個端點<br>First interval endpoint of pseudo random number generation
`dRandSndRng`|僞隨機數初始化區間的第二個端點<br>Second interval endpoint of pseudo random number generation
`iRandAcc`|僞隨機數產生精度<br>Accuracy of pseudo random number generation

### `NetLayerPool`

池化層\
Pooling layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`iCurrPoolType`|池化類型<br>Pooling type
`iCurrFilterLnCnt`|過濾器行計數<br>Line count of the filter
`iCurrFilterColCnt`|過濾器列計數<br>Coumn count of the filter
`iCurrLnStride`|池化行向步幅<br>Line direction pooling stride
`iCurrColStride`|池化列向步幅<br>Column direction pooling stride
`iCurrLnDilate`|池化行向擴張值<br>Line direction pooling dilation
`iCurrColDilate`|池化列向擴張值<br>Column direction pooling dilation

### `NetLayerBN`

批歸一化層\
BN layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`dShift`|初始化 $shift$ 值<br>Initial $shift$ value
`dScale`|初始化 $scale$ 值<br>Initial $scale$ value
`dInitLearnRate`|初始化學習率<br>Initial learn rate
`dDmt`|零分母<br>Zero denominator

## 示例<br>Example

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
    // 整個過程的開始時間點
    // whole process beginning time point
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;
    // 加載 mnist 數據根目錄
    // mnist data root directory loading
    std::string root = "...\\MNIST\\";
    // 訓練集
    // train set
    mnist train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
    // 測試集
    // test set
    test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());
    // 學習率
    // learn rate
    auto dLearnRate = .4l;
    // 初始化網絡核心
    // Initialize network core
                        // 訓練批大小
                        // train batch size
    NeunetCore net_core(125,
                        // 測試批大小
                        // test batch size 
                        125,
                        // 訓練精度
                        // train precision
                        .1l);
    /* 添加網絡層 add network layer */
    /* C1
     * 核數量 kernel quantity - 20
     * 核尺寸 kernel shape - 5 行 lines 5 列 columns
     * 步幅 stride - 行 line 1 列 column 1
     * 擴張 dilation - 行 line 0 列 column 0
     * 學習率 - learn rate
     */
    AddLayer<NetLayerConv>(net_core, 20, 5, 5, 1, 1, 0, 0, dLearnRate);
    /* BN
     * shift - 0
     * scale - 1
     * 學習率 - learn rate 1e-5
     */
    AddLayer<NetLayerBN>(net_core, 0, 1, 1e-5l);
    // 激活 activation - ReLU
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    /* P2
     * 池化類型 pooling type - 平均池化 average pooling
     * 過濾器尺寸 filter shape - 2 行 lines 2 列 columns
     * 步幅 stride - 行 line 2 列 column 2
     */
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    // C3
    AddLayer<NetLayerConv>(net_core, 50, 5, 5, 1, 1, 0, 0, dLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, 1e-5l);
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    // P4
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    // 攤平 flatting
    AddLayer<NetLayerFlat>(net_core);
    /* F5
     * 輸出向量行數 Line count of output vector - 500 行 lines
     * 學習率 - learn rate
     */
    AddLayer<NetLayerFC>(net_core, 500, dLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, 1e-5l);
    // 激活 activation - sigmoid
    AddLayer<NetLayerAct>(net_core, NEUNET_SIGMOID);
    /* F6
     * 輸出向量行數 Line count of output vector - 10 行 lines
     * 學習率 - learn rate
     */
    AddLayer<NetLayerFC>(net_core, 10, dLearnRate);
    // 激活 activation - softmax
    AddLayer<NetLayerAct>(net_core, NEUNET_SOFTMAX);
    // 運行初始化
    // Running initialization
    RunInit(net_core,
            // 訓練集數據個數
            // data element count of train set
            train.element_count,
            // 測試集數據個數
            // data element count of test set
            test.element_count,
            // 數據集輸入矩陣行計數
            // Line count of the dataset input matrix
            train.element_line_count,
            // 數據集輸入矩陣列計數
            // Column count of the dataset input matrix
            train.element_column_count,
            // 通道計數
            // Channel count
            1);
    // 訓練與推導綫程
    // train & deduce thread
    TrainDeduceThread(net_core, 
        // 訓練集數據
        // train set data
        train.elem,
        // 訓練集標簽
        // train set label
        train.lbl,
        // 測試集數據
        // test set data
        test.elem,
        //  測試集標簽
        // test set label
        test.lbl,
        // mnist 標簽數
        // mnist label count
        mnist_orgn_size);
    // 輸出顯示綫程
    // output display thread
    DataShowThread(net_core,
        // 訓練集數據數
        // train set data count
        train.element_count,
        // 測試集數據數
        // test set data count
        test.element_count);
    // 整個過程的結束時間點
    // whole process ending time point
    auto chrono_end = NEUNET_CHRONO_TIME_POINT;
    cout << '\n' << (chrono_end - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}
```

[<< 返回 Back](cover.md)

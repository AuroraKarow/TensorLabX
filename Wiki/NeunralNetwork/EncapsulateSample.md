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
`bIsPadMode`|`true`為擴展模式，`false`為裁切模式<br>`true` is padding mode, `false` is cropping mode
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
`dLearnRate`|初始化學習率<br>Initial learn rate
`dRandFstRng`|僞隨機數初始化區間的第一個端點<br>First interval endpoint of pseudo random number generation
`dRandSndRng`|僞隨機數初始化區間的第二個端點<br>Second interval endpoint of pseudo random number generation
`iRandAcc`|僞隨機數產生精度<br>Accuracy of pseudo random number generation

### `NetLayerFC`

全連接層\
Fully connection layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`iOutLnCnt`|當前層輸出向量的行計數<br>Line count of current layer output vector
`dLearnRate`|初始化學習率<br>Initial learn rate
`dRandFstRng`|僞隨機數初始化區間的第一個端點<br>First interval endpoint of pseudo random number generation
`dRandSndRng`|僞隨機數初始化區間的第二個端點<br>Second interval endpoint of pseudo random number generation
`iRandAcc`|僞隨機數產生精度<br>Accuracy of pseudo random number generation

### `NetLayerConv`

捲積層\
Convolution layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`iKernelQty`|捲積核數量<br>Convolution kernel amount
`iKernelLnCnt`|捲積核行計數<br>Line count of convolution kernel
`iKernelColCnt`|捲積核列計數<br>Column count of convolution kernel
`iLnStride`|捲積行向步幅<br>Line direction convolution stride
`iColStride`|捲積列向步幅<br>Column direction convolution stride
`iLnDilate`|捲積行向擴張值<br>Line direction convolution dilation
`iColDilate`|捲積列向擴張值<br>Column direction convolution dilation
`dLearnRate`|初始化學習率<br>Initial learn rate
`dRandFstRng`|僞隨機數初始化區間的第一個端點<br>First interval endpoint of pseudo random number generation
`dRandSndRng`|僞隨機數初始化區間的第二個端點<br>Second interval endpoint of pseudo random number generation
`iRandAcc`|僞隨機數產生精度<br>Accuracy of pseudo random number generation

### `NetLayerPool`

池化層\
Pooling layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`iPoolType`|池化類型<br>Pooling type
`iFilterLnCnt`|過濾器行計數<br>Line count of the filter
`iFilterColCnt`|過濾器列計數<br>Coumn count of the filter
`iLnStride`|池化行向步幅<br>Line direction pooling stride
`iColStride`|池化列向步幅<br>Column direction pooling stride
`iLnDilate`|池化行向擴張值<br>Line direction pooling dilation
`iColDilate`|池化列向擴張值<br>Column direction pooling dilation

### `NetLayerBN`

批歸一化層\
BN layer

初始化參數<br>Initialization parameters|參數描述<br>Parameter description
-|-
`dShiftPlaceholder`|初始化 $shift$ 值<br>Initial $shift$ value
`dScalePlaceholder`|初始化 $scale$ 值<br>Initial $scale$ value
`dShiftLearnRate`|$shift$ 值學習率<br>$shift$ learn rate
`dScaleLearnRate`|$scale$ 值學習率<br>$scale$ learn rate
`dMovAvgDecay`|滑動平均衰退率<br>Decay rate of moving average

## 示例<br>Example

```c++
#pragma once
// 包含頭文件
// includes head file
#include <iostream>
// 神經網絡
// ANN
#include "neunet"
// 用於實驗的數據集
// dataset for experiment
#include "dataset"
// 命名空間
// namespace
using namespace std;
using namespace neunet;
// using namespace neunet::layer;
using namespace layer;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.\n" << endl;
    // 開始計時
    // start timing
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;
    // BN 層和其他層的學習率
    // learn rate for BN layer and other layers
    auto dNormLearnRate = .4l, dBNLearnRate = 1e-5l;
    /* 初始化 ANN 核心
     * initialize ANN core
     * 訓練測試的批次大小 batch size of train & test
     * 125, 125
     * 訓練精度 train precision
     * 0.1
     */
    NeunetCore net_core(125, 125, .1l);
    // 添加網絡層
    // add layer to network
    /* C1
     * 捲積 convolution
     * 捲積核數量 quantity of kernels for convolution
     * 20
     * 核的行列計數（尺寸） line & column count of kernels (shape)
     * 5, 5
     * 下采樣行列方向步幅 down sampling stride in line & column direct
     * 1, 1
     * 下采樣過濾器行列擴張值 line & column direct dilation of down sampling
     * 0, 0
     * 學習率 learn rate
     * dNormLearnRate
     */
    AddLayer<NetLayerConv>(net_core, 20, 5, 5, 1, 1, 0, 0, dNormLearnRate);
    /* 批歸一化
     * batch normalization
     * shift & scale
     * 0, 1
     * shift & scale 學習率 learn rate
     * dBNLearnRate, dBNLearnRate
     */
    AddLayer<NetLayerBN>(net_core, 0, 1, dBNLearnRate, dBNLearnRate);
    /* 激活函數
     * activation function
     * ReLU
     */
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    /* P2
     * 池化
     * pooling
     * 池化類型 pooling type
     * NEUNET_POOL_AVG
     * 平均池化 average pooling
     * 過濾器行列計數（尺寸） line & column count of filter (shape)
     * 2, 2
     * 下采樣行列方向步幅 down sampling stride in line & column direct
     * 2, 2
     */
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    // C3
    AddLayer<NetLayerConv>(net_core, 50, 5, 5, 1, 1, 0, 0, dNormLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, dBNLearnRate, dBNLearnRate);
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    // P4
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    AddLayer<NetLayerFlat>(net_core);
    /* F5
     * 全連接
     * fully connection
     * 輸出向量行數 line count of output vector
     * 200
     * 學習率 learn rate
     * dNormLearnRate
     */
    AddLayer<NetLayerFC>(net_core, 500, dNormLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, dBNLearnRate, dBNLearnRate);
    AddLayer<NetLayerAct>(net_core, NEUNET_SIGMOID);
    // F6
    AddLayer<NetLayerFC>(net_core, 10, dNormLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, dBNLearnRate, dBNLearnRate);
    AddLayer<NetLayerAct>(net_core, NEUNET_SOFTMAX);
    // mnist 數據集根目錄
    // mnist root directory
    std::string root = "...\\MNIST\\";
    // 加載訓練和測試集
    // load train & test dataset
    mnist train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
    test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());
    // 訓練批計數
    // batch count of train
    auto trn_bat_cnt = train.element_count / net_core.iTrnBatSz;
    /* 初始化數據集訓練形狀
     * initialize train set shape
     * 訓練批計數 batch count of train
     * trn_bat_cnt
     * 輸入矩陣行列計數 line & column count of input matrix
     * train.element_line_count, train.element_column_count
     * 通道數 channel count
     * 1
     */
    Shape(net_core, trn_bat_cnt, train.element_line_count, train.element_column_count, 1);
    /* 訓練測試綫程
     * train & test thread
     * 訓練數據標簽 train data & label
     * train.elem, train.lbl
     * 測試數據標簽 test data & label
     * test.elem, test.lbl
     * 標簽類型數 type amount of label
     * mnist_orgn_size
     * 訓練批計數 batch count of train
     * trn_bat_cnt
     * 測試批計數 batch count of test
     * test.element_count / net_core.iTstBatSz
     */
    TrainTestThread(net_core, train.elem, train.lbl, train.data_idx, test.elem, test.lbl, mnist_orgn_size, trn_bat_cnt, (test.element_count / net_core.iTstBatSz));
    /* 數據打印綫程
     * data showing thread
     * 訓練批計數 batch count of train
     * trn_bat_cnt
     * 測試集數據計數 test data count
     * test.element_count
     */
    DataShowThread(net_core, trn_bat_cnt, test.element_count);
    // 結束計時
    // end timing
    auto chrono_end = NEUNET_CHRONO_TIME_POINT;
    cout << '\n' << (chrono_end - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}
```

[<< 返回 Back](cover.md)

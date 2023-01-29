# 使用基礎函數搭建 ANN<br>Build ANN with base functions

示例將使用 LeNet-5 搭建一個簡易的 ANN 框架，其步驟為\
Example of building a simple ANN structure would use LeNet-5. Process would be

- [必要的頭文件Necessary head file](#必要的頭文件necessary-head-file)
- [全局變量Global variable](#全局變量global-variable)
- [前向傳播Forward propagation](#前向傳播forward-propagation)
- [反向傳播Backward propagation](#反向傳播backward-propagation)
- [訓練Train](#訓練train)
- [測試Test](#測試test)

在這個部分，將嘗試給出多綫程思路。源文件是 [main_fn.cpp](../../main_fn.cpp)\
In this section, it would try to give the multi-thread programming idea. Source code is [main_fn.cpp](../../main_fn.cpp)

## 必要的頭文件<br>Necessary head file

在 LeNet-5 網絡結構中，包含了捲積、池化以及全連接計算，在訓練和推導的時候需要使用多綫程編程，因此源代碼應該看起來像\
In LeNet-5 network structure, it includes convolution, pooling, fully connection calculation and multi-thread would be used to training and deducing, so that the source code should be seem like

```c++
// file main_fn.cpp
#pragma once
// I/O 流和 ANN 頭文件
// I/O stream & ANN head file
#include <iostream>
#include "neunet"
// 内置 mnist 數據集 I/O
// built-in I/O for mnist dataset
#include "dataset"
// 主函數
// Main function
int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.";
    cout << endl;
    /* 其他源碼 Other source codes */
    return EXIT_SUCCESS;
}
```

由於頭文件 `neunet` 包含了一切所需的工具，因此引入這一個頭文件即可。命名空間為\
Since head file `neunet` includes all needed tool, including this head file is just fine. Namespace is

```c++
using namespace neunet::fn_app;
```

## 全局變量<br>Global variable

在神經網絡的信號傳播中，需要網絡層數據結構存儲臨時數據。因此，根據 LeNet-5 網絡結結構，可以創建網絡層數據結構。\
Signal propagating in netwrok need network layer data structure to store temporary data. Therefore, it need to create several corresponding network data structure according to LeNet-5 structure.

```c++
// 層類型編碼
// layer type code
constexpr uint64_t lyr_null = 0,
                   lyr_act  = 1,
                   lyr_fc   = 2,
                   lyr_conv = 3,
                   lyr_pool = 4,
                   lyr_bn   = 5,
                   lyr_flat = 6;
```

這是網絡層使用的識別編碼。\
These are the codes for network layer recognition

### `layer_base`

```c++
// 層基類
// layer base
struct layer_base {
    // 類型編碼
    // type code
    uint64_t type = 0;
    // virtual function for polymorphism
    // 多態虛函數
    virtual uint64_t __type__() { return type; }
};
typedef std::shared_ptr<layer_base> layer_base_ptr;
```

以上是層基類以及指針的定義。\
The definition of layer base data structure and its pointer is above.

### `layer_derive`

```c++
struct layer_derive : virtual layer_base {
    // 批處理同步計數
    // Synchronous counter of batch process 
    std::atomic_uint64_t batch_size_cnt = 0;
    
    // 輸入矩陣集合，臨時數據
    // Input matrix set, temporary data
    net_set<vect> input;
};
```

派生層用於記錄同步任務數以及輸入矩陣的臨時數據。\
Derivative layer is used to count synchronous tasks and temporary data of input matrix.

### `layer_weight`

```c++
struct layer_weight : virtual layer_base {
    // 權重更新的學習率
    // learn rate for weight updating
    long double learn_rate   = .0,
    // 僞隨機數生成區間的端點
    // endpoints of pseudo random number generation interval
                rand_fst_rng = .0,
                rand_snd_rng = .0;
    // 僞隨機數的生成精度
    // Generation accuracy of pseudo random number
    uint64_t rand_acc = 0;
    // 權重，權重的轉置以及 Nesterov 動量權重
    // Weight, Transposition of the weight and Nesterov momentum weight
    vect weight,
         weight_tp,
         weight_nv;
    // 批處理權重梯度集合
    // weight gradient set of batch process
    net_set<vect> weight_grad;
    // AdaDelta 和 Nada 梯度下降數據結構
    // AdaDelta & Nada GD data structure
    ada_delta<long double>    delta;
    ada_nesterov<long double> nesterov;
};
typedef std::shared_ptr<layer_weight> layer_weight_ptr;
```

用於保存帶權重計算數據的層數據結構，包括權重初始化和權重更新。\
Layer data structure for saving the data of weight processing, including weight initialization and weight update.

```c++
// 更新權重
// update weight
void layer_update(layer_weight_ptr src) {
    // 獲取梯度集合的期望
    // get the expectation of weight set
    auto grad = matrix::vect_sum(src->weight_grad).elem_wise_opt(src->weight_grad.length, MATRIX_ELEM_DIV);
    if (src->learn_rate) {
        // 如果有學習率，使用 Nada 梯度下降，獲取動量值
        // learn rate exists, use Nada GD to get the momentum value
        src->weight   -= src->nesterov.momentum(grad, src->learn_rate);
        // 獲取 nesterov 權重
        // get nesterov weight
        src->weight_nv = src->nesterov.weight(src->weight);
        // 權重轉置
        // weight transposition
        src->weight_tp = src->weight_nv.transpose;
    } else {
        // 如果沒有學習率，使用 AdaDelta 梯度下降獲取 delta 值
        // learn rate does not exist, use AdaDelta GD to get the adadelta value
        src->weight   -= src->delta.delta(grad);
        // 權重轉置
        // weight transposition 
        src->weight_tp = src->weight.transpose;
    }
}
```

### `layer_dim`

```c++
struct layer_dim : virtual layer_base {
    // 輸出矩陣的行計數
    // line count of output matrix
    uint64_t out_ln_cnt = 0;
};
```

保存輸出向量維度的層。\
Layer for output vector dimension saving.

### `layer_flat`

```c++
struct layer_flat : layer_dim {
    // 輸入矩陣元素計數
    // Input matrix element count
    uint64_t in_elem_cnt = 0,
    // 通道計數
    // Channel count
             chann_cnt   = 0;
};
typedef std::shared_ptr<layer_flat> layer_flat_ptr;
```

攤平操作其實就是一個矩陣元素排列變換的過程，改變矩陣形狀。\
The flatting opeation is actually a process of changing matrix element arrangement, change the shape of matrix.

$$M_{(m,n)}\implies M'_{(m\times n,1)}$$
$$\frac{dL}{dM_{(m,n)}}\impliedby\frac{dL}{dM'_{(m\times n,1)}}$$

```c++
// 初始函數
// Initialization function
void layer_init(layer_flat_ptr src) {
    // 設定當前層類型為“展平”
    // Set current layer type as "flatting"
    src->type = lyr_flat;
}
// 設定臨時數據矩陣的形狀
// Set the shape of temporary data matrix
void layer_shape(layer_flat_ptr src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t &chann_cnt) {
    // 通道數
    src->chann_cnt   = chann_cnt;
    // 輸入矩陣元素數
    src->in_elem_cnt = in_ln_cnt * in_col_cnt;
    // 輸出矩陣的行計數
    src->out_ln_cnt  = chann_cnt * src->in_elem_cnt;
    // 更改輸入矩陣行列計數以及通道數參數，用於之後網絡層數據矩陣的形狀設定
    // Alter the parameter of input matrix line & column count and channel count, it used for shape setting of successor layer data matrix
    in_ln_cnt        = src->out_ln_cnt;
    in_col_cnt       = 1;
    chann_cnt        = 1;
    // 展平后，矩陣的列和通道數都爲 1，行數為矩陣的元素數
    // After flatting, the column and channel count are 1, line count of matrix is element count of the matrix before flatting.
}
// 前向傳播，更改矩陣形狀即可。
// Forward peopagation, reshaping the matrix is just fine.
void layer_forward(layer_flat_ptr src, vect &input) { input = input.reshape(src->out_ln_cnt, 1); }
// 反向傳播，前向傳播的你操作，操作對象為回傳梯度。
// backward propagation, the inverse operation of forward propagation, the operation object is the backward gradient.
void layer_backward(layer_flat_ptr src, vect &grad) { grad = grad.reshape(src->in_elem_cnt, src->chann_cnt); }
// 數據推導，與前向傳播相同
// data deducing, same as forward propagation
void layer_deduce(layer_flat_ptr src, vect &input) { layer_forward(src, input); }
```

### `layer_act`

```c++
struct layer_act : layer_derive {
    // 激活函數類型編碼
    // activation function type code
    uint64_t act_fn_type = 0;
};
typedef std::shared_ptr<layer_act> layer_act_ptr;
```

激活層，使用激活函數。\
Activation layer for activation function using.

$$Y=\sigma(Z)$$
$$\frac{dL}{dZ}=\frac{dL}{dY}\odot\sigma'(Z)$$

```c++
// 指定激活函數類型
// speicify the activation function type
void layer_init(layer_act_ptr src, uint64_t act_fn_type = 0) {
    src->type        = lyr_act;
    src->act_fn_type = act_fn_type;
}
// 以批大小初始化輸入矩陣集合
// initialize input matrix set with batch size
void layer_shape(layer_act_ptr src, uint64_t batch_size) { src->input.init(batch_size, false); }
// 前向傳播，指定當前輸入矩陣以及同步任務的索引
// forward propagation, specify current matrix and the index of synchronous task
void layer_forward(layer_act_ptr src, vect &input, uint64_t bat_sz_idx) {
    // 用於反向傳播的臨時輸入矩陣值
    // the temporary input matrix value for backward propagation
    src->input[bat_sz_idx] = std::move(input);
    // activate
    switch(src->act_fn_type) {
    case NEUNET_SIGMOID: input = sigmoid(src->input[bat_sz_idx]); break;
    case NEUNET_RELU: input = ReLU(src->input[bat_sz_idx]); break;
    case NEUNET_ARELU_LOSS:
    case NEUNET_ARELU: input = AReLU(src->input[bat_sz_idx]); break;
    case NEUNET_SOFTMAX: input = softmax(src->input[bat_sz_idx]); break;
    default: input = src->input[bat_sz_idx]; break;
    }
}
// 反向傳播，需要損失梯度，同步任務索引以及標簽矩陣
// backward propagation, gradient from loss, synchronous task indexx and label matrix are needed
void layer_backward(layer_act_ptr src, vect &grad, uint64_t bat_sz_idx, const vect &orgn) {
    switch (src->act_fn_type) {
    case NEUNET_SIGMOID: grad = sigmoid_dv(src->input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_RELU: grad = ReLU_dv(src->input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_ARELU: grad = AReLU_dv(src->input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_ARELU_LOSS: grad = AReLU_loss_grad(src->input[bat_sz_idx], grad, orgn); break;
    case NEUNET_SOFTMAX: grad = softmax_cec_grad(grad, orgn); break;
    default: return;
    }
}
// 推測，參考前向傳播
// deduce, please refer to forward propagation
void layer_deduce(layer_act_ptr src, vect &input) {
    switch(src->act_fn_type) {
    case NEUNET_SIGMOID: input = sigmoid(input); break;
    case NEUNET_RELU: input = ReLU(input); break;
    case NEUNET_ARELU_LOSS:
    case NEUNET_ARELU: input = AReLU(input); break;
    case NEUNET_SOFTMAX: input = softmax(input); break;
    default: return;
    }
}
```

### `layer_fc`

```c++
struct layer_fc : layer_derive, layer_dim, layer_weight {};
typedef std::shared_ptr<layer_fc> layer_fc_ptr;
```

全連接層，使用矩陣乘法。更多請參閲 [`Output`](FC/Output.md)，[`GradLossToInput`](FC/GradLossToInput.md) 和 [`GradLossToWeight`](FC/GradLossToWeight.md)。\
Fully connection layer, using matrix multiplication. Please refer to [`Output`](FC/Output.md)， [`GradLossToInput`](FC/GradLossToInput.md) and [`GradLossToWeight`](FC/GradLossToWeight.md) for more details.

```c++
/* 初始化參數有
 * parameters for initializing are
 * 輸出向量行數
 * line count of output vector
 * 更新權重的學習率
 * learn rate for weight updating
 * 用於初始化僞隨機數的區間端點和精度
 * interval endpoints and accuracy of pseudo random number generation
 */
void layer_init(layer_fc_ptr src, uint64_t out_ln_cnt, long double learn_rate = 0, long double rand_fst_rng = 0, long double rand_snd_rng = 0, uint64_t rand_acc = 0) {
    src->type         = lyr_fc;
    src->learn_rate   = learn_rate;
    src->out_ln_cnt   = out_ln_cnt;
    src->rand_fst_rng = rand_fst_rng;
    src->rand_snd_rng = rand_snd_rng;
    src->rand_acc     = rand_acc;
}
/* 數據集初始化形狀
 * initializing shape of dataset
 * 輸入向量的行數
 * line count of input vector
 * 批大小
 * batch size
 */
void layer_shape(layer_fc_ptr src, uint64_t &in_ln_cnt, uint64_t batch_size) {
    // 初始化權重
    // initialize weight
    src->weight = fc::InitWeight(in_ln_cnt, src->out_ln_cnt, src->rand_fst_rng, src->rand_snd_rng, src->rand_acc);
    if (src->learn_rate) {
        // 如果制定了學習率則初始化 nesterov 權重
        // initialize nesterov weight if learn rate is specified
        src->weight_nv = src->nesterov.weight(src->weight);
    // 獲取權重轉置
    // get transposition of weight
        src->weight_tp = src->weight_nv.transpose;
    } else src->weight_tp = src->weight.transpose;
    // 初始化權重梯度集合
    // initialize weight gradient set
    src->weight_grad.init(batch_size, false);
    // 初始化輸入集合
    // initialize input set
    src->input.init(batch_size, false);
    // 下一層輸入向量的行計數
    // input vector line count for next layer
    in_ln_cnt = src->out_ln_cnt;
}
/* 前向傳播參數
 * parameter of forward propagation
 * 輸入矩陣
 * input matrix
 * 同步任務索引
 * synchronous task index
 */
void layer_forward(layer_fc_ptr src, vect &input, uint64_t bat_sz_idx) {
    using namespace fc;
    // 輸入矩陣移入層輸入矩陣集合對應同步任務索引中
    // move input matrix into the corresponding synchronous task index of layer input set
    src->input[bat_sz_idx] = std::move(input);
    // 如果學習率存在，使用 nesterov 權重
    // use nesterov weight for learn rate exisiting
    if (src->learn_rate) input = fc::Output(src->input[bat_sz_idx], src->weight_nv);
    // 否則使用正常權重
    // otherwise normal weight
    else input = fc::Output(src->input[bat_sz_idx], src->weight);
}
/* 反向傳播參數
 * parameter of back propagation
 * 回傳梯度
 * gradient of backward propagation
 * 同步任務索引
 * synchronous task index
 */
void layer_backward(layer_fc_ptr src, vect &grad, uint64_t bat_sz_idx) {
    // 當前索引的權重梯度
    // weight gradient of current index
    src->weight_grad[bat_sz_idx] = fc::GradLossToWeight(grad, src->input[bat_sz_idx].transpose);
    // 輸入矩陣的梯度
    // gradient of input matrix
    grad                         = fc::GradLossToInput(grad, src->weight_tp);
    if (++src->batch_size_cnt == src->input.length) {
        src->batch_size_cnt = 0;
        // 最後一個完成任務的索引對權重進行更新
        // Update weight at last index after task completed
        layer_update(src);
    }
}
// 推測
// deduce
void layer_deduce(layer_fc_ptr src, vect &input) { input = fc::Output(input, src->weight); }
```

### `layer_caffe`

```c++
struct layer_caffe : virtual layer_flat {
    uint64_t in_ln_cnt      = 0, // 輸入矩陣行計數 line count of input matrix
             in_col_cnt     = 0, // 輸入矩陣列計數 column count of input matrix
             out_col_cnt    = 0, // 輸出矩陣列計數 column count of output matrix
             ln_stride      = 0, // 行向采樣步幅 line direction stride of down sampling
             col_stride     = 0, // 列向采樣步幅 column direction stride down sampling
             ln_dilate      = 0, // 采樣行擴張值 down sampling dilation in line direction
             col_dilate     = 0, // 采樣列擴張值 down sampling dilation in column direction
             filter_ln_cnt  = 0, // 采樣過濾行計數 line count of down sampling filter
             filter_col_cnt = 0, // 采樣過濾列計數 column count of down sampling filter
             caffe_ln_cnt   = 0, // Caffe 矩陣行計數 line count of Caffe matrix
             caffe_col_cnt  = 0; // Caffe 矩陣列計數 column count of Caffe matrix
    // Caffe 矩陣元素索引數據 element index data of Caffe matrix
    net_set<uint64_t> caffe_data;
};
```

轉換 Im2Col 矩陣到 Caffe 矩陣。\
Transform Im2Col matrix to Caffe matrix.

### `layer_conv`

```c++
struct layer_conv : layer_derive, layer_weight, layer_caffe {
    // 核數量
    // kernel quantity
    uint64_t kernel_qty = 0;
};
typedef std::shared_ptr<layer_conv> layer_conv_ptr;
```

捲積，使用 Caffe 轉換和矩陣乘法。\
Convolution, using Caffe matrix transformation and matrix multiplication.

請參閲 [捲積定義](Conv/cover.md)，[`CaffeTransform`](Conv/CaffeTransform.md) 和 [`InitKernel`](Conv/InitKernel.md) 獲取更多。\
Please refer to [convolution definition](Conv/cover.md), [`CaffeTransform`](Conv/CaffeTransform.md) and [`InitKernel`](Conv/InitKernel.md) for more details.

```c++
/* 初始化層參數
 * parameter of layer initialization
 * 核數量
 * kernel quantity
 * 核行計數
 * line count of kernel
 * 核列計數
 * column count of kernel
 * 捲積行向步幅
 * stride in line direction of convolution
 * 捲積列向步幅
 * stride in column direction of convolution
 * 兩行閒捲積擴張值
 * dilation bewteen two lines of convolution
 * 兩列閒捲積擴張值
 * dilation between two columns of convolution
 * 用於核更新的學習率
 * learn rate for kernel updating
 * 用於初始化僞隨機數的區間端點和精度
 * interval endpoints and accuracy for pseudo random number generation 
 */
void layer_init(layer_conv_ptr src, uint64_t kernel_qty, uint64_t kernel_ln_cnt, uint64_t kernel_col_cnt, uint64_t ln_stride, uint64_t col_stride, uint64_t ln_dilate = 0, uint64_t col_dilate = 0, long double learn_rate = 0, long double rand_fst_rng = 0, long double rand_snd_rng = 0, uint64_t rand_acc = 0) {
    src->type           = lyr_conv;
    src->kernel_qty     = kernel_qty;
    src->filter_ln_cnt  = kernel_ln_cnt;
    src->filter_col_cnt = kernel_col_cnt;
    src->ln_stride      = ln_stride;
    src->col_stride     = col_stride;
    src->ln_dilate      = ln_dilate;
    src->col_dilate     = col_dilate;
    src->learn_rate     = learn_rate;
    src->rand_fst_rng   = rand_fst_rng;
    src->rand_snd_rng   = rand_snd_rng;
    src->rand_acc       = rand_acc;
}
/* 數據集形狀初始化參數
 * parameter of dataset shape initialization
 * 輸入矩陣行計數
 * input count of input matrix
 * 輸入矩陣列計數
 * column count of input matrix
 * 通道數
 * channel count
 * 同步任務批大小
 * batch size of synchronous task
 */
void layer_shape(layer_conv_ptr src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t &chann_cnt, uint64_t batch_size) {
    // 初始化輸入和權重矩陣集合
    // initialize input and weight matrix set
    src->input.init(batch_size, false);
    src->weight_grad.init(batch_size, false);
    // 初始化形狀
    // initialize shape
    src->in_ln_cnt   = in_ln_cnt;
    src->in_col_cnt  = in_col_cnt;
    src->in_elem_cnt = in_ln_cnt * in_col_cnt;
    src->chann_cnt   = chann_cnt;
    // 獲取 Caffe 矩陣數據
    // get Caffe matrix data
    src->caffe_data  = conv::CaffeTransformData(chann_cnt, src->caffe_ln_cnt, src->caffe_col_cnt, in_ln_cnt, in_col_cnt, src->out_ln_cnt, src->out_col_cnt, src->filter_ln_cnt, src->filter_col_cnt, src->ln_stride, src->col_stride, src->ln_dilate, src->col_dilate);
    // 初始化捲積核
    // initialize convolution kernels
    if (!src->weight.verify) src->weight = conv::InitKernel(src->kernel_qty, chann_cnt, src->filter_ln_cnt, src->filter_col_cnt, src->rand_fst_rng, src->rand_snd_rng, src->rand_acc);
    if (src->learn_rate) {
        // 學習率存在，使用 nesterov 權重
        // learn rate exists, use nesterov weight
        src->weight_nv = src->nesterov.weight(src->weight);
    // 獲取權重轉置
    // get weight transposition
        src->weight_tp = src->weight_nv.transpose;
    } else src->weight_tp = src->weight.transpose;
    // 後續層的形狀參數
    // shape parameter of successor layer
    in_ln_cnt  = src->out_ln_cnt;
    in_col_cnt = src->out_col_cnt;
    chann_cnt  = src->kernel_qty;
}
// 前向傳播
//forward propagation
void layer_forward(layer_conv_ptr src, vect &input, uint64_t bat_sz_idx) {
    // 輸入矩陣集合對應索引処存入輸入矩陣的 Caffe 矩陣
    // save the Caffe matrix of input matrix at the corresponding input matrix set index
    src->input[bat_sz_idx] = conv::CaffeTransform(input, src->caffe_data, src->caffe_ln_cnt, src->caffe_col_cnt);
    // 捲積
    // convolution
    if (src->learn_rate) input = conv::Conv(src->input[bat_sz_idx], src->weight_nv);
    else input = conv::Conv(src->input[bat_sz_idx], src->weight);
}
// 反向傳播
// backward propagation
void layer_backward(layer_conv_ptr src, vect &grad, uint64_t bat_sz_idx) {
    // 獲取核梯度
    // get kernel gradient
    src->weight_grad[bat_sz_idx] = conv::GradLossToConvKernal(grad, src->input[bat_sz_idx].transpose);
    // 反傳梯度
    // gradient backward propagation
    grad                         = conv::CaffeTransform(conv::GradLossToConvCaffeInput(grad, src->weight_tp), src->caffe_data, src->in_elem_cnt, src->chann_cnt, true);
    // 更新
    // update
    if (++src->batch_size_cnt == src->input.length) {
        // 最後索引
        // last index
        src->batch_size_cnt = 0;
        layer_update(src);
    }
}
// 推測
// deduce
void layer_deduce(layer_conv_ptr src, vect &input) { input = conv::Conv(conv::CaffeTransform(input, src->caffe_data, src->caffe_ln_cnt, src->caffe_col_cnt), src->weight); }
```

### `layer_pool`

```c++
struct layer_pool : layer_caffe {
    // 池化類型
    // pooling type
    uint64_t pool_type       = 0,
    // 過濾器元素計數
    // element count of filter
             filter_elem_cnt = 0;
    // 最大池化矩陣元素坐標集合
    // matrix element position set of max pooling
    net_set<net_set<net_list<matrix::pos>>> max_pool_pos;
};
typedef std::shared_ptr<layer_pool> layer_pool_ptr;
```

池化層，更多請參閲[池化定義](Conv/cover.md)。\
Pooling layer, please refer to [pooling definition](Conv/cover.md).

```c++
/* 初始化層參數
 * parameter of layer initialization
 * 池化類型
 * pooling type
 * 過濾器行計數
 * line count of filter
 * 過濾器列計數
 * column count of filter
 * 池化行向步幅
 * stride in line direction of pooling
 * 池化列向步幅
 * stride in column direction of pooling
 * 兩行閒池化擴張值
 * dilation bewteen two lines of pooling
 * 兩列閒池化擴張值
 * dilation between two columns of pooling
 * 用於核更新的學習率
 */
void layer_init(layer_pool_ptr src, uint64_t pool_type, uint64_t filter_ln_cnt, uint64_t filter_col_cnt, uint64_t ln_stride, uint64_t col_stride, uint64_t ln_dilate = 0, uint64_t col_dilate = 0) {
    src->type            = lyr_pool;
    src->pool_type       = pool_type;
    src->filter_ln_cnt   = filter_ln_cnt;
    src->filter_col_cnt  = filter_col_cnt;
    src->filter_elem_cnt = filter_ln_cnt * filter_col_cnt;
    src->ln_stride       = ln_stride;
    src->col_stride      = col_stride;
    src->ln_dilate       = ln_dilate;
    src->col_dilate      = col_dilate;
}
/* 數據集形狀初始化參數
 * parameter of dataset shape initialization
 * 輸入矩陣行計數
 * input count of input matrix
 * 輸入矩陣列計數
 * column count of input matrix
 * 通道數
 * channel count
 * 同步任務批大小
 * batch size of synchronous task
 */
void layer_shape(layer_pool_ptr src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t chann_cnt, uint64_t batch_size) {
    src->in_ln_cnt   = in_ln_cnt;
    src->in_col_cnt  = in_col_cnt;
    src->in_elem_cnt = in_ln_cnt * in_col_cnt;
    src->chann_cnt   = chann_cnt;
    if (src->pool_type == NEUNET_POOL_GAG) {
        src->out_ln_cnt  = 1;
        src->out_col_cnt = 1;
    } else {
        src->caffe_data = conv::CaffeTransformData(chann_cnt, src->caffe_ln_cnt, src->caffe_col_cnt, in_ln_cnt, in_col_cnt, src->out_ln_cnt, src->out_col_cnt, src->filter_ln_cnt, src->filter_col_cnt, src->ln_stride, src->col_stride, src->ln_dilate, src->col_dilate);
        src->max_pool_pos.init(batch_size, false);
    }
    in_ln_cnt  = src->out_ln_cnt;
    in_col_cnt = src->out_col_cnt;
}
// 前向傳播
// forward propagation
void layer_forward(layer_pool_ptr src, vect &input, uint64_t bat_sz_idx, bool trn_flag = true) { switch (src->pool_type) {
    // 全局平均池化
    // global average pooling
    case NEUNET_POOL_GAG: input = conv::PoolGlbAvg(input); break;
    // 平均池化
    // average pooling
    case NEUNET_POOL_AVG: input = conv::PoolAvg(input, src->caffe_data, src->filter_elem_cnt, src->caffe_ln_cnt); break;
    // 最大池化
    // max pooling
    case NEUNET_POOL_MAX: input = conv::PoolMax(input, src->caffe_data, src->filter_elem_cnt, src->caffe_ln_cnt, src->max_pool_pos[bat_sz_idx], trn_flag); break;
    default: break;
} }
// 反向傳播
// backward propagation
void layer_backward(layer_pool_ptr src, vect &grad, uint64_t bat_sz_idx) { switch (src->pool_type) {
    // 全局平均池化
    // global average pooling
    case NEUNET_POOL_GAG: grad = conv::GradLossToPoolGlbAvgChann(grad, src->in_elem_cnt); break;
    // 平均池化
    // average pooling
    case NEUNET_POOL_AVG: grad = conv::GradLossToPoolAvgChann(grad, src->caffe_data, src->filter_elem_cnt, src->in_elem_cnt); break;
    // 最大池化
    // max pooling
    case NEUNET_POOL_MAX: grad = conv::GradLossToPoolMaxChann(grad, src->in_elem_cnt, src->max_pool_pos[bat_sz_idx]); break;
    default: break;
} }
// 推導
// deduce
void layer_deduce(layer_pool_ptr src, vect &input) { layer_forward(src, input, 0, false); }
```

### `layer_bn`

```c++
struct layer_bn : layer_derive {
    // 移動平均衰退率
    // moving average decay rate
    long double mov_avg_decay    = 0.9,
                // shift 與 scale 的學習率
                // learn rate of shift and scale
                beta_learn_rate  = 0,
                gamma_learn_rate = 0;
    // shift & scale
    vect beta,
         gamma,
         // 梯度
         // gradient
         beta_grad,
         gamma_grad,
         // nesterov
         beta_nv,
         gamma_nv;
    // 輸入梯度批合集
    // input gradient batch set
    net_set<vect> input_grad;
    // 批歸一化數據
    // BN data
    BNData<long double> BN_data;
    // shift 和 scale 向量的 Nada 梯度下降
    // Nada GD of shift and scale vector
    ada_nesterov<long double> nesterov_beta,
                              nesterov_gamma;
    // shift 和 scale 向量的 AdaDelta 梯度下降
    // AdaDelta GD of shift and scale vector
    ada_delta<long double> delta_beta,
                           delta_gamma;
    // 同步任務在 BN 層的異步控制器
    // asynchronous controller of BN layer in synchronous task
    async::async_controller BN_ctrl;
};
typedef std::shared_ptr<layer_bn> layer_bn_ptr;
```

批歸一化層\
Batch normalization layer

更多請參閲 [`BNTrain`](BN/BNTrain.md)，[`BNGradLossToInputGammaBeta`](BN/BNGradLossToInputGammaBeta.md) 和 [`BNDeduce`](BN/BNDeduce.md) 。\
Please refer to [`BNTrain`](BN/BNTrain.md), [`BNGradLossToInputGammaBeta`](BN/BNGradLossToInputGammaBeta.md) and [`BNDeduce`](BN/BNDeduce.md) for more details.

```c++
/* 初始化 BN 層的參數
 * parameters for BN layer initialization
 * 初始化 shift 和 scale 值
 * initial value of shift and scale
 * shift 和 scale 的學習率
 * learn rate of shift and scale
 * 滑動平均衰減值
 * decay value for moving average value
 */
void layer_init(layer_bn_ptr src, vect &&shift = 0, vect &&scale = 1, long double shift_learn_rate = 0, long double scale_learn_rate = 0, long double mov_avg_decay = 0.9) {
    src->type             = lyr_bn;
    src->beta             = std::move(shift);
    src->gamma            = std::move(scale);
    src->beta_learn_rate  = shift_learn_rate;
    src->gamma_learn_rate = scale_learn_rate;
    src->mov_avg_decay    = mov_avg_decay;
}
/* 數據集形狀初始化
 * dataset shape intialization
 * 通道數
 * channel count
 * 批大小
 * batch size
 * 批數量
 * batch count
 */
void layer_shape(layer_bn_ptr src, uint64_t chann_cnt, uint64_t batch_size, uint64_t batch_cnt) {
    auto beta_v  = src->beta.atom,
         gamma_v = src->gamma.atom;
    src->beta = BNInitBetaGamma(chann_cnt, beta_v);
    src->gamma = BNInitBetaGamma(chann_cnt, gamma_v);
    if (beta_v) src->beta.fill_elem(beta_v);
    if (gamma_v) src->gamma.fill_elem(gamma_v);
    if (src->beta_learn_rate) src->beta_nv = src->nesterov_beta.weight(src->beta);
    if (src->gamma_learn_rate) src->gamma_nv = src->nesterov_gamma.weight(src->gamma);
    src->input.init(batch_size, false);
    src->input_grad.init(batch_size, false);
    BNInitBNData(src->BN_data, batch_size, batch_cnt);
}
// 更新 shift 和 scale
// update shift and scale
void layer_update(layer_bn_ptr src) {
    // shift
    if (src->beta_learn_rate) {
        src->beta   -= src->nesterov_beta.momentum(src->beta_grad, src->beta_learn_rate);
        src->beta_nv = src->nesterov_beta.weight(src->beta);
    } else src->beta -= src->delta_beta.delta(src->beta_grad);
    // scale
    if (src->gamma_learn_rate) {
        src->gamma   -= src->nesterov_gamma.momentum(src->gamma_grad, src->gamma_learn_rate);
        src->gamma_nv = src->nesterov_gamma.weight(src->gamma);
    } else src->gamma -= src->delta_gamma.delta(src->gamma_grad);
}
// 前向傳播
// forward propagation
void layer_forward(layer_bn_ptr src, vect &input, uint64_t bat_sz_idx) {
    // 同步輸入移入輸入矩陣集合對應索引
    // move synchronous input to the corresponding index of input matrix set
    src->input[bat_sz_idx] = std::move(input);
    if (++src->batch_size_cnt == src->input.length) {
        // 最後到達的同步任務執行 BN
        // last arriving synchronous task executes BN
        src->input_grad = BNTrain(src->BN_data, src->input, (src->beta_learn_rate ? src->beta_nv : src->beta), (src->gamma_learn_rate ? src->gamma_nv : src->gamma));
        // BN 后喚醒所有休眠綫程
        // wake up all dormant threads after BN
        src->BN_ctrl.thread_wake_all();
    // 其他任務休眠
    // other tasks sleep
    } else src->BN_ctrl.thread_sleep();
    // 按索引獲取前向傳播輸出
    // get forward propaagtion output according to the index
    input = std::move(src->input_grad[bat_sz_idx]);
}
// 反向傳播
// backward propagation
void layer_backward(layer_bn_ptr src, vect &grad, uint64_t bat_sz_idx) {
    // 同步梯度移入輸入矩陣梯度集合對應索引
    // move synchronous gradient to the corresponding index of input gradient matrix set
    src->input_grad[bat_sz_idx] = std::move(grad);
    // 非最後到達任務則睡眠
    // Sleep if it not the last arriving task
    if (--src->batch_size_cnt) src->BN_ctrl.thread_sleep();
    else {
        // 最後到達的任務執行 BN 反向傳播
        // last arriving task executes BN backward propagation
        src->input_grad = BNGradLossToInputGammaBeta(src->BN_data, src->gamma_grad, src->beta_grad, src->input_grad, (src->gamma_learn_rate ? src->gamma_nv : src->gamma), src->mov_avg_decay);
        // 醒所有休眠綫程
        // wake up all dormant threads
        src->BN_ctrl.thread_wake_all();
        // update
        layer_update(src);
    }
    // 獲取對應索引的梯度
    // get gradient at corresponding index
    grad = std::move(src->input_grad[bat_sz_idx]);
}
// 推測
// deduce
void layer_deduce(layer_bn_ptr src, vect &input) { input = BNDeduce(src->BN_data, input, src->beta, src->gamma); }
```

### 變量函數 Variabale function

封裝指針多態函數。\
Encapsulate pointer polymorphic function

```c++
// 派生轉換
// layer derivative transformation
template<typename layer_derive_t> std::shared_ptr<layer_derive_t> layer_cast(const layer_base_ptr src) { return std::dynamic_pointer_cast<layer_derive_t>(src); };
```

定義增添網絡層的函數，通過傳入初始化參數即可完成網絡層的初始化與添加。\
Define an adding network layers function, it can initialize and add network layer to network via initial parameter input.

```c++
template <typename layer_type, typename ... layer_paras> void net_add_layer(net_sequence<layer_base_ptr> &net_lyr, layer_paras &&...args) {
    // 多態檢查
    // polymorphic checking
    static_assert(std::is_same_v<layer_type, layer_act>  ||
                  std::is_same_v<layer_type, layer_fc>   ||
                  std::is_same_v<layer_type, layer_flat> ||
                  std::is_same_v<layer_type, layer_conv> ||
                  std::is_same_v<layer_type, layer_pool> ||
                  std::is_same_v<layer_type, layer_bn>,
                  "Should derive from data structure <layer_base>");
    auto curr_lyr_idx = net_lyr.length;
    // 網絡末尾添加層
    // add layer to the tail of network
    net_lyr.emplace_back(std::make_shared<layer_type>());
    // 初始化層
    // initialize layer
    layer_init(layer_cast<layer_type>(net_lyr[curr_lyr_idx]),
               // 轉發初始化參數
               // forward initial parameters
               std::forward<layer_paras>(args)...);
}
```

關於可變序列，請參閲 [`net_sequence`](../DataStructure/net_sequence/cover.md)。\
Please refer to [`net_sequence`](../DataStructure/net_sequence/cover.md) for more details about dynamic sequence.

定義數據集形狀初始化函數，可以初始化網絡所有層的形狀。\
Define dataset shape initializing function, it can initialize all layers' shape.

```c++
/* 數據集屬性
 * dataset property
 * 輸入矩陣行列計數
 * line & column count of input matrix
 * 通道計數
 * channel count
 * 批大小和數量
 * batch size & count
 */
void net_shape_init(net_sequence<layer_base_ptr> &net_lyr, uint64_t in_ln_cnt, uint64_t in_col_cnt, uint64_t chann_cnt, uint64_t batch_size, uint64_t batch_cnt) {
    for (auto i = 0ull; i < net_lyr.length; ++i) switch (net_lyr[i]->type) {
    case lyr_act: layer_shape(layer_cast<layer_act>(net_lyr[i]), batch_size); break;
    case lyr_fc: layer_shape(layer_cast<layer_fc>(net_lyr[i]), in_ln_cnt, batch_size); break;
    case lyr_conv: layer_shape(layer_cast<layer_conv>(net_lyr[i]), in_ln_cnt, in_col_cnt, chann_cnt, batch_size); break;
    case lyr_flat: layer_shape(layer_cast<layer_flat>(net_lyr[i]), in_ln_cnt, in_col_cnt, chann_cnt); break;
    case lyr_pool: layer_shape(layer_cast<layer_pool>(net_lyr[i]), in_ln_cnt, in_col_cnt, chann_cnt, batch_size); break;
    case lyr_bn: layer_shape(layer_cast<layer_bn>(net_lyr[i]), chann_cnt, batch_size, batch_cnt); break;
    default: break;
    }
}
```

## 前向傳播<br>Forward propagation

每一層調用對應的前向傳播函數。\
Every layer calls the corresponding function of forward propagation

```c++
void net_forward(net_sequence<layer_base_ptr> &net_lyr, vect &input, uint64_t bat_sz_idx) {
    for (auto i = 0ull; i < net_lyr.length; ++i) switch (net_lyr[i]->type) {
    case lyr_act: layer_forward(layer_cast<layer_act>(net_lyr[i]), input, bat_sz_idx); break;
    case lyr_fc: layer_forward(layer_cast<layer_fc>(net_lyr[i]), input, bat_sz_idx); break;
    case lyr_conv: layer_forward(layer_cast<layer_conv>(net_lyr[i]), input, bat_sz_idx); break;
    case lyr_flat: layer_forward(layer_cast<layer_flat>(net_lyr[i]), input); break;
    case lyr_pool: layer_forward(layer_cast<layer_pool>(net_lyr[i]), input, bat_sz_idx); break;
    case lyr_bn: layer_forward(layer_cast<layer_bn>(net_lyr[i]), input, bat_sz_idx); break;
    default: break;
    }
}
```

推測前向傳播\
Deducing forward propagation

```c++
void net_deduce(net_sequence<layer_base_ptr> &net_lyr, vect &input) {
    for (auto i = 0ull; i < net_lyr.length; ++i) switch (net_lyr[i]->type) {
    case lyr_act: layer_deduce(layer_cast<layer_act>(net_lyr[i]), input); break;
    case lyr_fc: layer_deduce(layer_cast<layer_fc>(net_lyr[i]), input); break;
    case lyr_conv: layer_deduce(layer_cast<layer_conv>(net_lyr[i]), input); break;
    case lyr_flat: layer_deduce(layer_cast<layer_flat>(net_lyr[i]), input); break;
    case lyr_pool: layer_deduce(layer_cast<layer_pool>(net_lyr[i]), input); break;
    case lyr_bn: layer_deduce(layer_cast<layer_bn>(net_lyr[i]), input); break;
    default: break;
    }
}
```

## 反向傳播<br>Backward propagation

每一層調用對應的反向傳播函數。\
Every layer calls the corresponding function of backward propagation

```c++
void net_backward(net_sequence<layer_base_ptr> &net_lyr, vect &grad, uint64_t bat_sz_idx, const vect &orgn) {
    for (auto idx = net_lyr.length; idx; --idx) {
        auto i = idx - 1;
        switch (net_lyr[i]->type) {
        case lyr_act: layer_backward(layer_cast<layer_act>(net_lyr[i]), grad, bat_sz_idx, orgn); break;
        case lyr_fc: layer_backward(layer_cast<layer_fc>(net_lyr[i]), grad, bat_sz_idx); break;
        case lyr_conv: layer_backward(layer_cast<layer_conv>(net_lyr[i]), grad, bat_sz_idx); break;
        case lyr_flat: layer_backward(layer_cast<layer_flat>(net_lyr[i]), grad); break;
        case lyr_pool: layer_backward(layer_cast<layer_pool>(net_lyr[i]), grad, bat_sz_idx); break;
        case lyr_bn: layer_backward(layer_cast<layer_bn>(net_lyr[i]), grad, bat_sz_idx); break;
        default: break;
        }
    }
}
```

## 訓練<br>Train

這個過程默認處於 `main` 函數中。\
This process is included in `main` function.

網絡狀態編碼<br>Network status code|描述<br>Description
-|-
`NEUNET_STAT_NRM`|正常<br>Normal
`NEUNET_STAT_END`|訓練完成<br>Train completed
`NEUNET_STAT_ERR`|錯誤<br>Error

```c++
using namespace std;
using namespace neunet;
using namespace fn_app;
using namespace dataset;

// 學習率
// learn rate
long double learn_rate = .4;
// 訓練和測試的數據批大小
// train & test data batch size
uint64_t trn_bat_sz = 125, tst_bat_sz = 125;
// 訓練精度
// train precision
long double trn_prec = .1;
// 網絡狀態編碼
// network status code
std::atomic_uint64_t net_stat   = NEUNET_STAT_NRM,
// 批大小計數器
// batch size counter
                     bat_sz_cnt = 0,
// 準確度計數
// accuracy counter
                     acc_cnt    = 0,
// 回收率計數
// recall counter
                     rc_cnt     = 0;
// 訓練與測試的精度與回收率隊列
// accuracy & recall count of train & test
net_queue<uint64_t> trn_acc, trn_rc, tst_acc, tst_rc;
// 綫程池
// threads pool
auto pool_sz = trn_bat_sz > tst_bat_sz ? trn_bat_sz : tst_bat_sz;
async_pool pool(pool_sz);
// 訓練與測試的異步控製器
// asynchronous process controller of train & test
async_controller trn_ctrl, tst_ctrl;
// 網絡層 - LeNet-5
// network layer - LeNet-5
net_sequence<layer_base_ptr> net_lyr;
// C0
net_add_layer<layer_conv>(net_lyr, 20, 5, 5, 1, 1, 0, 0, learn_rate);
net_add_layer<layer_bn>(net_lyr, 0, 1, 1e-5l, 1e-5l);
net_add_layer<layer_act>(net_lyr, NEUNET_RELU);
// P1
net_add_layer<layer_pool>(net_lyr, NEUNET_POOL_AVG, 2, 2, 2, 2);
// C2
net_add_layer<layer_conv>(net_lyr, 50, 5, 5, 1, 1, 0, 0, learn_rate);
net_add_layer<layer_bn>(net_lyr, 0, 1, 1e-5l, 1e-5l);
net_add_layer<layer_act>(net_lyr, NEUNET_RELU);
// P3
net_add_layer<layer_pool>(net_lyr, NEUNET_POOL_AVG, 2, 2, 2, 2);
net_add_layer<layer_flat>(net_lyr);
// F4
net_add_layer<layer_fc>(net_lyr, 500, learn_rate);
net_add_layer<layer_bn>(net_lyr, 0, 1, 1e-5l, 1e-5l);
net_add_layer<layer_act>(net_lyr, NEUNET_SIGMOID);
// F5
net_add_layer<layer_fc>(net_lyr, 10, learn_rate);
net_add_layer<layer_act>(net_lyr, NEUNET_SOFTMAX);
    
/* 訓練與測試集，内置數據結構 train & test dataset, built-in data structure "mnist" */

// mnist 數據集根目錄
// root directory of mnist dataset
std::string root = "...\\MNIST\\";
// 加載訓練與測試集
// load train & test dataset
mnist train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());
// 訓練集的批計數
// train batch count
auto trn_bat_cnt = train.element_count / trn_bat_sz;

// mnist 形狀
// mnist shape
net_shape_init(net_lyr, train.element_line_count, train.element_column_count, 1, trn_bat_sz, trn_bat_cnt);

// mnist 訓練和測試
// mnist train & deduce
for (auto i = 0ull; i < pool_sz; ++i) pool.add_task([&net_lyr, &train, &test, &bat_sz_cnt, &net_stat, &trn_ctrl, &tst_ctrl, &acc_cnt, &rc_cnt, &trn_acc, &trn_rc, &tst_acc, &tst_rc, pool_sz, i, trn_bat_sz, tst_bat_sz, trn_bat_cnt, trn_prec](uint64_t epoch, uint64_t tst_bat_cnt){ while (net_stat == NEUNET_STAT_NRM){
    // 處理當前數據索引值
    // current data index value of process
    uint64_t data_idx = i,
    // 批計數
    // batch count
             bat_cnt  = 0;
    // 最後一個縣城標記
    // last thread mark
    bool last_tkn = false;
    // 訓練
    // train
    while (bat_cnt < trn_bat_cnt && i < trn_bat_sz) {
        // 獲取當前批索引訓練數據對應的標簽
        // get train data and corresponding labels of current batch index
        auto input = train.elem[train.data_idx[data_idx]];
        auto lbl   = train.lbl[train.data_idx[data_idx]];
        auto orgn  = neunet::lbl_orgn(lbl, mnist_orgn_size);
        // 下一批數據索引
        // next batch data index
        data_idx  += trn_bat_sz;
        if (bat_cnt || epoch) {
            if (last_tkn) last_tkn = false;
            // 如果不是最後一個到達的綫程，需要等待最多 1000ms
            // if it is not last arriving thread, it need to wait, 1000ms at most
            else trn_ctrl.thread_sleep(1000);
            if (net_stat == NEUNET_STAT_END) break;
        }
        // 前向傳播
        // FP
        net_forward(net_lyr, input, i);
        neunet::output_acc_rc(input, trn_prec, lbl, acc_cnt, rc_cnt);
        // 下一批次
        // next batch
        ++bat_cnt;
        // 反向傳播
        // BP
        net_backward(net_lyr, input, i, orgn);
        if (++bat_sz_cnt == trn_bat_sz) {
            // 標記最後一個綫程
            // last thread marked
            bat_sz_cnt = 0;
            last_tkn   = true;
            // 保存精確度與回收計數值
            // save accuracy & recall count value
            trn_acc.en_queue(acc_cnt);
            trn_rc.en_queue(rc_cnt);
            acc_cnt = 0;
            rc_cnt  = 0;
            if (bat_cnt == trn_bat_cnt) {
                // 洗牌訓練數據索引
                // shuffle train data index
                train.data_idx.shuffle();
                // 激活測試過程
                // activate testing process
                tst_ctrl.thread_wake_all();
            // 否則激活下一批次訓練過程
            // otherwise activate training process of next batch
            } else trn_ctrl.thread_wake_all();
        }
    }
    if (net_stat == NEUNET_STAT_END) break;
    // 重置值
    // reset value
    data_idx = i;
    bat_cnt  = 0;
    // 推測
    // deduce
    while (bat_cnt < tst_bat_cnt && i < tst_bat_sz) {
        // 獲取當前批次測試數據對應的標簽
        // get test data and corresponding labels of current batch
        auto input = train.elem[data_idx];
        auto lbl   = train.lbl[data_idx];
        // 并非最後到達的綫程過程需要等待，最多 1000ms
        // process which is not the last arriving thread should wait, 1000ms at most
        if (!(bat_cnt || last_tkn)) tst_ctrl.thread_sleep();
        if (net_stat == NEUNET_STAT_END) break;
        // 測試
        // test
        net_deduce(net_lyr, input);
        neunet::output_acc_rc(input, trn_prec, lbl, acc_cnt, rc_cnt);
        // 下一批次數據索引
        // next batch data index
        data_idx += trn_bat_sz;
        ++bat_cnt;
    }
    // 下一個epoch
    // next epoch
    if (net_stat == NEUNET_STAT_END) break;
    ++epoch;
    if (++bat_sz_cnt == pool_sz) {
        bat_sz_cnt = 0;
        // 保存準確度與回收計數值
        // save accuracy & recall count value
        tst_acc.en_queue(acc_cnt);
        tst_rc.en_queue(rc_cnt);
        acc_cnt = 0;
        rc_cnt  = 0;
        // 激活下一個epoch訓練過程
        // activate next epoch train process
        trn_ctrl.thread_wake_all();
    }
} }, 0ull, test.element_count / tst_bat_sz);

// 訓練和測試的召回率
// recall rate of train & test
long double rc_rt = .0;
// epoch count
uint64_t ep_cnt = 0;
// 訓練與測試輸出數據表示
// train & test output data show
while (rc_rt < 1) {
    // 獲取epoch時間節點
    // get time point of a epoch
    auto ep_tm_pt = NEUNET_CHRONO_TIME_POINT;
    // 訓練
    // train
    for (auto i = 0ull; i < trn_bat_cnt; ++i) {
        // 獲取單批次訓練時間節點
        // get time point of a train batch
        auto tm_pt = NEUNET_CHRONO_TIME_POINT;
        // 計算準確率和召回率
        // calculate accuracy and recall rate
        auto acc   = trn_acc.de_queue() / (trn_bat_sz * 1.);
        rc_rt      = trn_rc.de_queue() / (trn_bat_sz * 1.);
        // 打印單批次訓練耗時
        // print the duration of a batch training
        neunet::print_train_progress((i + 1), trn_bat_cnt, acc, rc_rt, (NEUNET_CHRONO_TIME_POINT - tm_pt));
    }
    // 推測
    // deduce
    std::printf("\r[Deducing]...");
    // 計算準確率和召回率
    // calculate accuracy and recall rate
    auto acc = tst_acc.de_queue() / (test.element_count * 1.);
    rc_rt    = tst_rc.de_queue() / (test.element_count * 1.);
    // 打印一個epoch的耗時
    // print the duration of a epoch
    neunet::print_epoch_status(++ep_cnt, acc, rc_rt, (NEUNET_CHRONO_TIME_POINT - ep_tm_pt));
}
```

## 測試<br>Test

使用函數 `net_deduce` 推測自定義輸入。這個輸入需要符合 mnist 數據集形狀，即$28^2$。\
Use function `net_deduce` to deduce customized input. This input should fit to the mnist shape, $28^2$.

```c++
using bmio::bitmap;
// 加載手寫數字圖片
// load hand-writing number image
bitmap in_img("...\\deduce_test.bmp");
// 獲取灰度單通道圖片矩陣
// get gray single channel image matrix
auto test_input  = in_img.gray;
// 使用推測函數
// use deducing function
auto test_output = net_deduce(net_lyr, input);
// 測試標簽
// test label
auto lbl_test    = 0;
// 打印推測結果
// print deducing result
for (auto i = 1ull; i < test_output.element_count; ++i) if (test_output.index(i) > test_output.index(lbl_test)) lbl_test = i;
cout << "image is number: " << lbl_test << endl;
```

更多關於位圖請參閲 [`bitmap`](../DataStructure/bmio/bitmap/cover.md) 。\
Please refer to [`bitmap`](../DataStructure/bmio/bitmap/cover.md) for more details about bitmap process.

[<< 返回 Back](cover.md)

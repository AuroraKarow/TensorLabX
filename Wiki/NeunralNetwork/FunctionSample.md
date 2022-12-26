# 使用基礎函數搭建 ANN<br>Build ANN with base functions

示例將使用 LeNet-5 搭建一個簡易的 ANN 框架，其步驟為\
Example of building a simple ANN structure would use LeNet-5. Process would be

- [必要的頭文件 Necessary head file](#必要的頭文件necessary-head-file)
- 全局變量 Global variable
- 前向傳播 Forward propagation
- 反向傳播 Backward propagation
- 訓練 Train
- 測試 Deduce

在這個部分，將嘗試給出多綫程思路。\
In this section, it would try to give the multi-thread programming idea.

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
// 標準庫需要的函數與類
// Necessary functions and classed from standard library
using std::cout;
using std::endl;
using std::string;
// 集合和矩陣
// Set and matrix
using neunet::net_set;
using neunet::vect;
// 異步控制器和綫程池
// Aysnchronous controller & threads pool
using neunet::async::async_controller;
using neunet::async::async_pool;
// 主函數
// Main function
int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.";
    cout << endl;
    return EXIT_SUCCESS;
}
```

由於頭文件 `neunet` 包含了一切所需的工具，因此引入這一個頭文件即可。\
Since head file `neunet` includes all needed tool, including this head file is just fine.

[<< 返回 Back](cover.md)

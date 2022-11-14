/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once

#define OMP_MATRIX_ON       1
#define OMP_MATRIX_OFF      0

#define OMP_MATRIX_MODE     OMP_MATRIX_OFF

#define MATRIX_BLOCKSIZE    32
#define MATRIX_UNROLL       4

#include <iostream>
#include "net_chrono"
#include "neunet"
#include "dataset"

using namespace neunet::layer;

using std::cout;
using std::endl;
using std::string;

using neunet::net_set;
using neunet::vect;

using neunet::dataset::mnist;
using neunet::NeunetCore;
using neunet::AddLayer;
using neunet::RunInit;
using neunet::TrainDeduceThread;
using neunet::DataShowThread;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.\n" << endl;
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;

    // mnist data loading
    std::string root = "E:\\VS Code project data\\MNIST\\";
    mnist train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
    test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());

    // network declaration
    auto dLearnRate = .4l;
    NeunetCore net_core(125, 125, .1l);

    // layers adding
    
    AddLayer<NetLayerConv>(net_core, 20, 5, 5, 1, 1, 0, 0, dLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, 1e-5l); // 0, 1, 1e-5l
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    AddLayer<NetLayerConv>(net_core, 50, 5, 5, 1, 1, 0, 0, dLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, 1e-5l);
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    AddLayer<NetLayerTrans>(net_core);
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
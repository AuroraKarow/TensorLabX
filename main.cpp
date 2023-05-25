/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once

#include <iostream>
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
using neunet::Shape;
using neunet::TrainTestThread;
using neunet::DataShowThread;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.\n" << endl;
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;

    // network declaration

    auto dNormLearnRate = .4l,
         dBNLearnRate   = 1e-5l;
    NeunetCore net_core(80, 80, .1l);

    // layers adding
    
    AddLayer<NetLayerConv>(net_core, 20, 5, 5, 1, 1, 0, 0, dNormLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, dBNLearnRate, dBNLearnRate);
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    AddLayer<NetLayerConv>(net_core, 50, 5, 5, 1, 1, 0, 0, dNormLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, dBNLearnRate, dBNLearnRate);
    AddLayer<NetLayerAct>(net_core, NEUNET_RELU);
    AddLayer<NetLayerPool>(net_core, NEUNET_POOL_AVG, 2, 2, 2, 2);
    AddLayer<NetLayerFlat>(net_core);
    AddLayer<NetLayerFC>(net_core, 500, dNormLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, dBNLearnRate, dBNLearnRate);
    AddLayer<NetLayerAct>(net_core, NEUNET_SIGMOID);
    AddLayer<NetLayerFC>(net_core, 10, dNormLearnRate);
    AddLayer<NetLayerBN>(net_core, 0, 1, dBNLearnRate, dBNLearnRate);
    AddLayer<NetLayerAct>(net_core, NEUNET_SOFTMAX);

    // mnist data loading

    std::string root = "D:\\Users\\Aurora\\Documents\\Visual Studio Code Project\\MNIST\\file\\"; // "E:\\VS Code project data\\MNIST\\";
    mnist train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
    test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());

    auto trn_bat_cnt = train.element_count / net_core.iTrnBatSz;

    // shape initialization

    Shape(net_core, trn_bat_cnt, train.element_line_count, train.element_column_count, 1);

    // threads

    TrainTestThread(net_core, train.elem, train.lbl, train.data_idx, test.elem, test.lbl, mnist_orgn_size, trn_bat_cnt, (test.element_count / net_core.iTstBatSz));
    DataShowThread(net_core, trn_bat_cnt, test.element_count);

    auto chrono_end = NEUNET_CHRONO_TIME_POINT;
    cout << '\n' << (chrono_end - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}

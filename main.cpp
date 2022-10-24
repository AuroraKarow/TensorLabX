/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once

#include <iostream>
#include "net_chrono"
#include "neunet"
#include "dataset"

using std::cout;
using std::endl;
using std::string;

using neunet::net_set;
using neunet::dataset::mnist;
using neunet::Neunet;
using neunet::vect;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.\n" << endl;
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;

    std::string root = "E:\\VS Code project data\\MNIST\\";
    mnist train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
    test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());
    Neunet net(125, 125, .1l);
    auto dLearnRate = .8l;
    net.AddLayer<neunet::NetLayerConv>(20, 5, 5, 1, 1, 0, 0, dLearnRate);
    net.AddLayer<neunet::NetLayerBN>(); // 0, 1, 1e-5l
    net.AddLayer<neunet::NetLayerAct>(NEUNET_RELU);
    net.AddLayer<neunet::NetLayerPool>(NEUNET_POOL_AVG, 2, 2, 2, 2);
    net.AddLayer<neunet::NetLayerConv>(50, 5, 5, 1, 1, 0, 0, dLearnRate);
    net.AddLayer<neunet::NetLayerBN>();
    net.AddLayer<neunet::NetLayerAct>(NEUNET_RELU);
    net.AddLayer<neunet::NetLayerPool>(NEUNET_POOL_AVG, 2, 2, 2, 2);
    net.AddLayer<neunet::NetLayerTrans>();
    net.AddLayer<neunet::NetLayerFC>(500, dLearnRate);
    net.AddLayer<neunet::NetLayerBN>();
    net.AddLayer<neunet::NetLayerAct>(NEUNET_SIGMOID);
    net.AddLayer<neunet::NetLayerFC>(10, dLearnRate);
    net.AddLayer<neunet::NetLayerAct>(NEUNET_SOFTMAX);
    auto flag = net.Run(train.elem, train.lbl, test.elem, test.lbl, mnist_orgn_size, train.element_line_count, train.element_column_count, 1);
    cout << flag << endl;

    auto chrono_end = NEUNET_CHRONO_TIME_POINT;
    cout << '\n' << (chrono_end - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}
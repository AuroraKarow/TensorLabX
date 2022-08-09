/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once
#include <iostream>
#include "net_chrono"
#include "neunet"

using std::cout;
using std::endl;
using std::string;

using neunet::net_set;
using neunet::dataset::mnist;
using neunet::NeunetMNIST;
using neunet::vect;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.\n" << endl;
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;
    
    using mat_t = long double;
    std::string root = "...\\MNIST\\";
    mnist<mat_t> train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
    test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());
    NeunetMNIST net(125, 0.1);
    auto dLearnRate = 0.1l;
    net.AddLayer<neunet::layer::LayerConv<mat_t>>(20, 5, 5, 1, 1, 0, 0, dLearnRate);
    net.AddLayer<neunet::layer::LayerBN<mat_t>>();
    net.AddLayer<neunet::layer::LayerAct<mat_t>>(NEUNET_RELU);
    net.AddLayer<neunet::layer::LayerPool>(NEUNET_POOL_MAX, 2, 2, 2, 2);
    net.AddLayer<neunet::layer::LayerConv<mat_t>>(50, 5, 5, 1, 1, 0, 0, dLearnRate);
    net.AddLayer<neunet::layer::LayerBN<mat_t>>();
    net.AddLayer<neunet::layer::LayerAct<mat_t>>(NEUNET_RELU);
    net.AddLayer<neunet::layer::LayerPool>(NEUNET_POOL_MAX, 2, 2, 2, 2);
    net.AddLayer<neunet::layer::LayerTrans>();
    net.AddLayer<neunet::layer::LayerFC<mat_t>>(500, dLearnRate);
    net.AddLayer<neunet::layer::LayerBN<mat_t>>();
    net.AddLayer<neunet::layer::LayerAct<mat_t>>(NEUNET_SIGMOID);
    net.AddLayer<neunet::layer::LayerFC<mat_t>>(10, dLearnRate);
    net.AddLayer<neunet::layer::LayerAct<mat_t>>(NEUNET_SOFTMAX);
    std::cout << net.Run(train, test) << std::endl;

    auto chrono_end = NEUNET_CHRONO_TIME_POINT;
    cout << '\n' << (chrono_end - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}
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

using namespace neunet;

int main(int argc, char *argv[], int *envp[]) {
    std::cout << "hello, world." << std::endl;
    auto bgpt = NEUNET_CHRONO_TIME_POINT;

    NeunetCore core;

    AddLayer<layer::NetLayerFC>(core, 2);                   // 0
    neunet_layer_cast<layer::NetLayerFC>(core.seqLayer[0])->vecWeight = 
    {{1.4},
     {2.7}};
    AddLayer<layer::NetLayerBias>(core);                    // 1
    AddLayer<layer::NetLayerAct>(core, NEUNET_ARELU);       // 2
    AddLayer<layer::NetLayerFC>(core, 4);                   // 3
    neunet_layer_cast<layer::NetLayerFC>(core.seqLayer[3])->vecWeight = 
    {{1.4, 4.8},
     {2.7, 3.7},
     {3.3, 1.8},
     {1.9, 3.8}};
    AddLayer<layer::NetLayerBias>(core);                    // 4
    AddLayer<layer::NetLayerAct>(core, NEUNET_ARELU);       // 5
    AddLayer<layer::NetLayerFC>(core, 3);                   // 6
    neunet_layer_cast<layer::NetLayerFC>(core.seqLayer[6])->vecWeight = 
    {{1.4, 3.4, 2.8, 9.0},
     {2.7, 9.7, 3.6, 9.1},
     {3.5, 5.5, 2.0, 7.0}};
    AddLayer<layer::NetLayerBias>(core);                    // 7
    AddLayer<layer::NetLayerAct>(core, NEUNET_ARELU);       // 8
    AddLayer<layer::NetLayerFC>(core, 2);                   // 9
    neunet_layer_cast<layer::NetLayerFC>(core.seqLayer[9])->vecWeight = 
    {{1.4, 8.4, 1.0},
     {2.7, 7.2, 0.7}};
    AddLayer<layer::NetLayerBias>(core);                    // 10
    AddLayer<layer::NetLayerAct>(core, NEUNET_ARELU);       // 11
    AddLayer<layer::NetLayerFC>(core, 2);                   // 12
    neunet_layer_cast<layer::NetLayerFC>(core.seqLayer[12])->vecWeight = 
    {{1.4, 1.4},
     {2.7, 2.7}};
    AddLayer<layer::NetLayerBias>(core);                    // 13
    AddLayer<layer::NetLayerAct>(core, NEUNET_ARELU_LOSS);  // 14
    
    vect input = {{1.4}};
    TestShow(core, input, 0, 1, 1);
    
    auto edpt = NEUNET_CHRONO_TIME_POINT;
    cout << (edpt - bgpt) << "ms" << endl;
    return EXIT_SUCCESS;
}
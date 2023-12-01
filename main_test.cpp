/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once

#include <iostream>
#include "neunet"
#include "net_decimal"

using std::cout;
using std::endl;

using namespace neunet;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world." << endl;
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;

    net_decimal::default_infinite_precision = 64;
    cout << std::pow("-0.216"_d, 1_d / 3_d) << endl;

    cout << (NEUNET_CHRONO_TIME_POINT - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}
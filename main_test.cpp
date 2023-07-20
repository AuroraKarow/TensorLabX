/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once

#include <iostream>
#include "neunet"

using std::cout;
using std::endl;

using namespace neunet;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world." << endl; vect fst {10000, 10000, true}, snd {10000, 10000, true};
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;

    
    auto ans = fst * snd;

    cout << (NEUNET_CHRONO_TIME_POINT - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}
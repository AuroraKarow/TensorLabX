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

    uint64_t in_ln = 24, in_col = 24, out_ln = 0, out_col = 0;
    vect in(in_ln * in_col, 20, true);
    for (auto i = 0; i < 30000; ++i) {
        net_set<net_list<matrix::pos>> pos;
        auto caffe = conv::CaffeTransform(in, in_ln, in_col, out_ln, out_col, 2, 2, 2, 2, 0, 0),
             out   = conv::PoolMaxAvg(NEUNET_POOL_MAX, caffe, 2, 2, pos);
        auto grad  = conv::GradLossToPoolMaxAvgCaffeInput(NEUNET_POOL_MAX, out, 2, 2, pos);
        auto in_g  = conv::CaffeTransform(grad, in_ln, in_col, out_ln, out_col, 2, 2, 2, 2, 0, 0, true);
    }

    // vect in = {{1, 0, 1},
    //            {2, 2, 2},
    //            {0, 1, 1},
    //            {1, 0, 0},
    //            {1, 3, 1},
    //            {3, 2, 3},
    //            {0, 1, 3},
    //            {2, 1, 3},
    //            {2, 0, 2}};
    // uint64_t in_ln = 3, in_col = 3, out_ln = 0, out_col = 0;
    // net_set<net_list<matrix::pos>> pos;
    // auto caffe = conv::CaffeTransform(in, in_ln, in_col, out_ln, out_col, 2, 2, 1, 1, 0, 0);
    // auto out = conv::PoolMaxAvg(NEUNET_POOL_MAX, caffe, 2, 2, pos);
    // cout << out << '\n' << endl;
    // auto grad = conv::GradLossToPoolMaxAvgCaffeInput(NEUNET_POOL_MAX, out,  2, 2, pos);
    // cout << grad << '\n' << endl;
    // auto in_g = conv::CaffeTransform(grad, in_ln, in_col, out_ln, out_col, 2, 2, 1, 1, 0, 0, true);
    // cout << in_g << endl;

    auto edpt = NEUNET_CHRONO_TIME_POINT;
    cout << (edpt - bgpt) << "ms" << endl;
    return EXIT_SUCCESS;
}
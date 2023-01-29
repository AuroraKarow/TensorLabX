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
    cout << "hello, world." << endl;
    auto chrono_begin = NEUNET_CHRONO_TIME_POINT;

    vect in = {{1, 0, 1},
               {2, 2, 2},
               {0, 1, 1},
               {1, 0, 0},
               {1, 3, 1},
               {3, 2, 3},
               {0, 1, 3},
               {2, 1, 3},
               {2, 0, 2}};
    uint64_t caffe_ln_cnt = 0, caffe_col_cnt = 0, out_ln_cnt, out_col_cnt = 0;
    auto caffe_data = conv::CaffeTransformData(3, caffe_ln_cnt, caffe_col_cnt, 3, 3, out_ln_cnt, out_col_cnt, 2, 2, 1, 1, 0, 0);
    net_set<net_list<uint64_t>> elem_idx;
    auto out = conv::PoolMax(in, caffe_data, 4, caffe_ln_cnt, elem_idx);
    cout << out << endl;
    cout << endl;
    auto grad = conv::GradLossToPoolMaxChann(out, 9, elem_idx);
    cout << grad << endl;
    cout << endl;
    auto in_grad = im2col_trans(grad, 3, 3);
    cout << in_grad << endl;

    cout << (NEUNET_CHRONO_TIME_POINT - chrono_begin) << "ms" << endl;
    return EXIT_SUCCESS;
}
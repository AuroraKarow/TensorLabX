/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once

#include <iostream>

#include "net_chrono"
#include "net_memory"
#include "net_map"
#include "neunet"

using std::cout;
using std::endl;
using std::string;

using namespace neunet;

int main(int argc, char *argv[], int *envp[]) {
    std::cout << "hello, world." << std::endl;
    auto bgpt = NEUNET_CHRONO_TIME_POINT;

    vect in = {{1, 0, 1},
               {2, 2, 2},
               {0, 1, 1},
               {1, 0, 0},
               {1, 3, 1},
               {3, 2, 3},
               {0, 1, 3},
               {2, 1, 3},
               {2, 0, 2}};
    uint64_t caffe_ln_cnt = 0, caffe_col_cnt = 0, output_ln_cnt = 0, output_col_cnt = 0;
    auto caffe_data = conv::CaffeTransformData(3, caffe_ln_cnt, caffe_col_cnt, 3, 3, output_ln_cnt, output_col_cnt, 2, 2, 1, 1, 0, 0);
    auto caffe = conv::CaffeTransform(in, caffe_data, caffe_ln_cnt, caffe_col_cnt);
    cout << caffe << '\n' << endl;
    auto im2col = conv::CaffeTransform(caffe, caffe_data, in.line_count, in.column_count, false);
    cout << im2col << '\n' << endl;
    
    auto edpt = NEUNET_CHRONO_TIME_POINT;
    cout << (edpt - bgpt) << "ms" << endl;
    return EXIT_SUCCESS;
}
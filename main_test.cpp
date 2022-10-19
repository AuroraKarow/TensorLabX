/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once

#include <iostream>

#include "net_chrono"
#include "neunet"
#include "net_tree"

using std::cout;
using std::endl;
using std::string;

using namespace neunet;

int main(int argc, char *argv[], int *envp[]) {
    std::cout << "hello, world." << std::endl;
    auto bgpt = NEUNET_CHRONO_TIME_POINT;

    // uint64_t in_ln = 24, in_col = 24, out_ln = 0, out_col = 0;
    // vect in(in_ln * in_col, 20, true);
    // for (auto i = 0; i < 30000; ++i) {
    //     net_set<net_list<matrix::pos>> pos;
    //     auto caffe = conv::CaffeTransform(in, in_ln, in_col, out_ln, out_col, 2, 2, 2, 2, 0, 0),
    //          out   = conv::PoolMaxAvg(NEUNET_POOL_MAX, caffe, 2, 2, pos);
    //     auto grad  = conv::GradLossToPoolMaxAvgCaffeInput(NEUNET_POOL_MAX, out, 2, 2, pos);
    //     auto in_g  = conv::CaffeTransform(grad, in_ln, in_col, out_ln, out_col, 2, 2, 2, 2, 0, 0, true);
    // }

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

    // 用来放矩阵组合的

    vect a = {{0, 1},
              {2, 3}},
         b = {{0, 1, 2}},
         c = {{0},
              {1}};
    /* 0 1 0
     * 2 3 1
     * 0 1 2
     */
    net_sequence test = {a, b, c};

    net_sequence<matrix::pos> pos_seq;

    auto curr_ln = 0ull, curr_col = 0ull;
    for (auto i = 0ull; i < test.length; ++i) {
        pos_seq.emplace_back(matrix::pos{curr_ln, curr_col});
        // code for next current posistion of [curr_ln] and [curr_col]
        /* curr_ln  = ...
         * curr_col = ...
         */
    }

    vect d(test, pos_seq);
    // /*
    // vect d = {{a, c},
    //           {  b }};
    // */
    // cout << d << endl;

    // im2col 卷积测试

    // vect_dec chann_a = {{1, 2, 0},
    //                     {1, 1, 3},
    //                     {0, 2, 2}},
    //          chann_b = {{0, 2, 1},
    //                     {0, 3, 2},
    //                     {1, 1, 0}},
    //          chann_c = {{1, 2, 1},
    //                     {0, 1, 3},
    //                     {3, 3, 2}};
    // net_sequence chann = {chann_a, chann_b, chann_c};
    
    // auto im2col_chann = im2col_trans(chann);

    // vect_dec knl = {{1, 1},
    //                 {1, 0},
    //                 {2, 0},
    //                 {2, 1},
    //                 {1, 2},
    //                 {1, 1},
    //                 {1, 2},
    //                 {1, 1},
    //                 {0, 1},
    //                 {1, 2},
    //                 {1, 2},
    //                 {0, 0}};
    
    // uint64_t out_ln_cnt = 0, out_col_cnt = 0;

    // 从这里开始计时

    // // for (auto i = 0ull; i < 30000; ++i) {
    // //     auto in_caffe = conv::CaffeTransform(im2col_chann, 3, 3, out_ln_cnt, out_col_cnt, 2, 2, 1, 1, 0, 0);
    // //     auto im2col_out = conv::Conv(in_caffe, knl);
    // // }
    // auto in_caffe = conv::CaffeTransform(im2col_chann, 3, 3, out_ln_cnt, out_col_cnt, 2, 2, 1, 1, 0, 0);

    // 计时结束

    // auto im2col_out = conv::Conv(in_caffe, knl);

    // 函数式矩阵

    // uint64_t ln_cnt = 0, col_cnt = 0;
    // auto a = matrix::init({{1, 2},
    //                        {3, 4}}, ln_cnt, col_cnt);
    // matrix::print(a, ln_cnt, col_cnt);
    // std::printf("\n");
    // auto b = matrix::mult(a, ln_cnt, col_cnt, a, col_cnt);
    // matrix::print(b, ln_cnt, col_cnt);
    // matrix::recycle(a, b);

    // auto a = 13.67_dec;
    // cout << a.reciprocal << endl;
    
    auto edpt = NEUNET_CHRONO_TIME_POINT;
    cout << (edpt - bgpt) << "ms" << endl;
    return EXIT_SUCCESS;
}
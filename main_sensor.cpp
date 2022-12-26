/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

#pragma once

#include <iostream>
#include "neunet"

using std::cout;
using std::endl;
using std::string;

using namespace neunet;

// 网络层数
int layer_cnt   = 5;
// 数据批量
int input_batch = 10;

// 每层的权重，长度是网络层数
net_set<vect> set_weight(layer_cnt);
// 每层的偏倚，长度是网络层数
net_set<vect> set_bias(layer_cnt);
// 每层的批量X，长度是网络层数
net_set<net_set<vect>> weight_input(layer_cnt), 
// 每层的批量Z，长度是网络层
                       act_input(layer_cnt);

vect FC_output(const vect &weight, const vect &input) { return weight * input; }
vect FC_bias(const vect &bias, const vect &input) { return input + bias; }

vect FC_grad_input(const vect &grad, const vect &weight) { return weight.transpose * grad; }
vect FC_grad_weight(const vect &grad, const vect &input) { return grad * input.transpose; }

void forw_prop(net_set<vect> &input) {
    // 网络层迭代
    for (auto i = 0; i < layer_cnt; ++i) {
        // 保存当前层[i]的X
        weight_input[i] = input;
        // 如果当前层[i]的Z没有初始化，那么分配内存
        if (!act_input[i].length) act_input[i].init(input_batch);
        for (auto j = 0; j < input_batch; ++j) {
            // bar X = WX
            input[j]        = FC_output(set_weight[i], input[j]);
            // Z = bar X + b
            act_input[i][j] = FC_bias(set_bias[i], input[j]);
            // bar Y = σ(Z)
            input[j]        = AReLU(act_input[i][j]);
        }
    }
}

/**
 * @brief   BACKWARD PROPAGATION
 * @param	fp_output	[Input]	FORWARD PROPAGATION output -- bar Y
 * @param	origin	[Input]	Y
 * @param	weight_lr	[Inpit]	更新weight的学习率
 * @param	bias_lr	[Input]	更新bias的学习率
 */
void back_prop(net_set<vect> &fp_output, const net_set<vect> &origin, double weight_lr, double bias_lr) {
    // 损失到最后一层激活函数输入的梯度
    // last layer δ(Z)
    for (auto i = 0; i < input_batch; ++i) fp_output[i] = AReLU_loss_grad(act_input[layer_cnt - 1][i], fp_output[i], origin[i]);
    for (auto i = layer_cnt; i; --i) {
        auto curr_idx = i - 1;
        vect bias_grad;
        vect weight_grad;
        for (auto j = 0; j < input_batch; ++j) {
            // hardamard product of backward gradient and activation function derivative
            // δ(Z) = σ'(Z)·δ(bar Y)
            if (i != layer_cnt) {
                fp_output[j] = fp_output[j].elem_wise_opt(AReLU_dv(act_input[curr_idx][j]), MATRIX_ELEM_MULT);
            }
            // 由于偏倚的梯度倒数为1，那么梯度就是所有回传梯度的期望
            if (bias_grad.verify) bias_grad += fp_output[j];
            else bias_grad = fp_output[j];
            // 计算权重梯度的期望
            if (weight_grad.verify) weight_grad += FC_grad_weight(fp_output[j], act_input[curr_idx][j]);
            else weight_grad = FC_grad_weight(fp_output[j], act_input[curr_idx][j]);
            // 回传梯度
            if (curr_idx) fp_output[j] = FC_grad_input(fp_output[j], set_weight[i]);
        }
        // 更新
        set_weight[curr_idx] -= weight_lr * weight_grad.elem_wise_opt(input_batch, MATRIX_ELEM_DIV);
        set_bias[curr_idx]   -= bias_lr * bias_grad.elem_wise_opt(input_batch, MATRIX_ELEM_DIV);
    }
}

int main(int argc, char *argv[], int *envp[]) {
    std::cout << "hello, world." << std::endl;
    auto bgpt = NEUNET_CHRONO_TIME_POINT;

    // 输入矩阵批 X
    net_set<vect> input = {
        // 1
        {{0},
         {0}},
        // 2
        {{0},
         {0}},
        // 3
        {{0},
         {0}},
        // 4
        {{0},
         {0}},
        // 5
        {{0},
         {0}},
        // 6
        {{0},
         {0}},
        // 7
        {{0},
         {0}},
        // 8
        {{0},
         {0}},
        // 9
        {{0},
         {0}},
        // 10
        {{0},
         {0}}
    };

    // Y
    net_set<vect> origin = {
        // 1
        {{0},
         {0}},
        // 2
        {{0},
         {0}},
        // 3
        {{0},
         {0}},
        // 4
        {{0},
         {0}},
        // 5
        {{0},
         {0}},
        // 6
        {{0},
         {0}},
        // 7
        {{0},
         {0}},
        // 8
        {{0},
         {0}},
        // 9
        {{0},
         {0}},
        // 10
        {{0},
         {0}}
    };
    
    // 初始化权重 - 5个网络层, 5, 4, 3, 2, 2
    // 1
    set_weight[0] = vect(5, 2, true);
    // 2
    set_weight[1] = vect(4, 5, true);
    // 3
    set_weight[2] = vect(3, 4, true);
    // 4
    set_weight[3] = vect(2, 3, true);
    // 5
    set_weight[4] = vect(2, 2, true);

    // 偏倚 - 5个网络层, 5, 4, 3, 2, 2
    // 1
    set_bias[0] = vect(5, 1, true);
    // 2
    set_bias[1] = vect(4, 1, true);
    // 3
    set_bias[2] = vect(3, 1, true);
    // 4
    set_bias[3] = vect(2, 1, true);
    // 5
    set_bias[4] = vect(2, 1, true);

    double w_lr  = 1e-5,
           b_lr  = 1e-5,
           acc   = 0.1;
    
    bool train = true;

    while (train) {
        train = false;
        auto input_train = input;
        input_train.shuffle();
        forw_prop(input_train);
        for (auto i = 0; i < input_batch; ++i) {
            auto curr_abs = (input_train[i] - origin[i]).abs;
            if (curr_abs[0][0] > acc || curr_abs[1][0] > acc) {
                train = true;
                back_prop(input_train, origin, w_lr, b_lr);
                break;
            }
        }
    }
    
    auto edpt = NEUNET_CHRONO_TIME_POINT;
    cout << (edpt - bgpt) << "ms" << endl;
    return EXIT_SUCCESS;
}
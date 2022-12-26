/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

// file main_fn.cpp

#pragma once

#include <iostream>
#include "neunet"

namespace neunet { namespace fn_app {

/* layer */

constexpr int lyr_act  = 0,
              lyr_fc   = 1,
              lyr_conv = 2,
              lyr_pool = 3,
              lyr_bn   = 4;

// layer base

struct layer_base {};
using layer_ptr = std::shared_ptr<layer_base>;

// layer signal

struct layer_signal : layer_base {
    // layer type
    int layer_type = 0;
};

// layer derivative BP

struct layer_deriv : layer_base {
    // input
    net_set<vect> input;
};

void layer_init(layer_deriv &lyr, int batch_size) { lyr.input.init(batch_size, false); }

// act layer

struct layer_act : layer_deriv, layer_signal {
    // activation function type
    int act_fn_type = NEUNET_RELU;
};

void layer_create(layer_act &lyr, int act_fn_type) {
    lyr.act_fn_type = act_fn_type;
    lyr.layer_type  = NEUNET_LAYER_ACT;
}

/**
 * @brief   Activation layer FP
 * @param	lyr	[in]	Layer reference
 * @param	input	[in]	Input matrix
 * @param	bat_idx	[in]	Synchronous thread index
 */
void forw_prop(layer_act &lyr, vect &input, int bat_idx) {
    lyr.input[bat_idx] = std::move(input);
    // FP
    switch (lyr.act_fn_type) {
    case NEUNET_SIGMOID: input = sigmoid(lyr.input[bat_idx]); break;
    case NEUNET_RELU: input = ReLU(lyr.input[bat_idx]); break;
    case NEUNET_ARELU_LOSS:
    case NEUNET_ARELU: input = AReLU(lyr.input[bat_idx]); break;
    case NEUNET_SOFTMAX: input = softmax(lyr.input[bat_idx]); break;
    default: input = std::move(lyr.input[bat_idx]); break;
    }
}

/**
 * @brief   Activation layer BP
 * @param	lyr	[in]	Layer reference
 * @param	grad	[in]	BP gradient
 * @param	bat_idx	[in]	Synchronous thread index
 * @param	orgn	[in]	Label vector
 */
void back_prop(layer_act &lyr, vect &grad, int bat_idx, const vect &orgn) {
    switch (lyr.act_fn_type) {
    case NEUNET_SIGMOID: grad = sigmoid_dv(lyr.input[bat_idx]).elem_wise_opt(grad,MATRIX_ELEM_MULT); break;
    case NEUNET_RELU: grad = ReLU_dv(lyr.input[bat_idx]).elem_wise_opt(grad,MATRIX_ELEM_MULT); break;
    case NEUNET_ARELU: grad = AReLU_dv(lyr.input[bat_idx]).elem_wise_opt(grad,MATRIX_ELEM_MULT); break;
    case NEUNET_ARELU_LOSS: grad = AReLU_loss_grad(lyr.input[bat_idx], grad, orgn); break;
    case NEUNET_SOFTMAX: grad = softmax_cec_grad(grad, orgn); break;
    default: return;
    }
}

// shape layer

struct layer_shape : layer_base {
    // channel count
    int chann_cnt = 0;
};

// flatting layer

struct layer_flat : layer_shape {
    // channel matrix element count
    int chann_elem_cnt = 0,
    // tensor matrix element count
        ten_elem_cnt   = 0;
};

void layer_init(layer_flat &lyr, int &in_ln_cnt, int in_col_cnt, int chann_cnt) {
    lyr.chann_cnt      = chann_cnt;
    lyr.chann_elem_cnt = in_ln_cnt * in_col_cnt;
    lyr.ten_elem_cnt   = lyr.chann_elem_cnt * chann_cnt;
    in_ln_cnt          = lyr.ten_elem_cnt;
}

void forw_prop(layer_flat &lyr, vect &input) { input = input.reshape(lyr.ten_elem_cnt, 1); }

void back_prop(layer_flat &lyr, vect &grad) { grad = grad.reshape(lyr.chann_elem_cnt, lyr.chann_cnt); }

// caffe layer

struct layer_caffe : layer_shape {
    // caffe data
    net_set<uint64_t> caffe_data;
    // tensor parameter
    int in_ln_cnt       = 0,
        in_col_cnt      = 0,
        in_elem_cnt     = 0,
        caffe_ln_cnt    = 0,
        caffe_col_cnt   = 0,
    // stride
        stride_ln       = 0,
        stride_col      = 0,
    // filter
        filter_ln_cnt   = 0,
        filter_col_cnt  = 0,
        filter_elem_cnt = 0,
    // dilation
        dilate_ln      = 0,
        dilate_col     = 0;
};

void layer_create(layer_caffe &lyr, int filter_ln_cnt, int filter_col_cnt, int stride_ln_cnt, int stride_col_cnt, int dilate_ln_cnt, int dilate_col_cnt) {
    lyr.stride_ln       = filter_ln_cnt;
    lyr.stride_col      = filter_col_cnt;
    lyr.filter_ln_cnt   = filter_ln_cnt;
    lyr.filter_col_cnt  = filter_col_cnt;
    lyr.filter_elem_cnt = filter_ln_cnt * filter_col_cnt;
    lyr.dilate_ln       = dilate_ln_cnt;
    lyr.dilate_col      = dilate_col_cnt;
}

void layer_init(layer_caffe &lyr, int &input_ln_cnt, int &input_col_cnt, int chann_cnt) {
    lyr.chann_cnt  = chann_cnt;
    lyr.in_ln_cnt  = input_ln_cnt;
    lyr.in_col_cnt = input_col_cnt;
    uint64_t caffe_ln  = 0,
             caffe_col = 0,
             samp_ln   = 0,
             samp_col  = 0;
    using namespace conv;
    CaffeTransformData(chann_cnt, caffe_ln, caffe_col, chann_cnt, input_col_cnt, samp_ln, samp_col, lyr.filter_ln_cnt, lyr.filter_col_cnt, lyr.stride_ln, lyr.stride_col, lyr.dilate_ln, lyr.dilate_col);
    lyr.caffe_ln_cnt  = caffe_ln;
    lyr.caffe_col_cnt = caffe_col;
    input_ln_cnt      = samp_ln;
    input_col_cnt     = samp_col;
}

// pooling layer

struct layer_pool : layer_signal, layer_caffe {
    // max pooling position
    net_set<net_set<net_list<matrix::pos>>> max_pool_pos;
    // pooling type
    int pool_type = NEUNET_POOL_MAX;
};

void layer_create(layer_pool &lyr, int pool_type, int filter_ln_cnt, int filter_col_cnt, int stride_ln_cnt, int stride_col_cnt, int dilate_ln_cnt, int dilate_col_cnt) {
    lyr.pool_type  = pool_type;
    lyr.layer_type = NEUNET_LAYER_POOL;
    layer_create(lyr, filter_ln_cnt, filter_col_cnt, stride_ln_cnt, stride_col_cnt, dilate_ln_cnt, dilate_col_cnt);
}

/**
 * @brief   Pooling FP
 * @param	lyr	[in]	Layer reference
 * @param	input	[in]	Input matrix
 * @param	bat_idx	[in]	Synchronous thread index
 */
void forw_prop(layer_pool &lyr, vect &input, int bat_idx) {
    using namespace conv;
    if (lyr.pool_type == NEUNET_POOL_GAG) input = PoolGlbAvg(input);
    else PoolMaxAvg(lyr.pool_type, CaffeTransform(input, lyr.caffe_data, lyr.caffe_ln_cnt, lyr.caffe_col_cnt), lyr.chann_cnt, lyr.filter_elem_cnt, lyr.max_pool_pos[bat_idx]);
}

/**
 * @brief   Activation layer BP
 * @param	lyr	[in]	Layer reference
 * @param	grad	[in]	BP gradient
 * @param	bat_idx	[in]	Synchronous thread index
 */
void back_prop(layer_pool &lyr, vect &grad, int bat_idx) {
    using namespace conv;
    if (lyr.pool_type == NEUNET_POOL_GAG) grad = GradLossToPoolGlbAvgChann(grad, lyr.in_ln_cnt, lyr.in_col_cnt);
    else CaffeTransform(GradLossToPoolMaxAvgCaffeInput(lyr.pool_type, grad, lyr.filter_elem_cnt, lyr.max_pool_pos[bat_idx]), lyr.caffe_data, lyr.in_elem_cnt, lyr.chann_cnt);
}

// fc layer
struct layer_fc : layer_deriv {
    // learn rate
    double learn_rate = .0;
    // weight
    vect weight,
    // weight transposition
         weight_tp,
    // nesterov weight
         weight_nv;
    // GD
    ada_delta<long double> delta;
    ada_nesterov<long double> nesterov;
    // weight gradient
    net_set<vect> weight_grad;
};

void create_layer() {}

void forw_prop(layer_fc &lyr, vect &input, int bat_idx) {
    using namespace fc;
    lyr.input[bat_idx] = std::move(input);
    if (lyr.learn_rate) input = Output(lyr.input[bat_idx], lyr.weight_nv);
    else input = Output(lyr.input[bat_idx], lyr.weight);
}

void back_prop(layer_fc &lyr, vect &grad, int bat_idx) {
    using namespace fc;
    lyr.weight_grad[bat_idx] = GradLossToWeight(grad, lyr.input[bat_idx].transpose);
    grad = GradLossToInput(grad, lyr.weight_tp);
}

void layer_update(layer_fc &lyr) {
    auto grad = matrix::vect_sum(lyr.weight_grad);
    if (lyr.learn_rate) {
        lyr.weight   -= lyr.nesterov.momentum(grad, lyr.learn_rate);
        lyr.weight_nv = lyr.nesterov.weight(lyr.weight);
    } else lyr.weight   -= lyr.delta.delta(grad);
    lyr.weight_tp = lyr.weight.transpose;
}

// convolution layer

struct layer_conv : layer_fc, layer_caffe {
    // kernel amount
    int kernel_amt = 0;
};

} }

using std::cout;
using std::endl;
using std::string;

using neunet::net_set;
using neunet::vect;

using neunet::async::async_controller;
using neunet::async::async_pool;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.";
    cout << endl;

    

    return EXIT_SUCCESS;
}

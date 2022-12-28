/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

// file main_fn.cpp

#pragma once

#include <iostream>
#include "neunet"

namespace neunet { namespace fn_app {

/* layer data structure */

// layer type code

constexpr uint64_t lyr_act  = 0,
                   lyr_fc   = 1,
                   lyr_conv = 2,
                   lyr_pool = 3,
                   lyr_bn   = 4,
                   lyr_flat = 5;

// layer base

struct layer_base {};
using layer_ptr = std::shared_ptr<layer_base>;

// layer derive

struct layer_derive : layer_base {
    std::atomic_uint64_t batch_size_cnt = 0;

    net_set<vect> input;
};

// layer weight

struct layer_weight : layer_base {    
    long double learn_rate   = .0,
                rand_fst_rng = .0,
                rand_snd_rng = .0;

    uint64_t rand_acc = 0;

    vect weight,
         weight_tp,
         weight_nv;

    net_set<vect> weight_grad;

    ada_delta<long double>    delta;
    ada_nesterov<long double> nesterov;
};

// layer dim

struct layer_dim : layer_base { uint64_t out_ln_cnt = 0; };

// layer flat

struct layer_flat : layer_dim {
    static constexpr uint64_t type = lyr_flat;
    
    uint64_t in_elem_cnt = 0,
             chann_cnt   = 0;
};

void layer_shape(layer_flat &src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t &chann_cnt) {
    src.chann_cnt   = chann_cnt;
    src.in_elem_cnt = in_ln_cnt * in_col_cnt;
    src.out_ln_cnt  = chann_cnt * src.in_elem_cnt;
    in_ln_cnt       = src.out_ln_cnt;
    in_col_cnt      = 1;
    chann_cnt       = 1;
}

void layer_forward(layer_flat &src, vect &input) { input = input.reshape(src.out_ln_cnt, 1); }

void layer_backward(layer_flat &src, vect &grad) { grad = grad.reshape(src.in_elem_cnt, src.chann_cnt); }

void layer_deduce(layer_flat &src, vect &input) { return layer_forward(src, input); }

// layer activation

struct layer_act : layer_derive {
    static constexpr uint64_t type = lyr_act;

    uint64_t act_fn_type = 0;
};

void layer_init(layer_act &src, uint64_t act_fn_type = 0) { src.act_fn_type = act_fn_type; }

void layer_shape(layer_act &src, uint64_t batch_size) { src.input.init(batch_size, false); }

void layer_forward(layer_act &src, vect &input, uint64_t bat_sz_idx) {
    src.input[bat_sz_idx] = std::move(input);
    switch(src.act_fn_type) {
    case NEUNET_SIGMOID: input = sigmoid(src.input[bat_sz_idx]); break;
    case NEUNET_RELU: input = ReLU(src.input[bat_sz_idx]); break;
    case NEUNET_ARELU_LOSS:
    case NEUNET_ARELU: input = AReLU(src.input[bat_sz_idx]); break;
    case NEUNET_SOFTMAX: input = softmax(src.input[bat_sz_idx]); break;
    default: input = std::move(src.input[bat_sz_idx]); break;
    }
}

void layer_backward(layer_act &src, vect &grad, uint64_t bat_sz_idx, const vect &orgn) {
    switch (src.act_fn_type) {
    case NEUNET_SIGMOID: grad = sigmoid_dv(src.input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_RELU: grad = ReLU_dv(src.input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_ARELU: grad = AReLU_dv(src.input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_ARELU_LOSS: grad = AReLU_loss_grad(src.input[bat_sz_idx], grad, orgn); break;
    case NEUNET_SOFTMAX: grad = softmax_cec_grad(grad, orgn); break;
    default: return;
    }
}

void layer_deduce(layer_act &src, vect &input) {
    switch(src.act_fn_type) {
    case NEUNET_SIGMOID: input = sigmoid(input); break;
    case NEUNET_RELU: input = ReLU(input); break;
    case NEUNET_ARELU_LOSS:
    case NEUNET_ARELU: input = AReLU(input); break;
    case NEUNET_SOFTMAX: input = softmax(input); break;
    default: return;
    }
}

struct layer_fc : layer_derive, layer_dim, layer_weight { static constexpr uint64_t type = lyr_flat; };

void layer_init(layer_fc &src, uint64_t out_ln_cnt, long double learn_rate = 0, long double rand_fst_rng = 0, long double rand_snd_rng = 0, uint64_t rand_acc = 0) {
    src.learn_rate   = learn_rate;
    src.out_ln_cnt   = out_ln_cnt;
    src.rand_fst_rng = rand_fst_rng;
    src.rand_snd_rng = rand_snd_rng;
    src.rand_acc     = rand_acc;
}

void layer_shape(layer_fc &src, uint64_t &in_ln_cnt, uint64_t batch_size) {
    src.weight = fc::InitWeight(in_ln_cnt, src.out_ln_cnt, src.rand_fst_rng, src.rand_snd_rng, src.rand_acc);
    if (src.learn_rate) {
        src.weight_nv = src.nesterov.weight(src.weight);
        src.weight_tp = src.weight_nv.transpose;
    } else src.weight_tp = src.weight.transpose;
    src.weight_grad.init(batch_size, false);
    src.input.init(batch_size, false);
    in_ln_cnt = src.out_ln_cnt;
}

void layer_update(layer_fc &src) {
    auto grad = matrix::vect_sum(src.weight_grad).elem_wise_opt(src.weight_grad.length, MATRIX_ELEM_DIV);
    if (src.learn_rate) {
        src.weight   -= src.nesterov.momentum(grad, src.learn_rate);
        src.weight_nv = src.nesterov.weight(src.weight);
        src.weight_tp = src.weight_nv.transpose;
    } else {
        src.weight   -= src.delta.delta(grad);
        src.weight_tp = src.weight.transpose;
    }
}

void layer_forward(layer_fc &src, vect &input, uint64_t bat_sz_idx) {
    using namespace fc;
    src.input[bat_sz_idx] = std::move(input);
    if (src.learn_rate) input = fc::Output(src.input[bat_sz_idx], src.weight_nv);
    else input = fc::Output(src.input[bat_sz_idx], src.weight);
}

void layer_backward(layer_fc &src, vect &grad, uint64_t bat_sz_idx) {
    src.weight_grad[bat_sz_idx] = fc::GradLossToWeight(grad, src.input[bat_sz_idx].transpose);
    grad                        = fc::GradLossToInput(grad, src.weight_tp);
    if (++src.batch_size_cnt == src.input.length) {
        src.batch_size_cnt = 0;
        layer_update(src);
    }
}

void layer_deduce(layer_fc &src, vect &input) { input = fc::Output(input, src.weight); }

// layer caffe

struct layer_caffe : layer_flat {
    uint64_t in_ln_cnt       = 0,
             in_col_cnt      = 0,
             out_col_cnt     = 0,
             ln_stride       = 0,
             col_stride      = 0,
             ln_dilate       = 0,
             col_dilate      = 0,
             filter_ln_cnt   = 0,
             filter_col_cnt  = 0,
             caffe_ln_cnt    = 0,
             caffe_col_cnt   = 0;

    net_set<uint64_t> caffe_data;
};

// layer convolution

struct layer_conv : layer_derive, layer_weight, layer_caffe {
    static constexpr uint64_t type = lyr_conv;

    uint64_t kernel_qty = 0;
};

void layer_init(layer_conv &src, uint64_t kernel_qty, uint64_t kernel_ln_cnt, uint64_t kernel_col_cnt, uint64_t ln_stride, uint64_t col_stride, uint64_t ln_dilate = 0, uint64_t col_dilate = 0, long double learn_rate = 0, long double rand_fst_rng = 0, long double rand_snd_rng = 0, uint64_t rand_acc = 0) {
    src.kernel_qty     = kernel_qty;
    src.filter_ln_cnt  = kernel_ln_cnt;
    src.filter_col_cnt = kernel_col_cnt;
    src.ln_stride      = ln_stride;
    src.col_stride     = col_stride;
    src.ln_dilate      = ln_dilate;
    src.col_dilate     = col_dilate;
    src.learn_rate     = learn_rate;
    src.rand_fst_rng   = rand_fst_rng;
    src.rand_snd_rng   = rand_snd_rng;
    src.rand_acc       = rand_acc;
}

void layer_shape(layer_conv &src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t &chann_cnt, uint64_t batch_size) {
    src.input.init(batch_size, false);
    src.weight_grad.init(batch_size, false);
    src.in_ln_cnt   = in_ln_cnt;
    src.in_col_cnt  = in_col_cnt;
    src.in_elem_cnt = in_ln_cnt * in_col_cnt;
    src.chann_cnt   = chann_cnt;
    src.caffe_data  = conv::CaffeTransformData(chann_cnt, src.caffe_ln_cnt, src.caffe_col_cnt, in_ln_cnt, in_col_cnt, src.out_ln_cnt, src.out_col_cnt, src.filter_ln_cnt, src.filter_col_cnt, src.ln_stride, src.col_stride, src.ln_dilate, src.col_dilate);
    if (!src.weight.verify) src.weight = conv::InitKernel(src.kernel_qty, chann_cnt, src.filter_ln_cnt, src.filter_col_cnt, src.rand_fst_rng, src.rand_snd_rng, src.rand_acc);
    if (src.learn_rate) {
        src.weight_nv = src.nesterov.weight(src.weight);
        src.weight_tp = src.weight_nv.transpose;
    } else src.weight_tp = src.weight.transpose;
    in_ln_cnt  = src.out_ln_cnt;
    in_col_cnt = src.out_col_cnt;
    chann_cnt  = src.kernel_qty;
}

void layer_update(layer_conv &src) {
    auto grad = matrix::vect_sum(src.weight_grad).elem_wise_opt(src.input.length, MATRIX_ELEM_DIV);
    if (src.learn_rate) {
        src.weight   -= src.nesterov.momentum(grad, src.learn_rate);
        src.weight_nv = src.nesterov.weight(src.weight);
        src.weight_tp = src.weight_nv.transpose;
    } else {
        src.weight   -= src.delta.delta(grad);
        src.weight_tp = src.weight.transpose;
    }
}

void layer_forward(layer_conv &src, vect &input, uint64_t bat_sz_idx) {
    src.input[bat_sz_idx] = conv::CaffeTransform(input, src.caffe_data, src.caffe_ln_cnt, src.caffe_col_cnt);
    if (src.learn_rate) input = conv::Conv(src.input[bat_sz_idx], src.weight_nv);
    else input = conv::Conv(src.input[bat_sz_idx], src.weight);
}

void layer_backward(layer_conv &src, vect &grad, uint64_t bat_sz_idx) {
    src.weight_grad[bat_sz_idx] = conv::GradLossToConvKernal(grad, src.input[bat_sz_idx].transpose);
    grad                        = conv::CaffeTransform(conv::GradLossToConvCaffeInput(grad, src.weight_tp), src.caffe_data, src.in_elem_cnt, src.chann_cnt, true);
    if (++src.batch_size_cnt == src.input.length) {
        src.batch_size_cnt = 0;
        layer_update(src);
    }
}

void layer_deduce(layer_conv &src, vect &input) { input = conv::Conv(conv::CaffeTransform(input, src.caffe_data, src.caffe_ln_cnt, src.caffe_col_cnt), src.weight); }

// layer pooling

struct layer_pool : layer_caffe {
    static constexpr uint64_t type = lyr_pool;

    uint64_t pool_type       = NEUNET_POOL_AVG,
             filter_elem_cnt = 0;

    net_set<net_set<net_list<matrix::pos>>> max_pool_pos;
};

void layer_init(layer_pool &src, uint64_t pool_type, uint64_t filter_ln_cnt, uint64_t filter_col_cnt, uint64_t ln_stride, uint64_t col_stride, uint64_t ln_dilate = 0, uint64_t col_dilate = 0) {
    src.pool_type       = pool_type;
    src.filter_ln_cnt   = filter_ln_cnt;
    src.filter_col_cnt  = filter_col_cnt;
    src.filter_elem_cnt = filter_ln_cnt * filter_col_cnt;
    src.ln_stride       = ln_stride;
    src.col_stride      = col_stride;
    src.ln_dilate       = ln_dilate;
    src.col_dilate      = col_dilate;
}

void layer_shape(layer_pool &src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t chann_cnt, uint64_t batch_size) {
    src.in_ln_cnt  = in_ln_cnt;
    src.in_col_cnt = in_col_cnt;
    src.chann_cnt  = chann_cnt;
    if (src.pool_type == NEUNET_POOL_GAG) {
        src.out_ln_cnt  = 1;
        src.out_col_cnt = 1;
    } else {
        src.caffe_data = conv::CaffeTransformData(chann_cnt, src.caffe_ln_cnt, src.caffe_col_cnt, in_ln_cnt, in_col_cnt, src.out_ln_cnt, src.out_col_cnt, src.filter_ln_cnt, src.filter_col_cnt, src.ln_stride, src.col_stride, src.ln_dilate, src.col_dilate);
        src.max_pool_pos.init(batch_size, false);
    }
    in_ln_cnt  = src.out_ln_cnt;
    in_col_cnt = src.out_col_cnt;
}

void layer_forward(layer_pool &src, vect &input, uint64_t bat_sz_idx) {
    if (src.pool_type == NEUNET_POOL_GAG) input = conv::PoolGlbAvg(input);
    else input = conv::PoolMaxAvg(src.pool_type, conv::CaffeTransform(input, src.caffe_data, src.caffe_ln_cnt, src.caffe_col_cnt), src.chann_cnt, src.filter_elem_cnt, src.max_pool_pos[bat_sz_idx]);
}

void layer_backward(layer_pool &src, vect &grad, uint64_t bat_sz_idx) {
    if (src.pool_type == NEUNET_POOL_GAG) grad = conv::GradLossToPoolGlbAvgChann(grad, src.in_ln_cnt, src.in_col_cnt);
    else grad = conv::CaffeTransform(conv::GradLossToPoolMaxAvgCaffeInput(src.pool_type, grad, src.filter_elem_cnt, src.max_pool_pos[bat_sz_idx]), src.caffe_data, src.in_elem_cnt, src.chann_cnt, true);
}

void layer_deduce(layer_pool &src, vect &input) {
    if (src.pool_type == NEUNET_POOL_GAG) layer_forward(src, input, 0);
    else {
        net_set<net_list<matrix::pos>> temp;
        input = conv::PoolMaxAvg(src.pool_type, conv::CaffeTransform(input, src.caffe_data, src.caffe_ln_cnt, src.caffe_col_cnt), src.chann_cnt, src.filter_elem_cnt, temp);
    }
}

// layer BN

struct layer_bn : layer_derive {
    static constexpr uint64_t type = lyr_bn;

    long double mov_avg_decay    = 0.9,
                beta_learn_rate  = 0,
                gamma_learn_rate = 0;

    vect beta,
         gamma,
         beta_grad,
         gamma_grad,
         beta_nv,
         gamma_nv;

    net_set<vect> input_grad;

    BNData<long double> BN_data;

    ada_nesterov<long double> nesterov_beta,
                              nesterov_gamma;

    ada_delta<long double> delta_beta,
                           delta_gamma;

    async::async_controller BN_ctrl;
};

void layer_init(layer_bn &src, vect &&shift = 0, vect &&scale = 1, long double shift_learn_rate = 0, long double scale_learn_rate = 0, long double mov_avg_decay = 0.9) {
    src.beta             = std::move(shift);
    src.gamma            = std::move(scale);
    src.beta_learn_rate  = shift_learn_rate;
    src.gamma_learn_rate = scale_learn_rate;
    src.mov_avg_decay    = mov_avg_decay;
}

void layer_shape(layer_bn &src, uint64_t chann_cnt, uint64_t batch_size, uint64_t batch_cnt) {
    auto beta_v  = src.beta.atom,
         gamma_v = src.gamma.atom;
    if (beta_v) src.beta = BNInitBetaGamma(chann_cnt, beta_v);
    if (gamma_v) src.gamma = BNInitBetaGamma(chann_cnt, gamma_v);
    if (src.beta_learn_rate) src.beta_nv = src.nesterov_beta.weight(src.beta);
    if (src.gamma_learn_rate) src.gamma_nv = src.nesterov_gamma.weight(src.gamma);
    src.input.init(batch_size, false);
    src.input_grad.init(batch_size, false);
    BNInitBNData(src.BN_data, batch_size, batch_cnt);
}

void layer_update(layer_bn &src) {
    if (src.beta_learn_rate) {
        src.beta   -= src.nesterov_beta.momentum(src.beta_grad, src.beta_learn_rate);
        src.beta_nv = src.nesterov_beta.weight(src.beta);
    } else src.beta -= src.delta_beta.delta(src.beta_grad);
    if (src.beta_learn_rate) {
        src.gamma   -= src.nesterov_beta.momentum(src.beta_grad, src.beta_learn_rate);
        src.beta_nv = src.nesterov_beta.weight(src.gamma);
    } else src.gamma -= src.delta_beta.delta(src.beta_grad);
}

void layer_forward(layer_bn &src, vect &input, uint64_t bat_sz_idx) {
    src.input[bat_sz_idx] = std::move(input);
    if (++src.batch_size_cnt == src.input.length) {
        if (src.beta_learn_rate && src.gamma_learn_rate) src.input_grad = BNTrain(src.BN_data, src.input, src.beta_nv, src.gamma_nv);
        else if (src.beta_learn_rate) src.input_grad = BNTrain(src.BN_data, src.input, src.beta_nv, src.gamma);
        else if (src.gamma_learn_rate) src.input_grad = BNTrain(src.BN_data, src.input, src.beta, src.gamma_nv);
        else src.input_grad = BNTrain(src.BN_data, src.input, src.beta, src.gamma);
        src.BN_ctrl.thread_wake_all();
    } else src.BN_ctrl.thread_sleep();
    input = std::move(src.input_grad[bat_sz_idx]);
}

void layer_backward(layer_bn &src, vect &grad, uint64_t bat_sz_idx) {
    src.input_grad[bat_sz_idx] = std::move(grad);
    if (--src.batch_size_cnt) src.BN_ctrl.thread_sleep();
    else {
        if (src.gamma_learn_rate) src.input_grad = BNGradLossToInputGammaBeta(src.BN_data, src.gamma_grad, src.beta_grad, src.input_grad, src.gamma_nv, src.mov_avg_decay);
        else src.input_grad = BNGradLossToInputGammaBeta(src.BN_data, src.gamma_grad, src.beta_grad, src.input_grad, src.gamma, src.mov_avg_decay);
        src.BN_ctrl.thread_wake_all();
        layer_update(src);
    }
    grad = std::move(src.input_grad[bat_sz_idx]);
}

void layer_deduce(layer_bn &src, vect &input) { input = BNDeduce(src.BN_data, input, src.beta, src.gamma); }

} }

using std::cout;
using std::endl;
using std::string;

using neunet::net_set;
using neunet::vect;

using neunet::async::async_controller;
using neunet::async::async_pool;

using namespace neunet::fn_app;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.";
    cout << endl;

    layer_bn lyr;
    layer_init(lyr);
    cout << lyr.beta << endl;

    return EXIT_SUCCESS;
}

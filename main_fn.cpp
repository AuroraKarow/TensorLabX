/* Hello, This is Hatsune ~
 * こんにちは、ハツネちゃんです～　キラー～(∠・ω< )⌒✨
 */

// file main_fn.cpp

#pragma once

#include <iostream>
#include "neunet"

// mnist dataset
#include "dataset"

namespace neunet { namespace fn_app {

/* layer data structure */

// layer type code

constexpr uint64_t lyr_null = 0,
                   lyr_act  = 1,
                   lyr_fc   = 2,
                   lyr_conv = 3,
                   lyr_pool = 4,
                   lyr_bn   = 5,
                   lyr_flat = 6;

// layer base

struct layer_base {
    uint64_t type = 0;
    virtual uint64_t __type__() { return type; }
};
typedef std::shared_ptr<layer_base> layer_base_ptr;
// layer derivative transformation
template<typename layer_derive_t> std::shared_ptr<layer_derive_t> layer_cast(const layer_base_ptr src) { return std::dynamic_pointer_cast<layer_derive_t>(src); };

// layer derive

struct layer_derive : virtual layer_base {
    std::atomic_uint64_t batch_size_cnt = 0;

    net_set<vect> input;
};

// layer weight

struct layer_weight : virtual layer_base {    
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
typedef std::shared_ptr<layer_weight> layer_weight_ptr;

void layer_update(layer_weight_ptr src) {
    auto grad = matrix::vect_sum(src->weight_grad).elem_wise_opt(src->weight_grad.length, MATRIX_ELEM_DIV);
    if (src->learn_rate) {
        src->weight   -= src->nesterov.momentum(grad, src->learn_rate);
        src->weight_nv = src->nesterov.weight(src->weight);
        src->weight_tp = src->weight_nv.transpose;
    } else {
        src->weight   -= src->delta.delta(grad);
        src->weight_tp = src->weight.transpose;
    }
}

// layer dim

struct layer_dim : virtual layer_base { uint64_t out_ln_cnt = 0; };

// layer flat

struct layer_flat : layer_dim {
    uint64_t in_elem_cnt = 0,
             chann_cnt   = 0;
};
typedef std::shared_ptr<layer_flat> layer_flat_ptr;

void layer_init(layer_flat_ptr src) { src->type = lyr_flat; }

void layer_shape(layer_flat_ptr src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t &chann_cnt) {
    src->chann_cnt   = chann_cnt;
    src->in_elem_cnt = in_ln_cnt * in_col_cnt;
    src->out_ln_cnt  = chann_cnt * src->in_elem_cnt;
    in_ln_cnt        = src->out_ln_cnt;
    in_col_cnt       = 1;
    chann_cnt        = 1;
}

void layer_forward(layer_flat_ptr src, vect &input) { input = input.reshape(src->out_ln_cnt, 1); }

void layer_backward(layer_flat_ptr src, vect &grad) { grad = grad.reshape(src->in_elem_cnt, src->chann_cnt); }

void layer_deduce(layer_flat_ptr src, vect &input) { layer_forward(src, input); }

// layer activation

struct layer_act : layer_derive { uint64_t act_fn_type = 0; };
typedef std::shared_ptr<layer_act> layer_act_ptr;

void layer_init(layer_act_ptr src, uint64_t act_fn_type = 0) {
    src->type        = lyr_act;
    src->act_fn_type = act_fn_type;
}

void layer_shape(layer_act_ptr src, uint64_t batch_size) { src->input.init(batch_size, false); }

void layer_forward(layer_act_ptr src, vect &input, uint64_t bat_sz_idx) {
    src->input[bat_sz_idx] = std::move(input);
    switch(src->act_fn_type) {
    case NEUNET_SIGMOID: input = sigmoid(src->input[bat_sz_idx]); break;
    case NEUNET_RELU: input = ReLU(src->input[bat_sz_idx]); break;
    case NEUNET_ARELU_LOSS:
    case NEUNET_ARELU: input = AReLU(src->input[bat_sz_idx]); break;
    case NEUNET_SOFTMAX: input = softmax(src->input[bat_sz_idx]); break;
    default: input = std::move(src->input[bat_sz_idx]); break;
    }
}

void layer_backward(layer_act_ptr src, vect &grad, uint64_t bat_sz_idx, const vect &orgn) {
    switch (src->act_fn_type) {
    case NEUNET_SIGMOID: grad = sigmoid_dv(src->input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_RELU: grad = ReLU_dv(src->input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_ARELU: grad = AReLU_dv(src->input[bat_sz_idx]).elem_wise_opt(grad, MATRIX_ELEM_MULT); break;
    case NEUNET_ARELU_LOSS: grad = AReLU_loss_grad(src->input[bat_sz_idx], grad, orgn); break;
    case NEUNET_SOFTMAX: grad = softmax_cec_grad(grad, orgn); break;
    default: return;
    }
}

void layer_deduce(layer_act_ptr src, vect &input) {
    switch(src->act_fn_type) {
    case NEUNET_SIGMOID: input = sigmoid(input); break;
    case NEUNET_RELU: input = ReLU(input); break;
    case NEUNET_ARELU_LOSS:
    case NEUNET_ARELU: input = AReLU(input); break;
    case NEUNET_SOFTMAX: input = softmax(input); break;
    default: return;
    }
}

struct layer_fc : layer_derive, layer_dim, layer_weight {};
typedef std::shared_ptr<layer_fc> layer_fc_ptr;

void layer_init(layer_fc_ptr src, uint64_t out_ln_cnt, long double learn_rate = 0, long double rand_fst_rng = 0, long double rand_snd_rng = 0, uint64_t rand_acc = 0) {
    src->type         = lyr_fc;
    src->learn_rate   = learn_rate;
    src->out_ln_cnt   = out_ln_cnt;
    src->rand_fst_rng = rand_fst_rng;
    src->rand_snd_rng = rand_snd_rng;
    src->rand_acc     = rand_acc;
}

void layer_shape(layer_fc_ptr src, uint64_t &in_ln_cnt, uint64_t batch_size) {
    src->weight = fc::InitWeight(in_ln_cnt, src->out_ln_cnt, src->rand_fst_rng, src->rand_snd_rng, src->rand_acc);
    if (src->learn_rate) {
        src->weight_nv = src->nesterov.weight(src->weight);
        src->weight_tp = src->weight_nv.transpose;
    } else src->weight_tp = src->weight.transpose;
    src->weight_grad.init(batch_size, false);
    src->input.init(batch_size, false);
    in_ln_cnt = src->out_ln_cnt;
}

void layer_forward(layer_fc_ptr src, vect &input, uint64_t bat_sz_idx) {
    using namespace fc;
    src->input[bat_sz_idx] = std::move(input);
    if (src->learn_rate) input = fc::Output(src->input[bat_sz_idx], src->weight_nv);
    else input = fc::Output(src->input[bat_sz_idx], src->weight);
}

void layer_backward(layer_fc_ptr src, vect &grad, uint64_t bat_sz_idx) {
    src->weight_grad[bat_sz_idx] = fc::GradLossToWeight(grad, src->input[bat_sz_idx].transpose);
    grad                         = fc::GradLossToInput(grad, src->weight_tp);
    if (++src->batch_size_cnt == src->input.length) {
        src->batch_size_cnt = 0;
        layer_update(src);
    }
}

void layer_deduce(layer_fc_ptr src, vect &input) { input = fc::Output(input, src->weight); }

// layer caffe

struct layer_caffe : virtual layer_flat {
    uint64_t in_ln_cnt      = 0,
             in_col_cnt     = 0,
             out_col_cnt    = 0,
             ln_stride      = 0,
             col_stride     = 0,
             ln_dilate      = 0,
             col_dilate     = 0,
             filter_ln_cnt  = 0,
             filter_col_cnt = 0,
             caffe_ln_cnt   = 0,
             caffe_col_cnt  = 0;

    net_set<uint64_t> caffe_data;
};

// layer convolution

struct layer_conv : layer_derive, layer_weight, layer_caffe { uint64_t kernel_qty = 0; };
typedef std::shared_ptr<layer_conv> layer_conv_ptr;

void layer_init(layer_conv_ptr src, uint64_t kernel_qty, uint64_t kernel_ln_cnt, uint64_t kernel_col_cnt, uint64_t ln_stride, uint64_t col_stride, uint64_t ln_dilate = 0, uint64_t col_dilate = 0, long double learn_rate = 0, long double rand_fst_rng = 0, long double rand_snd_rng = 0, uint64_t rand_acc = 0) {
    src->type           = lyr_conv;
    src->kernel_qty     = kernel_qty;
    src->filter_ln_cnt  = kernel_ln_cnt;
    src->filter_col_cnt = kernel_col_cnt;
    src->ln_stride      = ln_stride;
    src->col_stride     = col_stride;
    src->ln_dilate      = ln_dilate;
    src->col_dilate     = col_dilate;
    src->learn_rate     = learn_rate;
    src->rand_fst_rng   = rand_fst_rng;
    src->rand_snd_rng   = rand_snd_rng;
    src->rand_acc       = rand_acc;
}

void layer_shape(layer_conv_ptr src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t &chann_cnt, uint64_t batch_size) {
    src->input.init(batch_size, false);
    src->weight_grad.init(batch_size, false);
    src->in_ln_cnt   = in_ln_cnt;
    src->in_col_cnt  = in_col_cnt;
    src->in_elem_cnt = in_ln_cnt * in_col_cnt;
    src->chann_cnt   = chann_cnt;
    src->caffe_data  = conv::CaffeTransformData(chann_cnt, src->caffe_ln_cnt, src->caffe_col_cnt, in_ln_cnt, in_col_cnt, src->out_ln_cnt, src->out_col_cnt, src->filter_ln_cnt, src->filter_col_cnt, src->ln_stride, src->col_stride, src->ln_dilate, src->col_dilate);
    if (!src->weight.verify) src->weight = conv::InitKernel(src->kernel_qty, chann_cnt, src->filter_ln_cnt, src->filter_col_cnt, src->rand_fst_rng, src->rand_snd_rng, src->rand_acc);
    if (src->learn_rate) {
        src->weight_nv = src->nesterov.weight(src->weight);
        src->weight_tp = src->weight_nv.transpose;
    } else src->weight_tp = src->weight.transpose;
    in_ln_cnt  = src->out_ln_cnt;
    in_col_cnt = src->out_col_cnt;
    chann_cnt  = src->kernel_qty;
}

void layer_forward(layer_conv_ptr src, vect &input, uint64_t bat_sz_idx) {
    src->input[bat_sz_idx] = conv::CaffeTransform(input, src->caffe_data, src->caffe_ln_cnt, src->caffe_col_cnt);
    if (src->learn_rate) input = conv::Conv(src->input[bat_sz_idx], src->weight_nv);
    else input = conv::Conv(src->input[bat_sz_idx], src->weight);
}

void layer_backward(layer_conv_ptr src, vect &grad, uint64_t bat_sz_idx) {
    src->weight_grad[bat_sz_idx] = conv::GradLossToConvKernal(grad, src->input[bat_sz_idx].transpose);
    grad                         = conv::CaffeTransform(conv::GradLossToConvCaffeInput(grad, src->weight_tp), src->caffe_data, src->in_elem_cnt, src->chann_cnt, true);
    if (++src->batch_size_cnt == src->input.length) {
        src->batch_size_cnt = 0;
        layer_update(src);
    }
}

void layer_deduce(layer_conv_ptr src, vect &input) { input = conv::Conv(conv::CaffeTransform(input, src->caffe_data, src->caffe_ln_cnt, src->caffe_col_cnt), src->weight); }

// layer pooling

struct layer_pool : layer_caffe {
    uint64_t pool_type       = 0,
             filter_elem_cnt = 0;

    net_set<net_set<net_list<uint64_t>>> max_pool_pos;
};
typedef std::shared_ptr<layer_pool> layer_pool_ptr;

void layer_init(layer_pool_ptr src, uint64_t pool_type, uint64_t filter_ln_cnt, uint64_t filter_col_cnt, uint64_t ln_stride, uint64_t col_stride, uint64_t ln_dilate = 0, uint64_t col_dilate = 0) {
    src->type            = lyr_pool;
    src->pool_type       = pool_type;
    src->filter_ln_cnt   = filter_ln_cnt;
    src->filter_col_cnt  = filter_col_cnt;
    src->filter_elem_cnt = filter_ln_cnt * filter_col_cnt;
    src->ln_stride       = ln_stride;
    src->col_stride      = col_stride;
    src->ln_dilate       = ln_dilate;
    src->col_dilate      = col_dilate;
}

void layer_shape(layer_pool_ptr src, uint64_t &in_ln_cnt, uint64_t &in_col_cnt, uint64_t chann_cnt, uint64_t batch_size) {
    src->in_ln_cnt   = in_ln_cnt;
    src->in_col_cnt  = in_col_cnt;
    src->in_elem_cnt = in_ln_cnt * in_col_cnt;
    src->chann_cnt   = chann_cnt;
    if (src->pool_type == NEUNET_POOL_GAG) {
        src->out_ln_cnt  = 1;
        src->out_col_cnt = 1;
    } else {
        src->caffe_data = conv::CaffeTransformData(chann_cnt, src->caffe_ln_cnt, src->caffe_col_cnt, in_ln_cnt, in_col_cnt, src->out_ln_cnt, src->out_col_cnt, src->filter_ln_cnt, src->filter_col_cnt, src->ln_stride, src->col_stride, src->ln_dilate, src->col_dilate);
        src->max_pool_pos.init(batch_size, false);
    }
    in_ln_cnt  = src->out_ln_cnt;
    in_col_cnt = src->out_col_cnt;
}

void layer_forward(layer_pool_ptr src, vect &input, uint64_t bat_sz_idx, bool trn_flag = true) { switch (src->pool_type) {
    case NEUNET_POOL_GAG: input = conv::PoolGlbAvg(input); break;
    case NEUNET_POOL_AVG: input = conv::PoolAvg(input, src->caffe_data, src->filter_elem_cnt, src->caffe_ln_cnt); break;
    case NEUNET_POOL_MAX: input = conv::PoolMax(input, src->caffe_data, src->filter_elem_cnt, src->caffe_ln_cnt, src->max_pool_pos[bat_sz_idx], trn_flag); break;
    default: break;
} }

void layer_backward(layer_pool_ptr src, vect &grad, uint64_t bat_sz_idx) { switch (src->pool_type) {
    case NEUNET_POOL_GAG: grad = conv::GradLossToPoolGlbAvgChann(grad, src->in_elem_cnt); break;
    case NEUNET_POOL_AVG: grad = conv::GradLossToPoolAvgChann(grad, src->caffe_data, src->filter_elem_cnt, src->in_elem_cnt); break;
    case NEUNET_POOL_MAX: grad = conv::GradLossToPoolMaxChann(grad, src->in_elem_cnt, src->max_pool_pos[bat_sz_idx]); break;
    default: break;
} }

void layer_deduce(layer_pool_ptr src, vect &input) { layer_forward(src, input, 0, false); }

// layer BN

struct layer_bn : layer_derive {
    std::atomic_uint64_t back_bat_sz_cnt = 0;

    long double mov_avg_decay    = 0.9,
                beta_learn_rate  = 0,
                gamma_learn_rate = 0;

    vect beta,
         gamma,
         beta_nv,
         gamma_nv;

    BNData<long double> BN_data;

    ada_nesterov<long double> nesterov_beta,
                              nesterov_gamma;

    ada_delta<long double> delta_beta,
                           delta_gamma;

    async::async_controller BN_for_ctrl,
                            BN_back_ctrl;
};
typedef std::shared_ptr<layer_bn> layer_bn_ptr;

void layer_init(layer_bn_ptr src, vect &&shift = 0, vect &&scale = 1, long double shift_learn_rate = 0, long double scale_learn_rate = 0, long double mov_avg_decay = 0.9) {
    src->type             = lyr_bn;
    src->beta             = std::move(shift);
    src->gamma            = std::move(scale);
    src->beta_learn_rate  = shift_learn_rate;
    src->gamma_learn_rate = scale_learn_rate;
    src->mov_avg_decay    = mov_avg_decay;
}

void layer_shape(layer_bn_ptr src, uint64_t chann_cnt, uint64_t batch_size, uint64_t batch_cnt) {
    auto beta_v  = src->beta.atom,
         gamma_v = src->gamma.atom;
    src->beta = BNInitBetaGamma(chann_cnt, beta_v);
    src->gamma = BNInitBetaGamma(chann_cnt, gamma_v);
    if (beta_v) src->beta.fill_elem(beta_v);
    if (gamma_v) src->gamma.fill_elem(gamma_v);
    if (src->beta_learn_rate) src->beta_nv = src->nesterov_beta.weight(src->beta);
    if (src->gamma_learn_rate) src->gamma_nv = src->nesterov_gamma.weight(src->gamma);
    src->input.init(batch_size, false);
    src->BN_data.BNDataInit(batch_size, batch_cnt);
}

void layer_update(layer_bn_ptr src, vect beta_grad, vect gamma_grad) {
    if (src->beta_learn_rate) {
        src->beta   -= src->nesterov_beta.momentum(beta_grad, src->beta_learn_rate);
        src->beta_nv = src->nesterov_beta.weight(src->beta);
    } else src->beta -= src->delta_beta.delta(beta_grad);
    if (src->gamma_learn_rate) {
        src->gamma   -= src->nesterov_gamma.momentum(gamma_grad, src->gamma_learn_rate);
        src->gamma_nv = src->nesterov_gamma.weight(src->gamma);
    } else src->gamma -= src->delta_gamma.delta(gamma_grad);
}

void layer_forward(layer_bn_ptr src, vect &input, uint64_t bat_sz_idx) {
    src->input[bat_sz_idx] = std::move(input);
    if (++src->batch_size_cnt == src->input.length) {
        src->input = BNTrain(src->BN_data, src->input, (src->beta_learn_rate ? src->beta_nv : src->beta), (src->gamma_learn_rate ? src->gamma_nv : src->gamma));
        src->batch_size_cnt = 0;
        src->BN_for_ctrl.thread_wake_all();
    } else while (src->batch_size_cnt) src->BN_for_ctrl.thread_sleep(1000);
    input = std::move(src->input[bat_sz_idx]);
}

void layer_backward(layer_bn_ptr src, vect &grad, uint64_t bat_sz_idx) {
    src->input[bat_sz_idx] = std::move(grad);
    if (++src->back_bat_sz_cnt == src->input.length) {
        vect beta_grad, gamma_grad;
        src->input = BNGradLossToInputGammaBeta(src->BN_data, gamma_grad, beta_grad, src->input, (src->gamma_learn_rate ? src->gamma_nv : src->gamma), src->mov_avg_decay);
        src->back_bat_sz_cnt = 0;
        src->BN_back_ctrl.thread_wake_all();
        layer_update(src, beta_grad, gamma_grad);
    } else while (src->back_bat_sz_cnt) src->BN_back_ctrl.thread_sleep(1000);
    grad = std::move(src->input[bat_sz_idx]);
}

void layer_deduce(layer_bn_ptr src, vect &input) { input = BNDeduce(src->BN_data, input, src->beta, src->gamma); }

template <typename layer_type, typename ... layer_paras> void net_add_layer(net_sequence<layer_base_ptr> &net_lyr, layer_paras &&...args) {
    // constraint
    static_assert(std::is_same_v<layer_type, layer_act>  ||
                  std::is_same_v<layer_type, layer_fc>   ||
                  std::is_same_v<layer_type, layer_flat> ||
                  std::is_same_v<layer_type, layer_conv> ||
                  std::is_same_v<layer_type, layer_pool> ||
                  std::is_same_v<layer_type, layer_bn>,
                  "Should derive from data structure <layer_base>");
    auto curr_lyr_idx = net_lyr.length;
    net_lyr.emplace_back(std::make_shared<layer_type>());
    layer_init(layer_cast<layer_type>(net_lyr[curr_lyr_idx]), std::forward<layer_paras>(args)...);
}

void net_shape_init(net_sequence<layer_base_ptr> &net_lyr, uint64_t in_ln_cnt, uint64_t in_col_cnt, uint64_t chann_cnt, uint64_t batch_size, uint64_t batch_cnt) {
    for (auto i = 0ull; i < net_lyr.length; ++i) switch (net_lyr[i]->type) {
    case lyr_act: layer_shape(layer_cast<layer_act>(net_lyr[i]), batch_size); break;
    case lyr_fc: layer_shape(layer_cast<layer_fc>(net_lyr[i]), in_ln_cnt, batch_size); break;
    case lyr_conv: layer_shape(layer_cast<layer_conv>(net_lyr[i]), in_ln_cnt, in_col_cnt, chann_cnt, batch_size); break;
    case lyr_flat: layer_shape(layer_cast<layer_flat>(net_lyr[i]), in_ln_cnt, in_col_cnt, chann_cnt); break;
    case lyr_pool: layer_shape(layer_cast<layer_pool>(net_lyr[i]), in_ln_cnt, in_col_cnt, chann_cnt, batch_size); break;
    case lyr_bn: layer_shape(layer_cast<layer_bn>(net_lyr[i]), chann_cnt, batch_size, batch_cnt); break;
    default: break;
    }
}

void net_forward(net_sequence<layer_base_ptr> &net_lyr, vect &input, uint64_t bat_sz_idx) {
    for (auto i = 0ull; i < net_lyr.length; ++i) switch (net_lyr[i]->type) {
    case lyr_act: layer_forward(layer_cast<layer_act>(net_lyr[i]), input, bat_sz_idx); break;
    case lyr_fc: layer_forward(layer_cast<layer_fc>(net_lyr[i]), input, bat_sz_idx); break;
    case lyr_conv: layer_forward(layer_cast<layer_conv>(net_lyr[i]), input, bat_sz_idx); break;
    case lyr_flat: layer_forward(layer_cast<layer_flat>(net_lyr[i]), input); break;
    case lyr_pool: layer_forward(layer_cast<layer_pool>(net_lyr[i]), input, bat_sz_idx); break;
    case lyr_bn: layer_forward(layer_cast<layer_bn>(net_lyr[i]), input, bat_sz_idx); break;
    default: break;
    }
}

void net_backward(net_sequence<layer_base_ptr> &net_lyr, vect &grad, uint64_t bat_sz_idx, const vect &orgn) {
    for (auto idx = net_lyr.length; idx; --idx) {
        auto i = idx - 1;
        switch (net_lyr[i]->type) {
        case lyr_act: layer_backward(layer_cast<layer_act>(net_lyr[i]), grad, bat_sz_idx, orgn); break;
        case lyr_fc: layer_backward(layer_cast<layer_fc>(net_lyr[i]), grad, bat_sz_idx); break;
        case lyr_conv: layer_backward(layer_cast<layer_conv>(net_lyr[i]), grad, bat_sz_idx); break;
        case lyr_flat: layer_backward(layer_cast<layer_flat>(net_lyr[i]), grad); break;
        case lyr_pool: layer_backward(layer_cast<layer_pool>(net_lyr[i]), grad, bat_sz_idx); break;
        case lyr_bn: layer_backward(layer_cast<layer_bn>(net_lyr[i]), grad, bat_sz_idx); break;
        default: break;
        }
    }
}

void net_deduce(net_sequence<layer_base_ptr> &net_lyr, vect &input) {
    for (auto i = 0ull; i < net_lyr.length; ++i) switch (net_lyr[i]->type) {
    case lyr_act: layer_deduce(layer_cast<layer_act>(net_lyr[i]), input); break;
    case lyr_fc: layer_deduce(layer_cast<layer_fc>(net_lyr[i]), input); break;
    case lyr_conv: layer_deduce(layer_cast<layer_conv>(net_lyr[i]), input); break;
    case lyr_flat: layer_deduce(layer_cast<layer_flat>(net_lyr[i]), input); break;
    case lyr_pool: layer_deduce(layer_cast<layer_pool>(net_lyr[i]), input); break;
    case lyr_bn: layer_deduce(layer_cast<layer_bn>(net_lyr[i]), input); break;
    default: break;
    }
}

} }

using std::cout;
using std::endl;
using std::string;

using neunet::net_set;
using neunet::net_sequence;
using neunet::vect;

using neunet::async::async_controller;
using neunet::async::async_pool;
using neunet::async::net_queue;

using neunet::dataset::mnist;

using namespace neunet::fn_app;

int main(int argc, char *argv[], char *envp[]) {
    cout << "hello, world.";
    cout << endl;
    
    // learn rate
    long double learn_rate = .4;
    // train & test data batch size
    uint64_t trn_bat_sz = 125, tst_bat_sz = 125;
    // train precision
    long double trn_prec = .1;
    // network status code
    std::atomic_uint64_t net_stat   = NEUNET_STAT_NRM,
    // batch size counter
                         bat_sz_cnt = 0,
    // accuracy counter
                         acc_cnt    = 0,
    // recall counter
                         rc_cnt     = 0;
    // accuracy & recall count of train & test
    net_queue<uint64_t> trn_acc, trn_rc, tst_acc, tst_rc;
    // threads pool
    auto pool_sz = trn_bat_sz > tst_bat_sz ? trn_bat_sz : tst_bat_sz;
    async_pool pool(pool_sz);
    // asynchronous process controller of train & test
    async_controller trn_ctrl, tst_ctrl;
    // network layer - LeNet-5
    net_sequence<layer_base_ptr> net_lyr;
    // C0
    net_add_layer<layer_conv>(net_lyr, 20, 5, 5, 1, 1, 0, 0, learn_rate);
    net_add_layer<layer_bn>(net_lyr, 0, 1, 1e-5l, 1e-5l);
    net_add_layer<layer_act>(net_lyr, NEUNET_RELU);
    // P1
    net_add_layer<layer_pool>(net_lyr, NEUNET_POOL_AVG, 2, 2, 2, 2);
    // C2
    net_add_layer<layer_conv>(net_lyr, 50, 5, 5, 1, 1, 0, 0, learn_rate);
    net_add_layer<layer_bn>(net_lyr, 0, 1, 1e-5l, 1e-5l);
    net_add_layer<layer_act>(net_lyr, NEUNET_RELU);
    // P3
    net_add_layer<layer_pool>(net_lyr, NEUNET_POOL_AVG, 2, 2, 2, 2);
    net_add_layer<layer_flat>(net_lyr);
    // F4
    net_add_layer<layer_fc>(net_lyr, 500, learn_rate);
    net_add_layer<layer_bn>(net_lyr, 0, 1, 1e-5l, 1e-5l);
    net_add_layer<layer_act>(net_lyr, NEUNET_SIGMOID);
    // F5
    net_add_layer<layer_fc>(net_lyr, 10, learn_rate);
    net_add_layer<layer_bn>(net_lyr, 0, 1, 1e-5l, 1e-5l);
    net_add_layer<layer_act>(net_lyr, NEUNET_SOFTMAX);
    
    /* train & test dataset, built-in data structure "mnist" */

    // root directory of mnist dataset
    // std::string root = "...\\MNIST\\";
    std::string root = "E:\\VS Code project data\\MNIST\\";
    // load train & test dataset
    mnist train((root + "train-images.idx3-ubyte").c_str(), (root + "train-labels.idx1-ubyte").c_str()), 
    test((root + "t10k-images.idx3-ubyte").c_str(), (root + "t10k-labels.idx1-ubyte").c_str());
    // train batch count
    auto trn_bat_cnt = train.element_count / trn_bat_sz;
    
    // mnist shape
    net_shape_init(net_lyr, train.element_line_count, train.element_column_count, 1, trn_bat_sz, trn_bat_cnt);

    // mnist train & deduce
    for (auto i = 0ull; i < pool_sz; ++i) pool.add_task([&net_lyr, &train, &test, &bat_sz_cnt, &net_stat, &trn_ctrl, &tst_ctrl, &acc_cnt, &rc_cnt, &trn_acc, &trn_rc, &tst_acc, &tst_rc, pool_sz, i, trn_bat_sz, tst_bat_sz, trn_bat_cnt, trn_prec](uint64_t epoch, uint64_t tst_bat_cnt){ while (net_stat == NEUNET_STAT_NRM){
        // current data index value of process
        uint64_t data_idx = i,
        // batch count
                 bat_cnt  = 0;
        // last thread
        bool last_tkn = false;
        // train
        while (bat_cnt < trn_bat_cnt && i < trn_bat_sz) {
            // get train data and corresponding labels of current batch index
            auto input = train.elem[train.data_idx[data_idx]];
            auto lbl   = train.lbl[train.data_idx[data_idx]];
            auto orgn  = neunet::lbl_orgn(lbl, mnist_orgn_size);
            // next batch data index
            data_idx  += trn_bat_sz;
            if (bat_cnt || epoch) {
                if (last_tkn) last_tkn = false;
                // if it is not last arriving thread, it need to wait, 1000ms at most
                else trn_ctrl.thread_sleep(1000);
                if (net_stat == NEUNET_STAT_END) break;
            }
            // FP
            net_forward(net_lyr, input, i);
            neunet::output_acc_rc(input, trn_prec, lbl, acc_cnt, rc_cnt);
            // next batch
            ++bat_cnt;
            // BP
            net_backward(net_lyr, input, i, orgn);
            if (++bat_sz_cnt == trn_bat_sz) {
                // last thread marked
                bat_sz_cnt = 0;
                last_tkn   = true;
                // save accuracy & recall count value
                trn_acc.en_queue(acc_cnt);
                trn_rc.en_queue(rc_cnt);
                acc_cnt = 0;
                rc_cnt  = 0;
                if (bat_cnt == trn_bat_cnt) {
                    // shuffle train data index
                    train.data_idx.shuffle();
                    // activate testing process
                    tst_ctrl.thread_wake_all();
                // otherwise activate training process of next batch
                } else trn_ctrl.thread_wake_all();
            }
        }
        if (net_stat == NEUNET_STAT_END) break;
        // reset value
        data_idx = i;
        bat_cnt  = 0;
        // deduce
        while (bat_cnt < tst_bat_cnt && i < tst_bat_sz) {
            // get test data and corresponding labels of current batch
            auto input = test.elem[data_idx];
            auto lbl   = test.lbl[data_idx];
            // process which is not the last arriving thread should wait, 1000ms at most
            if (!(bat_cnt || last_tkn)) tst_ctrl.thread_sleep();
            if (net_stat == NEUNET_STAT_END) break;
            // test
            net_deduce(net_lyr, input);
            neunet::output_acc_rc(input, trn_prec, lbl, acc_cnt, rc_cnt);
            // next batch data index
            data_idx += trn_bat_sz;
            ++bat_cnt;
        }
        // next epoch
        if (net_stat == NEUNET_STAT_END) break;
        ++epoch;
        if (++bat_sz_cnt == pool_sz) {
            bat_sz_cnt = 0;
            // save accuracy & recall count value
            tst_acc.en_queue(acc_cnt);
            tst_rc.en_queue(rc_cnt);
            acc_cnt = 0;
            rc_cnt  = 0;
            // activate next epoch train process
            trn_ctrl.thread_wake_all();
        }
    } }, 0ull, test.element_count / tst_bat_sz);
    
    // recall rate of train & test
    long double rc_rt = .0;
    // epoch count
    uint64_t ep_cnt = 0;
    // train & test output data show
    while (rc_rt < 1) {
        auto ep_tm_pt = NEUNET_CHRONO_TIME_POINT;
        // train
        for (auto i = 0ull; i < trn_bat_cnt; ++i) {
            auto tm_pt = NEUNET_CHRONO_TIME_POINT;
            auto acc   = trn_acc.de_queue() / (trn_bat_sz * 1.);
            rc_rt      = trn_rc.de_queue() / (trn_bat_sz * 1.);
            neunet::print_train_progress((i + 1), trn_bat_cnt, acc, rc_rt, (NEUNET_CHRONO_TIME_POINT - tm_pt));
        }
        // deduce
        std::printf("\r[Deducing]...");
        auto acc = tst_acc.de_queue() / (test.element_count * 1.);
        rc_rt    = tst_rc.de_queue() / (test.element_count * 1.);
        neunet::print_epoch_status(++ep_cnt, acc, rc_rt, (NEUNET_CHRONO_TIME_POINT - ep_tm_pt));
    }
    return EXIT_SUCCESS;
}

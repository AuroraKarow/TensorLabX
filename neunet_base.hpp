NEUNET_BEGIN

callback_matrix neunet_vect vec_travel(const neunet_vect &src, matrix_elem_t(*func)(const matrix_elem_t &)) {
    auto ans = src;
    for (auto i = 0ull; i < src.element_count; ++i) ans.index(i) = func(ans.index(i));
    return ans;
}

callback_matrix neunet_vect divisor_dominate(const neunet_vect &src, const matrix_elem_t &epsilon = 1e-8l) {
    auto ans = src;
    for (auto i = 0ull; i < src.element_count; ++i) if (ans.index(i) == 0) ans.index(i) = epsilon;
    return ans;
}

callback_matrix matrix_elem_t sigmoid(const matrix_elem_t &src) { return 1 / (1 + 1 / std::exp(src)); }
callback_matrix neunet_vect sigmoid(const neunet_vect &src) { return vec_travel(src, sigmoid); }

callback_matrix matrix_elem_t sigmoid_dv(const matrix_elem_t &src) { { return sigmoid(src) * (1 - sigmoid(src)); } }
callback_matrix neunet_vect sigmoid_dv(const neunet_vect &src) { return vec_travel(src, sigmoid_dv); }

callback_matrix matrix_elem_t ReLU(const matrix_elem_t &src) { return src < 0 ? 0 : src; }
callback_matrix neunet_vect ReLU(const neunet_vect &src) { return vec_travel(src, ReLU); }

callback_matrix matrix_elem_t ReLU_dv(const matrix_elem_t &src) { return src < 0 ? 0 : 1; }
callback_matrix neunet_vect ReLU_dv(const neunet_vect &src) { return vec_travel(src, ReLU_dv); }

callback_matrix matrix_elem_t AReLU(const matrix_elem_t &src) { return src < 1 ? 0 : src; }
callback_matrix neunet_vect AReLU(const neunet_vect &src) { return vec_travel(src, AReLU); }

callback_matrix matrix_elem_t AReLU_dv(const matrix_elem_t &src) { return src < 1 ? 0 : 1; }
callback_matrix neunet_vect AReLU_dv(const neunet_vect &src) { return vec_travel(src, AReLU_dv); }

callback_matrix neunet_vect AReLU_loss_grad(const neunet_vect &AReLU_input, const neunet_vect &AReLU_output, const neunet_vect &origin) { return (origin - AReLU_output).abs.elem_wise_opt(AReLU_dv(AReLU_input), MATRIX_ELEM_MULT); }

callback_matrix neunet_vect softmax(const neunet_vect &src) {
    neunet_vect ans(src.line_count, src.column_count);
    matrix_elem_t sum = 0;
    for (auto i = 0ull; i < src.element_count; ++i) sum += std::exp(src.index(i));
    for (auto i = 0ull; i < src.element_count; ++i) ans.index(i) = std::exp(src.index(i)) / sum;
    return ans;
}

callback_matrix neunet_vect softmax_cec_grad(const neunet_vect &softmax_output, const neunet_vect &origin) {
    auto out_grad = origin.elem_wise_opt(softmax_output, MATRIX_ELEM_DIV);
    auto sum_grad = out_grad.elem_wise_opt(softmax_output, MATRIX_ELEM_MULT).elem_sum();
    for (auto i = 0ull; i < out_grad.element_count; ++i) out_grad.index(i) = sum_grad - out_grad.index(i);
    return out_grad.elem_wise_opt(softmax_output, MATRIX_ELEM_MULT);
}

uint64_t samp_block_cnt(uint64_t filter_dir_cnt, uint64_t dir_dilate) { return (dir_dilate + 1) * filter_dir_cnt - dir_dilate; }

uint64_t samp_trace_pos(uint64_t output_dir_pos, uint64_t filter_dir_pos, uint64_t dir_stride, uint64_t dir_dilate) { return output_dir_pos * dir_stride + filter_dir_pos * (1 + dir_dilate);}

uint64_t samp_output_dir_cnt(uint64_t input_dir_cnt, uint64_t filter_dir_cnt, uint64_t dir_stride, uint64_t dir_dilate) { return (input_dir_cnt - samp_block_cnt(filter_dir_cnt, dir_dilate)) / dir_stride + 1; }

uint64_t samp_input_dir_cnt(uint64_t output_dir_cnt, uint64_t filter_dir_cnt, uint64_t dir_stride, uint64_t dir_dilate) { return (output_dir_cnt - 1) * dir_stride + samp_block_cnt(filter_dir_cnt, dir_dilate); }

bool samp_valid(uint64_t input_dir_cnt, uint64_t filter_dir_cnt, uint64_t dir_stride, uint64_t dir_dilate) { return (input_dir_cnt - samp_block_cnt(filter_dir_cnt, dir_dilate)) % dir_stride == 0; }

callback_matrix neunet_vect chann_vec_pad(uint64_t &ans_ln_cnt, uint64_t &ans_col_cnt, const neunet_vect &chann_vec, uint64_t chann_ln_cnt, uint64_t chann_col_cnt, uint64_t top_cnt, uint64_t right_cnt, uint64_t bottom_cnt, uint64_t left_cnt, uint64_t ln_dist, uint64_t col_dist) {
    if (top_cnt || right_cnt || bottom_cnt || left_cnt || ln_dist || col_dist) {
        if (!ans_ln_cnt) ans_ln_cnt = matrix::pad_res_dir_cnt(top_cnt, bottom_cnt, chann_ln_cnt, ln_dist);
        if (!ans_col_cnt) ans_col_cnt = matrix::pad_res_dir_cnt(left_cnt, right_cnt, chann_col_cnt, col_dist);
        neunet_vect ans(ans_ln_cnt * ans_col_cnt, chann_vec.column_count);
        for (auto i = 0ull; i < chann_vec.element_count; ++i) {
            auto axis = matrix::elem_pos(i, chann_vec.column_count),
                 dim  = matrix::elem_pos(axis.ln, chann_col_cnt);
            ans[matrix::elem_pos(top_cnt + dim.ln * (ln_dist + 1), left_cnt + dim.col * (col_dist + 1), ans_col_cnt)][axis.col] = chann_vec.index(i);
        }
        return ans;
    } else {
        ans_ln_cnt  = chann_ln_cnt;
        ans_col_cnt = chann_col_cnt;
        return chann_vec;
    }
}

callback_matrix neunet_vect chann_vec_crop(uint64_t &ans_ln_cnt, uint64_t &ans_col_cnt, const neunet_vect &chann_vec, uint64_t chann_ln_cnt, uint64_t chann_col_cnt, uint64_t top_cnt, uint64_t right_cnt, uint64_t bottom_cnt, uint64_t left_cnt, uint64_t ln_dist, uint64_t col_dist) {
    if (top_cnt || right_cnt || bottom_cnt || left_cnt || ln_dist || col_dist) {
        if (!ans_ln_cnt) ans_ln_cnt = matrix::crop_res_dir_cnt(top_cnt, bottom_cnt, chann_ln_cnt, ln_dist);
        if (!ans_col_cnt) ans_col_cnt = matrix::crop_res_dir_cnt(left_cnt, right_cnt, chann_col_cnt, col_dist);
        neunet_vect ans(ans_ln_cnt * ans_col_cnt, chann_vec.column_count);
        for (auto i = 0ull; i < ans.element_count; ++i) {
            auto axis = matrix::elem_pos(i, ans.column_count),
                 dim  = matrix::elem_pos(axis.ln, ans_col_cnt);
            ans.index(i) = chann_vec[matrix::elem_pos(top_cnt + dim.ln * (ln_dist + 1), left_cnt + dim.col * (col_dist + 1), chann_col_cnt)][axis.col];
        }
        return ans;
    } else {
        ans_ln_cnt  = chann_ln_cnt;
        ans_col_cnt = chann_col_cnt;
        return chann_vec;
    }
}

void print_train_progress(int curr_prog, int prog, long double acc, long double rc, int dur) { std::printf("\r[Train][%d/%d][Acc/Rc][%.2f/%.2f][%dms]", curr_prog, prog, acc, rc, dur); }

void print_epoch_status(int epoch, long double acc, long double rc, int dur) {
    std::printf("\r[Epoch][%d][Acc/Rc][%.4f/%.4f][%dms]", epoch, acc, rc, dur);
    std::cout << std::endl;
}

callback_matrix void print_output_status(const neunet_vect &output, uint64_t lbl) {
    std::cout << " [No.]\t[Output]\t[Origin]\n";
    for (auto i = 0ull; i < output.element_count; ++i) {
        if (i == lbl) std::cout << '>';
        else std::cout << ' ';
        std::cout << i << '\t' << output.index(i) << '\t';
        if (i == lbl) std::cout << 1 << '\n';
        else std::cout << 0 << '\n';
    }
}
callback_matrix void print_output_status(const net_set<neunet_vect> &output, const net_set<uint64_t> &lbl) {
    if (output.length != lbl.length) return;
    for (auto i = 0ull; i < output.length; ++i) {
        output_para(output[i], lbl[i]);
        std::cout << std::endl;
    }
}

callback_matrix void print_output(const neunet_vect &output, uint64_t lbl) {
    std::cout << " [No.]\t[Output]\t[Origin]\n";
    for (auto i = 0ull; i < output.element_count; ++i) {
        if (i == lbl) std::cout << '>';
        else std::cout << ' ';
        std::cout << i << '\t' << output.index(i) << '\t';
        if (i == lbl) std::cout << 1 << '\n';
        else std::cout << 0 << '\n';
    }
}
callback_matrix void print_output(const net_set<neunet_vect> &output, const net_set<uint64_t> &lbl) {
    if (output.length != lbl.length) return;
    for (auto i = 0ull; i < output.length; ++i) {
        print_output(output[i], lbl[i]);
        std::cout << std::endl;
    }
}

vect lbl_orgn(uint64_t lbl_val, uint64_t type_cnt) {
    vect ans(type_cnt, 1);
    ans.index(lbl_val) = 1;
    return ans;
}
net_set<vect> lbl_orgn(const net_set<uint64_t> &lbl_set, uint64_t type_cnt) {
    net_set<vect> ans(lbl_set.length);
    for (auto i = 0ull; i < ans.length; ++i) ans[i] = lbl_orgn(lbl_set[i], type_cnt);
    return ans;
}

callback_matrix void output_acc_rc(const neunet_vect &output, long double train_acc, uint64_t lbl, std::atomic_uint64_t &acc_cnt, std::atomic_uint64_t &rc_cnt) {
    if (output.index(lbl) > 0.5) ++acc_cnt;
    if (output.index(lbl) > (1 - train_acc)) ++rc_cnt;
}

matrix_declare struct ada_delta final {
private:
    void value_assign(const ada_delta &src) {
        rho     = src.rho;
        epsilon = src.epsilon;
    }

    void value_copy(const ada_delta &src) {
        value_assign(src);
        exp_grad  = src.exp_grad;
        exp_delta = src.exp_delta;
    }

    void value_move(ada_delta &&src) {
        value_assign(src);
        exp_grad  = std::move(src.exp_grad);
        exp_delta = std::move(src.exp_delta);
        src.reset();
    }

public:
    ada_delta() {}
    ada_delta(const ada_delta &src) { value_copy(src); }
    ada_delta(ada_delta &&src) { value_move(std::move(src)); }

    neunet_vect delta(const neunet_vect &curr_grad) {
        if (!exp_grad.verify) exp_grad = neunet_vect(curr_grad.line_count, curr_grad.column_count);
        if (!exp_delta.verify) exp_delta = neunet_vect(curr_grad.line_count, curr_grad.column_count);
        auto curr_grad_pw  = curr_grad;
        for (auto i = 0ull; i < curr_grad.element_count; ++i) curr_grad_pw.index(i) *= curr_grad_pw.index(i);
             exp_grad      = rho * exp_grad + (1 - rho) * curr_grad_pw;
        auto rms_exp_delta = divisor_dominate(exp_delta, epsilon).elem_wise_opt(0.5, MATRIX_ELEM_POW),
             rms_exp_grad  = divisor_dominate(exp_grad, epsilon).elem_wise_opt(0.5, MATRIX_ELEM_POW),
             curr_delta    = rms_exp_delta.elem_wise_opt(rms_exp_grad, MATRIX_ELEM_DIV).elem_wise_opt(curr_grad, MATRIX_ELEM_MULT);
        auto curr_delta_pw = curr_delta;
        for (auto i = 0ull; i < curr_delta.element_count; ++i) curr_delta_pw.index(i) *= curr_delta_pw.index(i);
             exp_delta     = rho * exp_delta + (1 - rho) * curr_delta_pw;
        return curr_delta;
    }

    void reset() {
        rho     = 0.95;
        epsilon = 1e-8;

        exp_grad.reset();
        exp_delta.reset();
    }

    ~ada_delta() { reset(); }

private:
    neunet_vect exp_grad,
                exp_delta;

public:
    long double rho     = 0.95,
                epsilon = 1e-8;

    ada_delta &operator=(const ada_delta &src) {
        value_copy(src);
        return *this;
    }
    ada_delta &operator=(ada_delta &&src) {
        value_move(std::move(src));
        return *this;
    }

    bool operator==(const ada_delta &src) const { return exp_grad == src.exp_grad && exp_delta == src.exp_delta && rho == src.rho && epsilon == src.epsilon; }

    bool operator!=(const ada_delta &src) const { return !(*this == src); }
                
};

matrix_declare struct ada_nesterov final {
private:
    void value_copy(const ada_nesterov &src) {
        rho      = src.rho;
        velocity = src.velocity;
    }

    void value_move(ada_nesterov &&src) {
        rho      = src.rho;
        velocity = std::move(src.velocity);
    }

public:
    ada_nesterov() {}
    ada_nesterov(const ada_nesterov &src) { value_copy(src); }
    ada_nesterov(ada_nesterov &&src) { value_move(std::move(src)); }

    neunet_vect weight(const neunet_vect &curr_weight) const {
        if (velocity.verify) return curr_weight - rho * velocity;
        else return curr_weight;
    }

    neunet_vect momentum(const neunet_vect &curr_grad, long double learn_rate) {
        if (velocity.verify) velocity = rho * velocity + learn_rate * curr_grad;
        else velocity = learn_rate * curr_grad;
        return velocity;
    }

    void reset() {
        rho = 0.9;
        velocity.reset();
    }

    ~ada_nesterov() { reset(); }

private: neunet_vect velocity;

public: 
    long double rho = 0.9;

    ada_nesterov &operator=(ada_nesterov &&src) {
        value_move(std::move(src));
        return *this;
    }
    ada_nesterov &operator=(const ada_nesterov &src) {
        value_copy(src);
        return *this;
    }

    bool operator=(const ada_nesterov &src) const { return rho == src.rho && velocity == src.velocity; }

    bool operator!=(const ada_nesterov &src) const { return !(*this == src); }

};

callback_matrix neunet_vect im2col_trans(const net_set<neunet_vect> &chann_vec) {
    neunet_vect ans(chann_vec[0].element_count, chann_vec.length);
    for (auto i = 0ull; i < chann_vec.length; ++i) for (auto j = 0ull; j < chann_vec[i].element_count; ++j) ans[j][i] = chann_vec[i].index(j);
    return ans;
}
callback_matrix net_set<neunet_vect> im2col_trans(const neunet_vect &chann, uint64_t chann_ln_cnt, uint64_t chann_col_cnt) {
    auto chann_elem_cnt = chann_ln_cnt * chann_col_cnt;
    net_set<neunet_vect> ans;
    if (chann.line_count != chann_elem_cnt) return ans;
    ans.init(chann.column_count);
    for (auto i = 0ull; i < ans.length; ++i) {
        ans[i] = neunet_vect(chann_ln_cnt, chann_col_cnt);
        for (auto j = 0ull; j < chann_elem_cnt; ++j) ans[i].index(j) = chann[j][i];
    }
    return ans;
}

NEUNET_END

LAYER_BEGIN

struct Layer {
    const uint64_t iLayerType = 0;
    
    Layer(uint64_t iLayerType = NEUNET_LAYER_NULL) : iLayerType(iLayerType) {}

    virtual ~Layer() {}
};

matrix_declare struct LayerWeight : virtual Layer {
    std::atomic_uint64_t iBatSzCnt = 0;

    long double dLearnRate  = .0,
                dRandFstRng = .0,
                dRandSndRng = .0;

    neunet_vect vecWeight,
                vecWeightTp,
                vecWeightNv;

    net_set<neunet_vect> setWeightGrad;

    ada_delta<matrix_elem_t> adaDelta;

    ada_nesterov<matrix_elem_t> advNesterov;

    void ValueAssign(const LayerWeight &lyrSrc) {
        iBatSzCnt   = (uint64_t)lyrSrc.iBatSzCnt;
        dLearnRate  = lyrSrc.dLearnRate;
        dRandFstRng = lyrSrc.dRandFstRng;
        dRandSndRng = lyrSrc.dRandSndRng;
    }

    void ValueCopy(const LayerWeight &lyrSrc) {
        ValueAssign(lyrSrc);
        adaDelta      = lyrSrc.adaDelta;
        advNesterov   = lyrSrc.advNesterov;
        vecWeight     = lyrSrc.vecWeight;
        vecWeightTp   = lyrSrc.vecWeightTp;
        vecWeightNv   = lyrSrc.vecWeightNv;
        setWeightGrad = lyrSrc.setWeightGrad;
    }

    void ValueMove(LayerWeight &&lyrSrc) {
        ValueAssign(lyrSrc);
        adaDelta      = std::move(lyrSrc.adaDelta);
        advNesterov   = std::move(lyrSrc.advNesterov);
        vecWeight     = std::move(lyrSrc.vecWeight);
        vecWeightTp   = std::move(lyrSrc.vecWeightTp);
        vecWeightNv   = std::move(lyrSrc.vecWeightNv);
        setWeightGrad = std::move(lyrSrc.setWeightGrad);
    }

    LayerWeight(long double dLearnRate = .0, long double dRandFstRng = -1, long double dRandSndRng = 1) :
        dLearnRate(dLearnRate),
        dRandFstRng(dRandFstRng),
        dRandSndRng(dRandSndRng) {}
    LayerWeight(const LayerWeight &lyrSrc) { ValueCopy(lyrSrc); }
    LayerWeight(LayerWeight &&lyrSrc) { ValueMove(std::move(lyrSrc)); }

    // call this function after weight initializing
    void Shape(uint64_t iBatSz, bool bWeightTp = false) {
        setWeightGrad.init(iBatSz, false);
        if (dLearnRate) {
            vecWeightNv = advNesterov.weight(vecWeight);
            if (bWeightTp) vecWeightTp = vecWeightNv.transpose;
        } else if (bWeightTp) vecWeightTp = vecWeight.transpose;
    }

    void Update(bool bWeightTp = false) {
        auto vecGrad = matrix::vect_sum(setWeightGrad).elem_wise_opt(setWeightGrad.length, MATRIX_ELEM_DIV);
        if (dLearnRate) {
            vecWeight  -= advNesterov.momentum(vecGrad, dLearnRate);
            vecWeightNv = advNesterov.weight(vecWeight);
            if (bWeightTp) vecWeightTp = vecWeightNv.transpose;
        } else {
            vecWeight -= adaDelta.delta(vecGrad);
            if (bWeightTp) vecWeightTp = vecWeight.transpose;
        }
    }

    LayerWeight &operator=(const LayerWeight &lyrSrc) {
        ValueCopy(lyrSrc);
        return *this;
    }
    LayerWeight &operator=(LayerWeight &&lyrSrc) {
        ValueMove(std::move(lyrSrc));
        return *this;
    }

    virtual ~LayerWeight() {
        iBatSzCnt   = 0;
        dLearnRate  = .0;
        dRandFstRng = .0;
        dRandSndRng = .0;
        adaDelta.reset();
        advNesterov.reset();
        vecWeight.reset();
        vecWeightTp.reset();
        vecWeightNv.reset();
        setWeightGrad.reset();
    }
};

matrix_declare struct LayerBias final : LayerWeight<matrix_elem_t> {
    LayerBias(long double dLearnRate = .0, long double dRandFstRng = -1, long double dRandSndRng = 1) : Layer(NEUNET_LAYER_BIAS), LayerWeight<matrix_elem_t>(dLearnRate, dRandFstRng, dRandSndRng) {}

    void Shape(uint64_t iInLnCnt, uint64_t iInColCnt, uint64_t iChannCnt, uint64_t iBatSz) {
        this->vecWeight = neunet_vect(iInLnCnt * iInColCnt, iChannCnt, true, this->dRandFstRng, this->dRandSndRng);
        LayerWeight<matrix_elem_t>::Shape(iBatSz);
    }

    void ForProp(neunet_vect &vecIn) {
        if (this->dLearnRate) vecIn += this->vecWeightNv;
        else vecIn += this->vecWeight;
    }

    void BackProp(neunet_vect &vecGrad, uint64_t iBatSzIdx) {
        this->setWeightGrad[iBatSzIdx] = vecGrad;
        if (++this->iBatSzCnt == this->setWeightGrad.length) {
            this->iBatSzCnt = 0;
            this->Update();
        }
    }

    void Deduce(neunet_vect &vecIn) { vecIn += this->vecWeight; }

    ~LayerBias() {}
};

matrix_declare struct LayerDerive : virtual Layer {
    net_set<neunet_vect> setIn;

    void ValueCopy(const LayerDerive &lyrSrc) { setIn = lyrSrc.setIn; }

    void ValueMove(LayerDerive &&lyrSrc) { setIn = std::move(lyrSrc.setIn); }

    LayerDerive() {}
    LayerDerive(const LayerDerive &lyrSrc) { ValueCopy(lyrSrc); }
    LayerDerive(LayerDerive &&lyrSrc) { ValueMove(std::move(lyrSrc)); }

    void Shape(uint64_t iBatSz) { setIn.init(iBatSz, false); }

    LayerDerive &operator=(const LayerDerive &lyrSrc) {
        ValueCopy(lyrSrc);
        return *this;
    }
    LayerDerive &operator=(LayerDerive &&lyrSrc) {
        ValueMove(std::move(lyrSrc));
        return *this;
    }

    virtual ~LayerDerive() { setIn.reset(); }
};

matrix_declare struct LayerAct : LayerDerive<matrix_elem_t> {
    uint64_t iActFnType = NULL;

    void ValueAssign(const LayerAct &lyrSrc) { iActFnType = lyrSrc.iActFnType; }

    LayerAct(uint64_t iActFnType = NULL) : Layer(NEUNET_LAYER_ACT),
        iActFnType(iActFnType) {}
    LayerAct(const LayerAct &lyrSrc) : LayerDerive<matrix_elem_t>(lyrSrc) { ValueAssign(lyrSrc); }
    LayerAct(LayerAct &&lyrSrc) : LayerDerive<matrix_elem_t>(std::move(lyrSrc)) { ValueAssign(lyrSrc); }

    void ForProp(neunet_vect &vecIn, uint64_t iBatSzIdx) {
        if (iActFnType) this->setIn[iBatSzIdx] = std::move(vecIn);
        switch(iActFnType) {
        case NEUNET_SIGMOID: vecIn = sigmoid(this->setIn[iBatSzIdx]); break;
        case NEUNET_RELU: vecIn = ReLU(this->setIn[iBatSzIdx]); break;
        case NEUNET_ARELU_LOSS:
        case NEUNET_ARELU: vecIn = AReLU(this->setIn[iBatSzIdx]); break;
        case NEUNET_SOFTMAX: vecIn = softmax(this->setIn[iBatSzIdx]); break;
        default: break;
        }
    }

    void BackProp(neunet_vect &vecGrad, const neunet_vect &vecOrgn, uint64_t iBatSzIdx) const {
        switch (iActFnType) {
        case NEUNET_SIGMOID: vecGrad = sigmoid_dv(this->setIn[iBatSzIdx]).elem_wise_opt(vecGrad, MATRIX_ELEM_MULT); break;
        case NEUNET_RELU: vecGrad = ReLU_dv(this->setIn[iBatSzIdx]).elem_wise_opt(vecGrad, MATRIX_ELEM_MULT); break;
        case NEUNET_ARELU: vecGrad = AReLU_dv(this->setIn[iBatSzIdx]).elem_wise_opt(vecGrad, MATRIX_ELEM_MULT); break;
        case NEUNET_ARELU_LOSS: vecGrad = AReLU_loss_grad(this->setIn[iBatSzIdx], vecGrad, vecOrgn); break;
        case NEUNET_SOFTMAX: vecGrad = softmax_cec_grad(vecGrad, vecOrgn); break;
        default: break;
        }
    }

    void Deduce(neunet_vect &vecIn) const {
        switch(iActFnType) {
        case NEUNET_SIGMOID: vecIn = sigmoid(vecIn); break;
        case NEUNET_RELU: vecIn = ReLU(vecIn); break;
        case NEUNET_ARELU_LOSS:
        case NEUNET_ARELU: vecIn = AReLU(vecIn); break;
        case NEUNET_SOFTMAX: vecIn = softmax(vecIn); break;
        default: break;
        }
    }

    LayerAct &operator=(const LayerAct &lyrSrc) {
        LayerDerive<matrix_elem_t>::operator=(lyrSrc);
        ValueAssign(lyrSrc);
        return *this;
    }
    LayerAct &operator=(LayerAct &&lyrSrc) {
        LayerDerive<matrix_elem_t>::operator=(std::move(lyrSrc));
        ValueAssign(lyrSrc);
        return *this;
    }

    virtual ~LayerAct() { iActFnType = NULL; }
};

struct LayerDim : virtual Layer {
    uint64_t iOutLnCnt = 0;

    void ValueAssign(const LayerDim &lyrSrc) { iOutLnCnt = lyrSrc.iOutLnCnt; }

    LayerDim(uint64_t iOutLnCnt = 0) :
        iOutLnCnt(iOutLnCnt) {}
    LayerDim(const LayerDim &lyrSrc) { ValueAssign(lyrSrc); }

    LayerDim &operator=(const LayerDim &lyrSrc) {
        ValueAssign(lyrSrc);
        return *this;
    }

    ~LayerDim() { iOutLnCnt = 0; }
};

struct LayerPC final : LayerDim {
    bool     bPadMode   = false;
     
    uint64_t iInLnCnt   = 0,
             iInColCnt  = 0,
             iOutColCnt = 0,
             
             iTop       = 0,
             iRight     = 0,
             iBottom    = 0,
             iLeft      = 0,
             
             iLnDist    = 0,
             iColDist   = 0;
    
    void ValueAssign(const LayerPC &lyrSrc) {
        bPadMode   = lyrSrc.bPadMode;
        
        iInLnCnt   = lyrSrc.iInLnCnt;
        iInColCnt  = lyrSrc.iInColCnt;
        iOutColCnt = lyrSrc.iOutColCnt;
        
        iTop       = lyrSrc.iTop;
        iRight     = lyrSrc.iRight;
        iBottom    = lyrSrc.iBottom;
        iLeft      = lyrSrc.iLeft;
        
        iLnDist    = lyrSrc.iLnDist;
        iColDist   = lyrSrc.iColDist;
    }

    LayerPC(bool bIsPadMode = false, uint64_t iTopCnt = 0, uint64_t iRightCnt = 0, uint64_t iBottomCnt = 0, uint64_t iLeftCnt = 0, uint64_t iLnDistCnt = 0, uint64_t iColDistCnt = 0) : Layer(NEUNET_LAYER_PC),
        bPadMode(bIsPadMode),
        iTop(iTopCnt),
        iRight(iRightCnt),
        iBottom(iBottomCnt),
        iLeft(iLeftCnt),
        iLnDist(iLnDistCnt),
        iColDist(iColDistCnt) {}
    LayerPC(const LayerPC &lyrSrc) : LayerDim(lyrSrc) { ValueAssign(lyrSrc); }
    
    void Shape(uint64_t &iInLnCnt, uint64_t &iInColCnt) {
        this->iInLnCnt  = iInLnCnt;
        this->iInColCnt = iInColCnt;
        if (bPadMode) {
            iOutLnCnt  = matrix::pad_res_dir_cnt(iTop, iBottom, iInLnCnt, iLnDist);
            iOutColCnt = matrix::pad_res_dir_cnt(iLeft, iRight, iInColCnt, iColDist);
        } else {
            iOutLnCnt  = matrix::crop_res_dir_cnt(iTop, iBottom, iInLnCnt, iLnDist);
            iOutColCnt = matrix::crop_res_dir_cnt(iLeft, iRight, iInColCnt, iColDist);
        }
        iInLnCnt  = iOutLnCnt;
        iInColCnt = iOutColCnt;
    }

    callback_matrix void PadCrop(neunet_vect &vecSrc, bool bIsPadMode) {
        if (iTop || iRight || iBottom || iLeft || iLnDist || iColDist) {
            if(bIsPadMode) vecSrc = chann_vec_pad(iOutLnCnt, iOutColCnt, vecSrc, iInLnCnt, iInColCnt, iTop, iRight, iBottom, iLeft, iLnDist, iColDist);
            else vecSrc = chann_vec_crop(iOutLnCnt, iOutColCnt, vecSrc, iInLnCnt, iInColCnt, iTop, iRight, iBottom, iLeft, iLnDist, iColDist);
        }
    }

    callback_matrix void ForProp(neunet_vect &vecIn) { PadCrop(vecIn, bPadMode); }

    callback_matrix void BackProp(neunet_vect &vecGrad) { PadCrop(vecGrad, !bPadMode); }

    callback_matrix void Deduce(neunet_vect &vecIn) { ForProp(vecIn); }

    LayerPC &operator=(const LayerPC &lyrSrc){
        LayerDim::operator=(lyrSrc);
        ValueAssign(lyrSrc);
        return *this;
    }

    virtual ~LayerPC() {
        bPadMode   = false;
        iInLnCnt   = 0;
        iInColCnt  = 0;
        iOutColCnt = 0;
        iTop       = 0;
        iRight     = 0;
        iBottom    = 0;
        iLeft      = 0;
        iLnDist    = 0;
        iColDist   = 0;
    }
};

LAYER_END
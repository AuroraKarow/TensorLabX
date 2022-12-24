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

callback_matrix neunet_vect softmax_cec_grad(const neunet_vect &softmax_output, const neunet_vect &origin) { return softmax_output - origin; }

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
    const uint64_t iLayerType = NEUNET_LAYER_ACT;
    long double    dLearnRate = 0;

    // asynchronous

    std::atomic_uint64_t iLayerBatchSizeIdx = 0;

    virtual void ValueAssign(const Layer &lyrSrc) {
        dLearnRate         = lyrSrc.dLearnRate;
        iLayerBatchSizeIdx = (uint64_t)lyrSrc.iLayerBatchSizeIdx;
    }

    virtual void ValueCopy(const Layer &lyrSrc) { ValueAssign(lyrSrc); }

    virtual void ValueMove(Layer &&lyrSrc) {
        ValueAssign(lyrSrc);
        lyrSrc.Reset(false);
    }

    Layer(uint64_t iInitLayerType = NEUNET_LAYER_ACT, long double dInitLearnRate = 0) :
        iLayerType(iInitLayerType),
        dLearnRate(dInitLearnRate) {}
    Layer(const Layer &lyrSrc) :
        iLayerType(lyrSrc.iLayerType) { ValueCopy(lyrSrc); }
    Layer(Layer &&lyrSrc) :
        iLayerType(lyrSrc.iLayerType) { ValueMove(std::move(lyrSrc)); }

    virtual void Reset(bool bFull = true) { dLearnRate = 0; }

    virtual ~Layer() { Reset(false); }

    virtual Layer &operator=(const Layer &lyrSrc) {
        if (iLayerType == lyrSrc.iLayerType) ValueCopy(lyrSrc);
        return *this;
    }
    virtual Layer &operator=(Layer &&lyrSrc) {
        if (iLayerType == lyrSrc.iLayerType) ValueMove(std::move(lyrSrc));
        return *this;
    }

};

matrix_declare struct LayerAct : Layer {
    uint64_t iActType = NULL;

    net_set<neunet_vect> setInput;

    virtual void ValueAssign(const LayerAct &lyrSrc) { iActType = lyrSrc.iActType; }

    virtual void ValueCopy(const LayerAct &lyrSrc) {
        ValueAssign(lyrSrc);
        setInput = lyrSrc.setInput;
    }

    virtual void ValueMove(LayerAct &&lyrSrc) {
        ValueAssign(lyrSrc);
        setInput = std::move(lyrSrc.setInput);
        lyrSrc.Reset(false);
    }

    LayerAct(uint64_t iCurrActType = NULL) : Layer(),
        iActType(iCurrActType) {}
    LayerAct(const LayerAct &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerAct(LayerAct &&lyrSrc) : Layer(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void RunInit(uint64_t iTrainBatchSize) { setInput.init(iTrainBatchSize); }

    bool ForwProp(neunet_vect &vecInput, uint64_t iIdx) {
        if (iIdx >= setInput.length) return false;
        setInput[iIdx] = std::move(vecInput);
        switch(iActType) {
        case NEUNET_SIGMOID: vecInput = sigmoid(setInput[iIdx]); break;
        case NEUNET_RELU: vecInput = ReLU(setInput[iIdx]); break;
        case NEUNET_ARELU_LOSS:
        case NEUNET_ARELU: vecInput = AReLU(setInput[iIdx]); break;
        case NEUNET_SOFTMAX: vecInput = softmax(setInput[iIdx]); break;
        default: vecInput = std::move(setInput[iIdx]); break;
        }
        return vecInput.verify;
    }

    bool BackProp(neunet_vect &vecGrad, const neunet_vect &vecOrgn, uint64_t iIdx) const {
        if (iIdx >= setInput.length) return false;
        switch (iActType) {
        case NEUNET_SIGMOID: vecGrad = sigmoid_dv(setInput[iIdx]).elem_wise_opt(vecGrad, MATRIX_ELEM_MULT); break;
        case NEUNET_RELU: vecGrad = ReLU_dv(setInput[iIdx]).elem_wise_opt(vecGrad, MATRIX_ELEM_MULT); break;
        case NEUNET_ARELU: vecGrad = AReLU_dv(setInput[iIdx]).elem_wise_opt(vecGrad, MATRIX_ELEM_MULT); break;
        case NEUNET_ARELU_LOSS: AReLU_loss_grad(setInput[iIdx], vecGrad, vecOrgn); break;
        case NEUNET_SOFTMAX: vecGrad = softmax_cec_grad(vecGrad, vecOrgn); break;
        default: return true;
        }
        return vecGrad.verify;
    }

    bool Deduce(neunet_vect &vecInput) const {
        switch(iActType) {
        case NEUNET_SIGMOID: vecInput = sigmoid(vecInput); break;
        case NEUNET_RELU: vecInput = ReLU(vecInput); break;
        case NEUNET_ARELU_LOSS:
        case NEUNET_ARELU: vecInput = AReLU(vecInput); break;
        case NEUNET_SOFTMAX: vecInput = softmax(vecInput); break;
        default: return true;
        }
        return vecInput.verify;
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);
        iActType = NULL;
        setInput.reset();
    }

    virtual ~LayerAct() { Reset(false); }

    virtual LayerAct &operator=(const LayerAct &lyrSrc) {
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(lyrSrc);
            ValueCopy(lyrSrc);
        }
        return *this;
    }
    virtual LayerAct &operator=(LayerAct &&lyrSrc) {
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(std::move(lyrSrc));
            ValueMove(std::move(lyrSrc));
        }
        return *this;
    }

};

struct LayerPC : Layer {
    bool     bPadMode      = true;
    
    uint64_t iInputLnCnt   = 0,
             iInputColCnt  = 0,

             iOutputLnCnt  = 0,
             iOutputColCnt = 0,

             iTop          = 0,
             iRight        = 0,
             iBottom       = 0,
             iLeft         = 0,

             iLnDist       = 0,
             iColDist      = 0;
    
    virtual void ValueAssign(const LayerPC &lyrSrc) {
        bPadMode      = lyrSrc.bPadMode;

        iInputLnCnt   = lyrSrc.iInputLnCnt;
        iInputColCnt  = lyrSrc.iInputColCnt;

        iOutputLnCnt  = lyrSrc.iOutputLnCnt;
        iOutputColCnt = lyrSrc.iOutputColCnt;

        iTop          = lyrSrc.iTop;
        iRight        = lyrSrc.iRight;
        iBottom       = lyrSrc.iBottom;
        iLeft         = lyrSrc.iLeft;

        iLnDist       = lyrSrc.iLnDist;
        iColDist      = lyrSrc.iColDist;
    }

    virtual void ValueCopy(const LayerPC &lyrSrc) { ValueAssign(lyrSrc); }

    virtual void ValueMove(LayerPC &&lyrSrc) {
        ValueAssign(lyrSrc);
        lyrSrc.Reset(false);
    }

    LayerPC(bool bIsPadMode = true, uint64_t iTopCnt = 0, uint64_t iRightCnt = 0, uint64_t iBottomCnt = 0, uint64_t iLeftCnt = 0, uint64_t iLnDistCnt = 0, uint64_t iColDistCnt = 0) : Layer(NEUNET_LAYER_PC),
        bPadMode(bIsPadMode),
        iTop(iTopCnt),
        iRight(iRightCnt),
        iBottom(iBottomCnt),
        iLeft(iLeftCnt),
        iLnDist(iLnDistCnt),
        iColDist(iColDistCnt) {}
    LayerPC(const LayerPC &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerPC(LayerPC &&lyrSrc) : Layer(lyrSrc) { ValueMove(std::move(lyrSrc)); }
    
    void RunInit(uint64_t &iCurrInputLnCnt, uint64_t &iCurrInputColCnt) {
        iInputLnCnt  = iCurrInputLnCnt;
        iInputColCnt = iCurrInputColCnt;
        if (bPadMode) {
            iOutputLnCnt  = matrix::pad_res_dir_cnt(iTop, iBottom, iInputLnCnt, iLnDist);
            iOutputColCnt = matrix::pad_res_dir_cnt(iLeft, iRight, iInputColCnt, iColDist);
        } else {
            iOutputLnCnt  = matrix::crop_res_dir_cnt(iTop, iBottom, iInputLnCnt, iLnDist);
            iOutputColCnt = matrix::crop_res_dir_cnt(iLeft, iRight, iInputColCnt, iColDist);
        }
        iCurrInputLnCnt  = iOutputLnCnt;
        iCurrInputColCnt = iOutputColCnt;
    }

    callback_matrix bool PadCrop(neunet_vect &vecSrc, bool bIsPadMode) {
        if (!(iTop || iRight || iBottom || iLeft || iLnDist || iColDist)) return true;
        if(bIsPadMode) vecSrc = chann_vec_pad(iOutputLnCnt, iOutputColCnt, vecSrc, iInputLnCnt, iInputColCnt, iTop, iRight, iBottom, iLeft, iLnDist, iColDist);
        else vecSrc = chann_vec_crop(iOutputLnCnt, iOutputColCnt, vecSrc, iInputLnCnt, iInputColCnt, iTop, iRight, iBottom, iLeft, iLnDist, iColDist);
        return vecSrc.verify;
    }

    callback_matrix bool ForwProp(neunet_vect &vecInput) { return PadCrop(vecInput, bPadMode); }

    callback_matrix bool BackProp(neunet_vect &vecGrad) { return PadCrop(vecGrad, !bPadMode); }

    callback_matrix bool Deduce(neunet_vect &vecInput) { return ForwProp(vecInput); }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);

        bPadMode      = true;

        iInputLnCnt   = 0;
        iInputColCnt  = 0;

        iOutputLnCnt  = 0;
        iOutputColCnt = 0;

        iTop          = 0;
        iRight        = 0;
        iBottom       = 0;
        iLeft         = 0;

        iLnDist       = 0;
        iColDist      = 0;
    }

    virtual ~LayerPC() { Reset(false); }

    virtual LayerPC &operator=(const LayerPC &lyrSrc){
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(lyrSrc);
            ValueCopy(lyrSrc);
        }
        return *this;
    }
    virtual LayerPC &operator=(LayerPC &&lyrSrc){
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(std::move(lyrSrc));
            ValueMove(std::move(lyrSrc));
        }
        return *this;
    }
};

matrix_declare struct LayerBias : Layer {
    matrix_elem_t dFstRng = 0,
                  dSndRng = 0;

    uint64_t iAcc       = 0,
             iBatchSize = 0;

    ada_nesterov<matrix_elem_t> advBias;
    ada_delta<matrix_elem_t>    adaBias;

    neunet_vect vecBias,
                vecNesterovBias;

    void ValueAssign(const LayerBias &lyrSrc) {
        dFstRng    = lyrSrc.dFstRng;
        dSndRng    = lyrSrc.dSndRng;
        iAcc       = lyrSrc.iAcc;
        iBatchSize = lyrSrc.iBatchSize;
    }

    void ValueCopy(const LayerBias &lyrSrc) {
        ValueAssign(lyrSrc);
        vecBias         = lyrSrc.vecBias;
        vecNesterovBias = lyrSrc.vecNesterovBias;
        advBias         = lyrSrc.advBias;
        adaBias         = lyrSrc.adaBias;
    }

    void ValueMove(LayerBias &&lyrSrc) {
        ValueAssign(lyrSrc);
        vecBias         = std::move(lyrSrc.vecBias);
        vecNesterovBias = std::move(lyrSrc.vecNesterovBias);
        advBias         = std::move(lyrSrc.advBias);
        adaBias         = std::move(lyrSrc.adaBias);
        Reset(false);
    }

    LayerBias(long double dInitLearnRate = 0, const matrix_elem_t &dRandFstRng = 0, const matrix_elem_t &dRandSndRng = 0, uint64_t iRandAcc = 8) : Layer(NEUNET_LAYER_BIAS, dInitLearnRate),
        dFstRng(dRandFstRng),
        dSndRng(dRandSndRng),
        iAcc(iRandAcc) {}
    LayerBias(const LayerBias &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerBias(LayerBias &&lyrSrc) : Layer(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void RunInit(uint64_t iCurrInputLnCnt, uint64_t iCurrInputColCnt, uint64_t iCurrChannCnt, uint64_t iTrainBatchSize) {
        iBatchSize = iTrainBatchSize;
        vecBias    = neunet_vect(iCurrInputLnCnt * iCurrInputColCnt, iCurrChannCnt, true, dFstRng, dSndRng, iAcc);
        if (dLearnRate) vecNesterovBias = advBias.weight(vecBias);
    }

    bool ForwProp(neunet_vect &vecInput) {
        if (dLearnRate) {
            vecInput += vecNesterovBias;
            return vecInput.verify;
        } else return Deduce(vecInput);
    }

    bool BackProp(neunet_vect &vecGrad) {
        if (++iLayerBatchSizeIdx == iBatchSize) {
            iLayerBatchSizeIdx = 0;
            if (dLearnRate) vecBias -= advBias.momentum(vecGrad, dLearnRate);
            else vecBias -= adaBias.delta(vecGrad);
            return vecBias.verify;
        } else return true;
    }

    callback_matrix bool Deduce(neunet_vect &vecInput) {
        vecInput += vecBias;
        return vecInput.verify;
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);
        iAcc       = 0;
        dFstRng    = 0;
        dSndRng    = 0;
        iBatchSize = 0;
        vecBias.reset();
        advBias.reset();
        adaBias.reset();
        vecNesterovBias.reset();
    }

    virtual ~LayerBias() { Reset(false); }

    virtual LayerBias &operator=(const LayerBias &lyrSrc){
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(lyrSrc);
            ValueCopy(lyrSrc);
        }
        return *this;
    }
    virtual LayerBias &operator=(LayerBias &&lyrSrc){
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(std::move(lyrSrc));
            ValueMove(std::move(lyrSrc));
        }
        return *this;
    }
};

LAYER_END
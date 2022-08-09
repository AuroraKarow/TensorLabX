CONV_BEGIN

callback_matrix neunet_vect CaffeTransform(const neunet_vect &vecChann, uint64_t iChannLnCnt, uint64_t iChannColCnt, uint64_t &iSampChannLnCnt, uint64_t &iSampChannColCnt, uint64_t iFilterLnCnt, uint64_t iFilterColCnt, uint64_t iLnStride, uint64_t iColStride, uint64_t iLnDilate, uint64_t iColDilate) {
    if (!iSampChannLnCnt) iSampChannLnCnt = samp_output_dir_cnt(iChannLnCnt, iFilterLnCnt, iLnStride, iLnDilate);
    if (!iSampChannColCnt) iSampChannColCnt = samp_output_dir_cnt(iChannColCnt, iFilterColCnt, iColStride, iColDilate);
    auto iSampChannElemCnt = iSampChannLnCnt * iSampChannColCnt,
         iFilterElemCnt    = iFilterLnCnt * iFilterColCnt;
    neunet_vect vecAns(iSampChannElemCnt, vecChann.column_count*iFilterElemCnt);
    auto iTensorSize = iSampChannElemCnt * iFilterElemCnt,
         iHyperSize  = vecChann.column_count * iTensorSize;
    for (auto i = 0ull; i < iHyperSize; ++i) {
        auto posHyper  = matrix::elem_pos(i, iTensorSize),
             posTensor = matrix::elem_pos(posHyper.col, iFilterElemCnt),
             posSamp   = matrix::elem_pos(posTensor.ln, iSampChannColCnt),
             posFilter = matrix::elem_pos(posTensor.col, iFilterColCnt);
        vecAns[posTensor.ln][posHyper.ln * iFilterElemCnt + posTensor.col] = vecChann[matrix::elem_pos(samp_trace_pos(posSamp.ln, posFilter.ln, iLnStride, iLnDilate), samp_trace_pos(posSamp.col, posFilter.col, iColStride, iColDilate), iChannColCnt)][posHyper.ln];
    }
    return vecAns;
}
callback_matrix neunet_vect CaffeTransform(const neunet_vect &vecCaffe, uint64_t &iChannLnCnt, uint64_t &iChannColCnt, uint64_t iSampChannLnCnt, uint64_t iSampChannColCnt, uint64_t iFilterLnCnt, uint64_t iFilterColCnt, uint64_t iLnStride, uint64_t iColStride, uint64_t iLnDilate, uint64_t iColDilate, bool bIsGrad) {
    if (!iChannLnCnt) iChannLnCnt = samp_input_dir_cnt(iSampChannLnCnt, iFilterLnCnt, iLnStride, iLnDilate);
    if (!iChannColCnt) iChannColCnt = samp_input_dir_cnt(iSampChannColCnt, iFilterColCnt, iColStride, iColDilate);
    auto iFilterElemCnt = iFilterLnCnt * iFilterColCnt;
    neunet_vect vecAns(iChannLnCnt * iChannColCnt, vecCaffe.column_count / iFilterElemCnt);
    for (auto i = 0ull; i < vecAns.column_count; ++i) for (auto j = 0ull; j < vecCaffe.line_count; ++j) for (auto k = 0ull; k < iFilterElemCnt; ++k) {
        auto posSamp   = matrix::elem_pos(j, iSampChannColCnt),
             posFilter = matrix::elem_pos(k, iFilterColCnt);
        auto iTraceIdx = matrix::elem_pos(samp_trace_pos(posSamp.ln, posFilter.ln, iLnStride, iLnDilate), samp_trace_pos(posSamp.col, posFilter.col, iColStride, iColDilate), iChannColCnt);
        if (bIsGrad) vecAns[iTraceIdx][i] += vecCaffe[j][i * iFilterElemCnt + k];
        else vecAns[iTraceIdx][i] = vecCaffe[j][i * iFilterElemCnt + k];
    }
    return vecAns;
}

callback_matrix neunet_vect InitKernel(uint64_t iAmt, uint64_t iChannCnt, uint64_t iLnCnt, uint64_t iColCnt, const matrix_elem_t &dFstRng = 0, const matrix_elem_t &dSndRng = 0, uint64_t iAcc = 8) { return neunet_vect(iLnCnt * iColCnt * iChannCnt, iAmt, true, dFstRng, dSndRng, iAcc); }

callback_matrix neunet_vect Conv(const neunet_vect &vecCaffeInput, const neunet_vect &vecKernelChann) { return fc::Output(vecKernelChann, vecCaffeInput); }

callback_matrix neunet_vect GradLossToConvCaffeInput(const neunet_vect &vecGradLossToOutput, const neunet_vect &vecKernelChann) { return fc::GradLossToWeight(vecGradLossToOutput, vecKernelChann); }

callback_matrix neunet_vect GradLossToConvKernal(const neunet_vect &vecGradLossToOutput, const neunet_vect &vecCaffeInput) { return fc::GradLossToInput(vecGradLossToOutput, vecCaffeInput); }

callback_matrix neunet_vect PoolGlbAvg(const neunet_vect &vecChann) {
    neunet_vect vecAns(1, vecChann.column_count);
    for (auto i = 0ull; i < vecChann.column_count; ++i) {
        for (auto j = 0ull; j < vecChann.line_count; ++j) vecAns.index(i) += vecChann[j][i];
        vecAns.index(i) /= vecChann.line_count;
    }
    return vecAns;
}

callback_matrix neunet_vect PoolMaxAvg(uint64_t iPoolType, const neunet_vect &vecCaffeInput, uint64_t iFilterLnCnt, uint64_t iFilterColCnt, net_set<net_set<matrix::pos>> &setCaffeMaxPos) {
    auto iFilterElemCnt = iFilterLnCnt * iFilterColCnt;
    neunet_vect vecAns(vecCaffeInput.line_count, vecCaffeInput.column_count / iFilterElemCnt);
    for (auto i = 0ull; i < vecAns.element_count; ++i) {
        auto posDim             = matrix::elem_pos(i, vecAns.column_count);
        auto iCurrChannBeginCol = posDim.col * iFilterElemCnt;
        if (iPoolType == NEUNET_POOL_AVG) vecAns.index(i) = vecCaffeInput.elem_sum(posDim.ln, posDim.ln, iCurrChannBeginCol, iCurrChannBeginCol + iFilterElemCnt - 1, 0, 0) / iFilterElemCnt;
        else if (iPoolType == NEUNET_POOL_MAX) {
            auto setExtmPos = vecCaffeInput.extremum_position(true, posDim.ln, posDim.ln, iCurrChannBeginCol, iCurrChannBeginCol + iFilterElemCnt - 1);
            vecAns.index(i) = vecCaffeInput[setExtmPos[0].ln][setExtmPos[0].col];
            if(!setCaffeMaxPos.length) setCaffeMaxPos.init(vecAns.element_count, false);
            setCaffeMaxPos[i] = std::move(setExtmPos);
        } else {
            vecAns.reset();
            setCaffeMaxPos.reset();
            break;
        }
    }
    return vecAns;
}

callback_matrix neunet_vect GradLossToPoolGlbAvgChann(const neunet_vect &vecGradLossToOutput, uint64_t iChannLnCnt, uint64_t iChannColCnt) {
    neunet_vect vecAns(iChannLnCnt * iChannColCnt, vecGradLossToOutput.column_count);
    for (auto i = 0ull; i < vecAns.column_count; ++i) {
        auto dGradChann = vecGradLossToOutput.index(i) / vecAns.line_count;
        for(auto j = 0ull; j < vecAns.line_count; ++j) vecAns[j][i] = dGradChann;
    }
    return vecAns;
}

callback_matrix neunet_vect GradLossToPoolMaxAvgCaffeInput(uint64_t iPoolType, const neunet_vect &vecGradLossToOutput, uint64_t iFilterLnCnt, uint64_t iFilterColCnt, const net_set<net_set<matrix::pos>> &setCaffeMaxPos) {
    auto iFilterElemCnt = iFilterLnCnt * iFilterColCnt;
    neunet_vect vecAns(vecGradLossToOutput.line_count, iFilterElemCnt * vecGradLossToOutput.column_count);
    for (auto i = 0ull; i < vecGradLossToOutput.element_count; ++i) {
        auto posDim             = matrix::elem_pos(i, vecGradLossToOutput.column_count);
        auto iCurrChannBeginCol = posDim.col * iFilterElemCnt;
        if (iPoolType == NEUNET_POOL_AVG) {
            auto iAvgVal = vecGradLossToOutput.index(i) / iFilterElemCnt;
            for (auto j = 0ull; j < iFilterElemCnt; ++j) vecAns[posDim.ln][j + iCurrChannBeginCol] = iAvgVal;
        } else if (iPoolType == NEUNET_POOL_MAX && setCaffeMaxPos.length) for (auto j = 0ull; j < setCaffeMaxPos[i].length; ++j) vecAns[setCaffeMaxPos[i][j].ln][setCaffeMaxPos[i][j].col] += vecGradLossToOutput.index(i);
        else {
            vecAns.reset();
            break;
        }
    }
    return vecAns;
}

CONV_END

LAYER_BEGIN

matrix_declare struct LayerConv : Layer {
    uint64_t iInputLnCnt     = 0,
             iInputColCnt    = 0,
             iOutputLnCnt    = 0,
             iOutputColCnt   = 0,

             iLnStride       = 0,
             iColStride      = 0,
             iLnDilate       = 0,
             iColDilate      = 0,

             iKernelAmt      = 0,
             iKernelChannCnt = 0,
             iKernelLnCnt    = 0,
             iKernelColCnt   = 0,

             iAcc            = 8;

    matrix_elem_t dFstRng = 0,
                  dSndRng = 0;

    neunet_vect vecKernel,
                vecNesterovKernel;

    ada_nesterov<matrix_elem_t> advKernel;
    ada_delta<matrix_elem_t>    adaKernel;

    net_set<neunet_vect> setCaffeInput,
                         setGradKernel;

    virtual void ValueAssign(const LayerConv &lyrSrc) {
        iInputLnCnt     = lyrSrc.iInputLnCnt;
        iInputColCnt    = lyrSrc.iInputColCnt;
        iOutputLnCnt    = lyrSrc.iOutputLnCnt;
        iOutputColCnt   = lyrSrc.iOutputColCnt;
        iLnStride       = lyrSrc.iLnStride;
        iColStride      = lyrSrc.iColStride;
        iLnDilate       = lyrSrc.iLnDilate;
        iColDilate      = lyrSrc.iColDilate;
        iKernelAmt      = lyrSrc.iKernelAmt;
        iKernelChannCnt = lyrSrc.iKernelChannCnt;
        iKernelLnCnt    = lyrSrc.iKernelLnCnt;
        iKernelColCnt   = lyrSrc.iKernelColCnt;
        iAcc            = lyrSrc.iAcc;
    }

    virtual void ValueCopy(const LayerConv &lyrSrc) {
        ValueAssign(lyrSrc);
        dFstRng           = lyrSrc.dFstRng;
        dSndRng           = lyrSrc.dSndRng;
        vecKernel         = lyrSrc.vecKernel;
        vecNesterovKernel = lyrSrc.vecNesterovKernel;
        advKernel         = lyrSrc.advKernel;
        adaKernel         = lyrSrc.adaKernel;
        setCaffeInput     = lyrSrc.setCaffeInput;
        setGradKernel     = lyrSrc.setGradKernel;
    }

    virtual void ValueMove(LayerConv &&lyrSrc) {
        ValueAssign(lyrSrc);
        dFstRng           = std::move(lyrSrc.dFstRng);
        dSndRng           = std::move(lyrSrc.dSndRng);
        vecKernel         = std::move(lyrSrc.vecKernel);
        vecNesterovKernel = std::move(lyrSrc.vecNesterovKernel);
        advKernel         = std::move(lyrSrc.advKernel);
        adaKernel         = std::move(lyrSrc.adaKernel);
        setCaffeInput     = std::move(lyrSrc.setCaffeInput);
        setGradKernel     = std::move(lyrSrc.setGradKernel);
        lyrSrc.Reset(false);
    }

    LayerConv(uint64_t iCurrKernelAmt = 0, uint64_t iCurrKernelLnCnt = 0, uint64_t iCurrKernelColCnt = 0, uint64_t iCurrLnStride = 0, uint64_t iCurrColStride = 0, uint64_t iCurrLnDilate = 0, uint64_t iCurrColDilate = 0, long double dInitLearnRate = 0, const matrix_elem_t &dRandFstRng = 0, const matrix_elem_t &dRandSndRng = 0, uint64_t iRandAcc = 8) : Layer(NEUNET_LAYER_CONV, dInitLearnRate),
        iKernelAmt(iCurrKernelAmt),
        iKernelLnCnt(iCurrKernelLnCnt),
        iKernelColCnt(iCurrKernelColCnt),
        iLnStride(iCurrLnStride),
        iColStride(iCurrColStride),
        iLnDilate(iCurrLnDilate),
        iColDilate(iCurrColDilate),
        dFstRng(dRandFstRng),
        dSndRng(dRandSndRng),
        iAcc(iRandAcc) {}
    LayerConv(const LayerConv &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerConv(LayerConv &&lyrSrc) : Layer(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void RunInit(uint64_t iCurrInputLnCnt, uint64_t iCurrInputColCnt, uint64_t iCurrChannCnt, uint64_t iBatchSize) {
        iInputLnCnt     = iCurrInputLnCnt;
        iInputColCnt    = iCurrInputColCnt;
        iKernelChannCnt = iCurrChannCnt;
        iOutputLnCnt    = samp_output_dir_cnt(iInputLnCnt, iKernelLnCnt, iLnStride, iLnDilate);
        iOutputColCnt   = samp_output_dir_cnt(iInputColCnt, iKernelColCnt, iColStride, iColDilate);
        vecKernel       = conv::InitKernel(iKernelAmt, iKernelChannCnt, iKernelLnCnt, iKernelColCnt, dFstRng, dSndRng, iAcc);
        if (this->dLearnRate) vecNesterovKernel = advKernel.weight(vecKernel);
        setCaffeInput.init(iBatchSize, false);
        setGradKernel.init(iBatchSize, false);
    }

    bool ForwProp(neunet_vect &vecInput, uint64_t iIdx) {
        if (iIdx >= setCaffeInput.length) return false;
        setCaffeInput[iIdx] = conv::CaffeTransform(vecInput, iInputLnCnt, iInputColCnt, iOutputLnCnt, iOutputColCnt, iKernelLnCnt, iKernelColCnt, iLnStride, iColStride, iLnDilate, iColDilate);
        if (this->dLearnRate) vecInput = conv::Conv(setCaffeInput[iIdx], vecNesterovKernel);
        else vecInput = conv::Conv(setCaffeInput[iIdx], vecKernel);
        return vecInput.verify;
    }

    bool BackProp(neunet_vect &vecGrad, uint64_t iIdx) {
        if (iIdx >= setCaffeInput.length) return false;
        setGradKernel[iIdx] = conv::GradLossToConvKernal(vecGrad, setCaffeInput[iIdx]);
        if (this->dLearnRate) vecGrad = conv::GradLossToConvCaffeInput(vecGrad, vecNesterovKernel);
        else vecGrad = conv::GradLossToConvCaffeInput(vecGrad, vecKernel);
        vecGrad = conv::CaffeTransform(vecGrad, iInputLnCnt, iInputColCnt, iOutputLnCnt, iOutputColCnt, iKernelLnCnt, iKernelColCnt, iLnStride, iColStride, iLnDilate, iColDilate, true);
        return vecGrad.verify && setGradKernel[iIdx].verify;
    }

    bool Deduce(neunet_vect &vecInput) {
        vecInput = conv::Conv(conv::CaffeTransform(vecInput, iInputLnCnt, iInputColCnt, iOutputLnCnt, iOutputColCnt, iKernelLnCnt, iKernelColCnt, iLnStride, iColStride, iLnDilate, iColDilate), vecKernel);
        return vecInput.verify;
    }

    void Update() {
        auto vecGrad = setGradKernel.sum.elem_wise_opt(setGradKernel.length, MATRIX_ELEM_DIV);
        if (this->dLearnRate) {
            vecKernel        -= advKernel.momentum(vecGrad, this->dLearnRate);
            vecNesterovKernel = advKernel.weight(vecKernel);
        }
        else vecKernel -= adaKernel.delta(vecGrad);
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(bFull);
        iInputLnCnt     = 0;
        iInputColCnt    = 0;
        iOutputLnCnt    = 0;
        iOutputColCnt   = 0;
        iLnStride       = 0;
        iColStride      = 0;
        iLnDilate       = 0;
        iColDilate      = 0;
        iKernelAmt      = 0;
        iKernelChannCnt = 0;
        iKernelLnCnt    = 0;
        iKernelColCnt   = 0;
        iAcc            = 8;
        dFstRng         = 0;
        dSndRng         = 0;
        vecKernel.reset();        
        vecNesterovKernel.reset();
        advKernel.reset();        
        adaKernel.reset();        
        setCaffeInput.reset();    
        setGradKernel.reset();            
    }

    virtual ~LayerConv() { Reset(false); }

    virtual LayerConv &operator=(const LayerConv &lyrSrc) {
        if (this->iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(lyrSrc);
            ValueCopy(lyrSrc);
        }
        return *this;
    }
    virtual LayerConv &operator=(LayerConv &&lyrSrc) {
        if (this->iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(std::move(lyrSrc));
            ValueMove(std::move(lyrSrc));
        }
        return *this;
    }
};

struct LayerPool : Layer {
    uint64_t iPoolType     = NEUNET_POOL_MAX,

             iInputLnCnt   = 0,
             iInputColCnt  = 0,
             iOutputLnCnt  = 0,
             iOutputColCnt = 0,

             iLnStride     = 0,
             iColStride    = 0,
             iLnDilate     = 0,
             iColDilate    = 0,

             iFilterLnCnt  = 0,
             iFilterColCnt = 0;

    net_set<net_set<net_set<matrix::pos>>> setCaffeMaxPos;

    virtual void ValueAssign(const LayerPool &lyrSrc) {
        iPoolType     = lyrSrc.iPoolType;
        iInputLnCnt   = lyrSrc.iInputLnCnt;
        iInputColCnt  = lyrSrc.iInputColCnt;
        iOutputLnCnt  = lyrSrc.iOutputLnCnt;
        iOutputColCnt = lyrSrc.iOutputColCnt;
        iLnStride     = lyrSrc.iLnStride;
        iColStride    = lyrSrc.iColStride;
        iLnDilate     = lyrSrc.iLnDilate;
        iColDilate    = lyrSrc.iColDilate;
        iFilterLnCnt  = lyrSrc.iFilterLnCnt;
        iFilterColCnt = lyrSrc.iFilterColCnt;
    }

    virtual void ValueCopy(const LayerPool &lyrSrc) {
        ValueAssign(lyrSrc);
        setCaffeMaxPos = lyrSrc.setCaffeMaxPos;
    }

    virtual void ValueMove(LayerPool &&lyrSrc) {
        ValueAssign(lyrSrc);
        setCaffeMaxPos = std::move(lyrSrc.setCaffeMaxPos);
        lyrSrc.Reset(false);
    }

    LayerPool(uint64_t iCurrPoolType = NEUNET_POOL_MAX, uint64_t iCurrFilterLnCnt = 0, uint64_t iCurrFilterColCnt = 0, uint64_t iCurrLnStride = 0, uint64_t iCurrColStride = 0, uint64_t iCurrLnDilate = 0, uint64_t iCurrColDilate = 0) : Layer(NEUNET_LAYER_POOL),
        iPoolType(iCurrPoolType),
        iFilterLnCnt(iCurrFilterLnCnt),
        iFilterColCnt(iCurrFilterColCnt),
        iLnStride(iCurrLnStride),
        iColStride(iCurrColStride),
        iLnDilate(iCurrLnDilate),
        iColDilate(iCurrColDilate) {}
    LayerPool(const LayerPool &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerPool(LayerPool &&lyrSrc) : Layer(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void RunInit(uint64_t iCurrInputLnCnt, uint64_t iCurrInputColCnt, uint64_t iBatchSize) {
        iInputLnCnt  = iCurrInputLnCnt;
        iInputColCnt = iCurrInputColCnt;
        if (iPoolType == NEUNET_POOL_GAG) {
            iOutputColCnt = 1;
            iOutputLnCnt  = 1;
        } else {
            iOutputColCnt = samp_output_dir_cnt(iInputLnCnt, iFilterLnCnt, iLnStride, iLnDilate);
            iOutputLnCnt  = samp_output_dir_cnt(iInputColCnt, iFilterColCnt, iColStride, iColDilate);
            setCaffeMaxPos.init(iBatchSize, false);
        }
    }

    callback_matrix bool ForwProp(neunet_vect &vecInput, uint64_t iIdx) {
        if (iPoolType == NEUNET_POOL_GAG) vecInput = conv::PoolGlbAvg(vecInput);
        else vecInput = conv::PoolMaxAvg(iPoolType, conv::CaffeTransform(vecInput, iInputLnCnt, iInputColCnt, iOutputLnCnt, iOutputColCnt, iFilterLnCnt, iFilterColCnt, iLnStride, iColStride, iLnDilate, iColDilate), iFilterLnCnt, iFilterColCnt, setCaffeMaxPos[iIdx]);
        return vecInput.verify;
    }

    callback_matrix bool BackProp(neunet_vect &vecGrad, uint64_t iIdx) {
        if (iPoolType == NEUNET_POOL_GAG) vecGrad = conv::GradLossToPoolGlbAvgChann(vecGrad, iInputLnCnt, iInputColCnt);
        else vecGrad = conv::CaffeTransform(conv::GradLossToPoolMaxAvgCaffeInput(iPoolType, vecGrad, iFilterLnCnt, iFilterColCnt, setCaffeMaxPos[iIdx]), iInputLnCnt, iInputColCnt, iOutputLnCnt, iOutputColCnt, iFilterLnCnt, iFilterColCnt, iLnStride, iColStride, iLnDilate, iColDilate, true);
        return vecGrad.verify;
    }

    callback_matrix bool Deduce(neunet_vect &vecInput) {
        if (iPoolType == NEUNET_POOL_GAG) return ForwProp(vecInput, 0);
        net_set<net_set<matrix::pos>> setTemp;
        vecInput = conv::PoolMaxAvg(iPoolType, conv::CaffeTransform(vecInput, iInputLnCnt, iInputColCnt, iOutputLnCnt, iOutputColCnt, iFilterLnCnt, iFilterColCnt, iLnStride, iColStride, iLnDilate, iColDilate), iFilterLnCnt, iFilterColCnt, setTemp);
        return vecInput.verify;
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);
        iPoolType     = NEUNET_POOL_MAX;
        iInputLnCnt   = 0;
        iInputColCnt  = 0;
        iOutputLnCnt  = 0;
        iOutputColCnt = 0;
        iLnStride     = 0;
        iColStride    = 0;
        iLnDilate     = 0;
        iColDilate    = 0;
        iFilterLnCnt  = 0;
        iFilterColCnt = 0;
    }

    virtual ~LayerPool() { Reset(false); }

    virtual LayerPool &operator=(const LayerPool &lyrSrc) {
        if (this->iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(lyrSrc);
            ValueCopy(lyrSrc);
        }
        return *this;
    }
    virtual LayerPool &operator=(LayerPool &&lyrSrc) {
        if (this->iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(std::move(lyrSrc));
            ValueMove(std::move(lyrSrc));
        }
        return *this;
    }

};

LAYER_END
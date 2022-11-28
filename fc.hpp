FC_BEGIN

callback_matrix neunet_vect InitWeight(uint64_t iInputLnCnt, uint64_t iOutputLnCnt, const matrix_elem_t &dFstRng = 0, const matrix_elem_t &dSndRng = 0, uint64_t iAcc = 8) { return neunet_vect(iOutputLnCnt, iInputLnCnt, true, dFstRng, dSndRng, iAcc); }

callback_matrix neunet_vect Output(const neunet_vect &vecInput, const neunet_vect &vecWeight) { return vecWeight * vecInput; }

callback_matrix neunet_vect GradLossToInput(const neunet_vect &vecGradLossToOutput, const neunet_vect &vecWeightTranspose) { return vecWeightTranspose * vecGradLossToOutput; }

/** [FCN BP gradient]
 * @brief Get gradient from loss to weight vector
 * @param vecGradLossToOutput Gradient from loss to FC output vector.
 * @param vecInputTranspose   Transposition of FC input vector
 * @warning Call this function at last.
 */
callback_matrix neunet_vect GradLossToWeight(const neunet_vect &vecGradLossToOutput, const neunet_vect &vecInputTranspose) { return vecGradLossToOutput * vecInputTranspose; }

FC_END

LAYER_BEGIN

struct LayerTrans : Layer {
    uint64_t iLnCnt    = 0,
             iColCnt   = 0,
             iChannCnt = 0;
    
    virtual void ValueAssign(const LayerTrans &lyrSrc) {
        iLnCnt    = iLnCnt;
        iColCnt   = iColCnt;
        iChannCnt = lyrSrc.iChannCnt;
    }

    virtual void ValueCopy(const LayerTrans &lyrSrc) { ValueAssign(lyrSrc); }

    virtual void ValueMove(LayerTrans &&lyrSrc) {
        ValueAssign(lyrSrc);
        lyrSrc.Reset(false);
    }

    LayerTrans() : Layer(NEUNET_LAYER_TRANS) {}
    LayerTrans(const LayerTrans &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerTrans(LayerTrans &&lyrSrc) : Layer(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void RunInit(uint64_t &iInputLnCnt, uint64_t &iInputColCnt, uint64_t &iCurrChannCnt) {
        iLnCnt        = iInputColCnt;
        iColCnt       = iInputColCnt;
        iChannCnt     = iCurrChannCnt;
        iInputLnCnt   = iInputLnCnt * iInputColCnt * iChannCnt;
        iInputColCnt  = 1;
        iCurrChannCnt = 1;
    }
    
    callback_matrix bool ForwProp(neunet_vect &vecInput) { return Deduce(vecInput); }

    callback_matrix bool BackProp(neunet_vect &vecGrad) {
        vecGrad = vecGrad.reshape(iLnCnt * iColCnt, iChannCnt);
        return vecGrad.verify;
    }

    callback_matrix bool Deduce(neunet_vect &vecInput) {
        vecInput = vecInput.reshape(iLnCnt * iColCnt * iChannCnt, 1);  
        return vecInput.verify;  
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);
        iLnCnt    = 0;
        iColCnt   = 0;
        iChannCnt = 0;
    }

    virtual ~LayerTrans() { Reset(false); }

    LayerTrans &operator=(const LayerTrans &lyrSrc) {
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(lyrSrc);
            ValueCopy(lyrSrc);
        }
        return *this;
    }
    LayerTrans &operator=(LayerTrans &&lyrSrc) {
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(std::move(lyrSrc));
            ValueMove(std::move(lyrSrc));
        }
        return *this;
    }

};

matrix_declare struct LayerFC : Layer {
    uint64_t iOutputLnCnt = 0,
             iAcc         = 8;

    matrix_elem_t dFstRng = 0,
                  dSndRng = 0;
    
    neunet_vect vecWeight,
                vecTpWeight,
                vecNesterovWeight;

    ada_nesterov<matrix_elem_t> advWeight;
    ada_delta<matrix_elem_t>    adaWeight;

    net_set<neunet_vect> setInput,
                         setGradWeight;

    virtual void ValueAssign(const LayerFC &lyrSrc) {
        iOutputLnCnt = lyrSrc.iOutputLnCnt;
        iAcc         = lyrSrc.iAcc;
    }

    virtual void ValueCopy(const LayerFC &lyrSrc) {
        ValueAssign(lyrSrc);
        dFstRng           = lyrSrc.dFstRng;
        dSndRng           = lyrSrc.dSndRng;
        setInput          = lyrSrc.setInput;
        advWeight         = lyrSrc.advWeight;
        adaWeight         = lyrSrc.adaWeight;
        vecWeight         = lyrSrc.vecWeight;
        vecTpWeight       = lyrSrc.vecTpWeight;
        setGradWeight     = lyrSrc.setGradWeight;
        vecNesterovWeight = lyrSrc.vecNesterovWeight;
    }

    virtual void ValueMove(LayerFC &&lyrSrc) {
        ValueAssign(lyrSrc);
        dFstRng           = std::move(lyrSrc.dFstRng);
        dSndRng           = std::move(lyrSrc.dSndRng);
        setInput          = std::move(lyrSrc.setInput);
        advWeight         = std::move(lyrSrc.advWeight);
        adaWeight         = std::move(lyrSrc.adaWeight);
        vecWeight         = std::move(lyrSrc.vecWeight);
        vecTpWeight       = std::move(lyrSrc.vecTpWeight);
        setGradWeight     = std::move(lyrSrc.setGradWeight);
        vecNesterovWeight = std::move(lyrSrc.vecNesterovWeight);
        lyrSrc.Reset(false);
    }

    LayerFC(uint64_t iCurrOutputLnCnt = 1, long double dInitLearnRate = 0, const matrix_elem_t &dRandFstRng = 0, const matrix_elem_t &dRandSndRng = 0, uint64_t iRandAcc = 8) : Layer(NEUNET_LAYER_FC, dInitLearnRate),
        iOutputLnCnt(iCurrOutputLnCnt),
        dFstRng(dRandFstRng),
        dSndRng(dRandSndRng),
        iAcc(iRandAcc) {}
    LayerFC(const LayerFC &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerFC(LayerFC &&lyrSrc) : Layer(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void RunInit(uint64_t &iCurrInputLnCnt, uint64_t iTrainBatchSize) {
        if (!vecWeight.verify) vecWeight = fc::InitWeight(iCurrInputLnCnt, iOutputLnCnt, dFstRng, dSndRng, iAcc);
        if (dLearnRate) {
            vecNesterovWeight = advWeight.weight(vecWeight);
            vecTpWeight       = vecNesterovWeight.transpose;
        } else vecTpWeight = vecWeight.transpose;
        setInput.init(iTrainBatchSize, false);
        setGradWeight.init(iTrainBatchSize, false);
        iCurrInputLnCnt = iOutputLnCnt;
    }

    bool ForwProp(neunet_vect &vecInput, uint64_t iIdx) {
        if (iIdx >= setInput.length) return false;
        setInput[iIdx] = std::move(vecInput);
        if (dLearnRate) vecInput = vecNesterovWeight * setInput[iIdx];
        else vecInput = vecWeight * setInput[iIdx];
        return vecInput.verify;
    }

    bool BackProp(neunet_vect &vecGrad, uint64_t iIdx) {
        if (iIdx >= setInput.length) return false;
        setGradWeight[iIdx] = vecGrad * setInput[iIdx].transpose;
        if (!setGradWeight[iIdx].verify) return false;
        vecGrad = vecTpWeight * vecGrad;
        if (++iLayerBatchSizeIdx == setInput.length) {
            Update();
            iLayerBatchSizeIdx = 0;
        }
        return vecGrad.verify;
    }

    bool Deduce(neunet_vect &vecInput) const {
        vecInput = vecWeight * vecInput;
        return vecInput.verify;
    }

    void Update() {
        auto vecGrad = matrix::vect_sum(setGradWeight).elem_wise_opt(setGradWeight.length, MATRIX_ELEM_DIV);
        if (dLearnRate) {
            vecWeight        -= advWeight.momentum(vecGrad, dLearnRate);
            vecNesterovWeight = advWeight.weight(vecWeight);
            vecTpWeight       = vecNesterovWeight.transpose;
        } else {
            vecWeight  -= adaWeight.delta(vecGrad);
            vecTpWeight = vecWeight.transpose;
        }
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);
        iOutputLnCnt = 0;
        iAcc         = 8;
        dFstRng      = 0;
        dSndRng      = 0;
        setInput.reset();
        vecWeight.reset();
        advWeight.reset();
        advWeight.reset();
        vecTpWeight.reset();
        setGradWeight.reset();
        vecNesterovWeight.reset();
    }

    virtual ~LayerFC() { Reset(false); }

    virtual LayerFC &operator=(const LayerFC &lyrSrc) {
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(lyrSrc);
            ValueCopy(lyrSrc);
        }
        return *this;
    }
    virtual LayerFC &operator=(LayerFC &&lyrSrc) {
        if (iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(std::move(lyrSrc));
            ValueMove(std::move(lyrSrc));
        }
        return *this;
    }

};

LAYER_END
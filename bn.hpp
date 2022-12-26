NEUNET_BEGIN

/* Batch normalization */

// BN parameters
callback_matrix neunet_vect BNInitBetaGamma(uint64_t iChannCnt, const matrix_elem_t &iPlaceholder = 0) {
    neunet_vect vecAns(iChannCnt, 1);
    if (iPlaceholder != 0) vecAns.fill_elem(iPlaceholder);
    return vecAns;
}

matrix_declare struct BNData final {
    neunet_vect vecMuBeta,
                vecSigmaSqr,
                vecSigma,
                vecSigmaDom,
                vecExpMuBeta,
                vecExpSigmaSqr;

    uint64_t iTrainBatchCnt = 0,
             iTrainBatchIdx = 0;

    long double dCoeBatchSize   = 0,
                dCoeDbBatchSize = 0;

    net_set<neunet_vect> setBarX;
    net_set<neunet_vect> setDist;
};

callback_matrix void BNInitBNData(BNData<matrix_elem_t> &BdData, uint64_t iTrainBatchSize, uint64_t iTrainBatchCnt) {
    BdData.dCoeBatchSize   = 1.l / iTrainBatchSize;
    BdData.dCoeDbBatchSize = 2.l / iTrainBatchSize;
    BdData.iTrainBatchCnt  = iTrainBatchCnt;
}

callback_matrix net_set<neunet_vect> BNTrain (BNData<matrix_elem_t> &BdData, const net_set<neunet_vect> &setInput, const neunet_vect &vecBeta, const neunet_vect &vecGamma) {
    // Average, mu
    BdData.vecMuBeta = BdData.dCoeBatchSize * matrix::vect_sum(setInput);
    // Variance, sigma square
    BdData.setDist.init(setInput.length, false);
    BdData.vecSigmaSqr = neunet_vect(BdData.vecMuBeta.line_count, BdData.vecMuBeta.column_count);
    for (auto i = 0ull; i < setInput.length; ++i) {
        BdData.setDist[i]   = setInput[i] - BdData.vecMuBeta;
        BdData.vecSigmaSqr += BdData.setDist[i].elem_wise_opt(BdData.setDist[i], MATRIX_ELEM_MULT);
    }
    BdData.vecSigmaSqr *= BdData.dCoeBatchSize;
    BdData.vecSigma     = BdData.vecSigmaSqr.elem_wise_opt(0.5l, MATRIX_ELEM_POW);
    BdData.vecSigmaDom  = BdData.vecSigma;
    // Bar X, normalize x
    BdData.setBarX.init(setInput.length, false);
    for (auto i = 0ull; i < setInput.length; ++i) BdData.setBarX[i] = BdData.setDist[i].elem_wise_opt(BdData.vecSigmaDom, MATRIX_ELEM_DIV);
    // Y, Output
    auto setY = BdData.setBarX;
    // Y = gamma * bar x + beta
    for (auto i = 0ull; i < setY.length; ++i) for (auto j = 0ull; j < setY[i].line_count; ++j) for (auto k = 0ull; k < setY[i].column_count; ++k) {
        setY[i][j][k] *= vecGamma.index(k);
        setY[i][j][k] += vecBeta.index(k);
    }
    return setY;
}

callback_matrix net_set<neunet_vect> BNGradLossToInputGammaBeta(BNData<matrix_elem_t> &BdData, neunet_vect &vecGradGamma, neunet_vect &vecGradBeta, net_set<neunet_vect> &setGradLossToOutput, const neunet_vect &vecGamma, long double dDecay = .9l) {
    // Gradient gamma & beta
    vecGradGamma = neunet_vect(BdData.vecSigma.column_count, 1);
    vecGradBeta  = vecGradGamma;
    for (auto i = 0ull; i < BdData.setBarX.length; ++i) BdData.setBarX[i] = BdData.setBarX[i].elem_wise_opt(setGradLossToOutput[i], MATRIX_ELEM_MULT);
    auto vecGradGammaTensor = matrix::vect_sum(BdData.setBarX),
         vecGradBetaTensor  = matrix::vect_sum(setGradLossToOutput);
    for (auto i = 0ull; i < vecGradGammaTensor.line_count; ++i) for (auto j = 0ull; j < vecGradBetaTensor.column_count; ++j) {
        vecGradGamma.index(j) += vecGradGammaTensor[i][j];
        vecGradBeta.index(j)  += vecGradBetaTensor[i][j];
    }
    // Gradient bar x
    auto setGradBarX = std::move(setGradLossToOutput);
    for (auto i = 0ull; i < setGradBarX.length; ++i) for (auto j = 0ull; j < setGradBarX[i].line_count; ++j) for (auto k = 0ull; k < setGradBarX[i].column_count; ++k) setGradBarX[i][j][k] *= vecGamma.index(k);
    // Gradient variant
    auto vecSigmaSqOnePtFive = BdData.vecSigmaSqr.elem_wise_opt(BdData.vecSigma, MATRIX_ELEM_MULT);
    neunet_vect vecGradSigmaSqr(BdData.vecSigma.line_count, BdData.vecSigma.column_count);
    for (auto i = 0ull; i < BdData.setBarX.length; ++i) vecGradSigmaSqr += setGradBarX[i].elem_wise_opt(BdData.setDist[i], MATRIX_ELEM_MULT);
    vecGradSigmaSqr  = vecGradSigmaSqr.elem_wise_opt(vecSigmaSqOnePtFive, MATRIX_ELEM_DIV);
    vecGradSigmaSqr *= (-.5l);
    // Gradient expectation
    auto vecGradMuBeta = matrix::vect_sum(setGradBarX).elem_wise_opt(BdData.vecSigmaDom, MATRIX_ELEM_DIV) + BdData.dCoeDbBatchSize * vecGradSigmaSqr.elem_wise_opt(matrix::vect_sum(BdData.setDist), MATRIX_ELEM_MULT);
    vecGradMuBeta *= BdData.dCoeBatchSize;
    // Gradient input
    net_set<neunet_vect> setGradInput(BdData.setBarX.length);
    for (auto i = 0ull; i < setGradInput.length; ++i) {
        setGradInput[i] = setGradBarX[i].elem_wise_opt(BdData.vecSigmaDom, MATRIX_ELEM_DIV) + BdData.dCoeDbBatchSize * vecGradSigmaSqr.elem_wise_opt(BdData.setDist[i], MATRIX_ELEM_MULT) - vecGradMuBeta;
        if (!setGradInput[i].verify) {
            setGradInput.reset();
            break;
        }
    }
    // moving average
    if (BdData.vecExpMuBeta.verify && BdData.vecExpSigmaSqr.verify) {
        BdData.vecExpMuBeta   = dDecay * BdData.vecExpMuBeta + (1 - dDecay) * BdData.vecMuBeta;
        BdData.vecExpSigmaSqr = dDecay * BdData.vecExpSigmaSqr + (1 - dDecay) * BdData.vecSigmaSqr;
    } else {
        BdData.vecExpMuBeta   = std::move(BdData.vecMuBeta);
        BdData.vecExpSigmaSqr = std::move(BdData.vecSigmaSqr);            
    }
    if (++BdData.iTrainBatchIdx == BdData.iTrainBatchCnt) {
        BdData.iTrainBatchIdx = 0;
        BdData.vecExpSigmaSqr = BdData.vecExpSigmaSqr.elem_wise_opt(0.5l, MATRIX_ELEM_POW);
    }
    return setGradInput;
}

callback_matrix neunet_vect BNDeduce(const BNData<matrix_elem_t> &BdData, const neunet_vect &vecInput, const neunet_vect &vecBeta, const neunet_vect &vecGamma) {
    auto vecAns = (vecInput - BdData.vecExpMuBeta).elem_wise_opt(BdData.vecExpSigmaSqr, MATRIX_ELEM_DIV);
    for (auto i = 0ull; i < vecAns.line_count; ++i) for (auto j = 0ull; j < vecAns.column_count; ++j) vecAns[i][j] = vecGamma.index(j) * vecAns[i][j] + vecBeta.index(j);    
    return vecAns;
}

NEUNET_END

LAYER_BEGIN

matrix_declare struct LayerBN : Layer {
    long double dDecay = 0.9l;

    matrix_elem_t dBeta  = 0,
                  dGamma = 1;

    ada_nesterov<matrix_elem_t> advBeta,
                                advGamma;

    ada_delta<matrix_elem_t> adaBeta,
                             adaGamma;

    neunet_vect vecBeta,
                vecGamma,

                vecNesterovBeta,
                vecNesterovGamma,

                vecGradBeta,
                vecGradGamma;

    net_set<neunet_vect> setInput,
                         setGrad;

    BNData<matrix_elem_t> BdData;

    async::async_controller asyBNCtrl;

    virtual void ValueAssign(const LayerBN &lyrSrc) {
        dDecay                 = lyrSrc.dDecay;
        BdData.iTrainBatchCnt  = lyrSrc.BdData.iTrainBatchCnt;
        BdData.iTrainBatchIdx  = lyrSrc.BdData.iTrainBatchIdx;
        BdData.dCoeBatchSize   = lyrSrc.BdData.dCoeBatchSize;
        BdData.dCoeDbBatchSize = lyrSrc.BdData.dCoeDbBatchSize;
    }

    virtual void ValueCopy(const LayerBN &lyrSrc) {
        ValueAssign(lyrSrc);
        setInput              = lyrSrc.setInput;
        setGrad               = lyrSrc.setGrad;
        dBeta                 = lyrSrc.dBeta;
        dGamma                = lyrSrc.dGamma;
        vecBeta               = lyrSrc.vecBeta;
        vecGamma              = lyrSrc.vecGamma;
        vecNesterovBeta       = lyrSrc.vecNesterovBeta;
        vecNesterovGamma      = lyrSrc.vecNesterovGamma;
        vecGradBeta           = lyrSrc.vecGradBeta;
        vecGradGamma          = lyrSrc.vecGradGamma;
        advBeta               = lyrSrc.advBeta;
        advGamma              = lyrSrc.advGamma;
        adaBeta               = lyrSrc.adaBeta;
        adaGamma              = lyrSrc.adaGamma;
        BdData.setBarX        = lyrSrc.BdData.setBarX;
        BdData.setDist        = lyrSrc.BdData.setDist;
        BdData.vecMuBeta      = lyrSrc.BdData.vecMuBeta;
        BdData.vecSigma       = lyrSrc.BdData.vecSigma;
        BdData.vecSigmaDom    = lyrSrc.BdData.vecSigmaDom;
        BdData.vecSigmaSqr    = lyrSrc.BdData.vecSigmaSqr;
        BdData.vecExpMuBeta   = lyrSrc.BdData.vecExpMuBeta;
        BdData.vecExpSigmaSqr = lyrSrc.BdData.vecExpSigmaSqr;
    }

    virtual void ValueMove(LayerBN &&lyrSrc) {
        ValueAssign(lyrSrc);
        setInput              = std::move(lyrSrc.setInput);
        setGrad               = std::move(lyrSrc.setGrad);
        dBeta                 = std::move(lyrSrc.dBeta);
        dGamma                = std::move(lyrSrc.dGamma);
        vecBeta               = std::move(lyrSrc.vecBeta);
        vecGamma              = std::move(lyrSrc.vecGamma);
        vecNesterovBeta       = std::move(lyrSrc.vecNesterovBeta);
        vecNesterovGamma      = std::move(lyrSrc.vecNesterovGamma);
        vecGradBeta           = std::move(lyrSrc.vecGradBeta);
        vecGradGamma          = std::move(lyrSrc.vecGradGamma);
        advBeta               = std::move(lyrSrc.advBeta);
        advGamma              = std::move(lyrSrc.advGamma);
        adaBeta               = std::move(lyrSrc.adaBeta);
        adaGamma              = std::move(lyrSrc.adaGamma);
        BdData.setBarX        = std::move(lyrSrc.BdData.setBarX);
        BdData.setDist        = std::move(lyrSrc.BdData.setDist);
        BdData.vecSigma       = std::move(lyrSrc.BdData.vecSigma);
        BdData.vecMuBeta      = std::move(lyrSrc.BdData.vecMuBeta);
        BdData.vecSigmaDom    = std::move(lyrSrc.BdData.vecSigmaDom);
        BdData.vecSigmaSqr    = std::move(lyrSrc.BdData.vecSigmaSqr);
        BdData.vecExpMuBeta   = std::move(lyrSrc.BdData.vecExpMuBeta);
        BdData.vecExpSigmaSqr = std::move(lyrSrc.BdData.vecExpSigmaSqr);
        lyrSrc.Reset(false);
    }

    LayerBN(const matrix_elem_t &dShift = 0, const matrix_elem_t &dScale = 1, long double dInitLearnRate = 0, long double dDeduceDecay = 0.9l) : Layer(NEUNET_LAYER_BN, dInitLearnRate),
        dGamma(dScale),
        dBeta(dShift),
        dDecay(dDeduceDecay){}
    LayerBN(const LayerBN &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerBN(LayerBN &&lyrSrc) : Layer(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void RunInit(uint64_t iChannCnt, uint64_t iTrainBatchSize, uint64_t iTrainBatchCnt) {
        if (!vecBeta.verify) vecBeta  = BNInitBetaGamma(iChannCnt, dBeta);
        if (!vecGamma.verify) vecGamma = BNInitBetaGamma(iChannCnt, dGamma);
        if (this->dLearnRate) {
            vecNesterovBeta  = advBeta.weight(vecBeta);
            vecNesterovGamma = advGamma.weight(vecGamma);
        }
        setInput.init(iTrainBatchSize, false);
        setGrad.init(iTrainBatchSize, false);
        BNInitBNData(BdData, iTrainBatchSize, iTrainBatchCnt);
    }

    bool ForwProp() {
        if (dLearnRate) setGrad = BNTrain(BdData, setInput, vecNesterovBeta, vecNesterovGamma);
        else setGrad = BNTrain(BdData, setInput, vecBeta, vecGamma);
        return setGrad.length;
    }

    bool BackProp() {
        if (dLearnRate) setGrad = BNGradLossToInputGammaBeta(BdData, vecGradGamma, vecGradBeta, setGrad, vecNesterovGamma, dDecay);
        else setGrad = BNGradLossToInputGammaBeta(BdData, vecGradGamma, vecGradBeta, setGrad, vecGamma, dDecay);
        return setGrad.length;
    }

    bool ForwPropAsync(neunet_vect &vecInput, uint64_t iIdx) {
        setInput[iIdx] = std::move(vecInput);
        auto bFlag = true;
        if (++iLayerBatchSizeIdx == setInput.length) {
            bFlag = ForwProp();
            asyBNCtrl.thread_wake_all();
        } else asyBNCtrl.thread_sleep();
        vecInput = std::move(setGrad[iIdx]);
        return bFlag;
    }

    bool BackPropAsync(neunet_vect &vecGrad, uint64_t iIdx) {
        setGrad[iIdx] = std::move(vecGrad);
        auto bFlag = true;
        if (--iLayerBatchSizeIdx) asyBNCtrl.thread_sleep();
        else {
            bFlag = BackProp();
            asyBNCtrl.thread_wake_all();
            Update();
        }
        vecGrad = std::move(setGrad[iIdx]);
        return bFlag;
    }

    bool Deduce(neunet_vect &vecInput) {
        vecInput = BNDeduce(BdData, vecInput, vecBeta, vecGamma);
        return vecInput.verify;
    }

    void Update() {
        if (this->dLearnRate) {
            vecBeta         -= advBeta.momentum(vecGradBeta, this->dLearnRate);
            vecGamma        -= advGamma.momentum(vecGradGamma, this->dLearnRate);
            vecNesterovBeta  = advBeta.weight(vecBeta);
            vecNesterovGamma = advGamma.weight(vecGamma);
        } else {
            vecBeta  -= adaBeta.delta(vecGradBeta);
            vecGamma -= adaGamma.delta(vecGradGamma);
        }
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);
        dDecay                 = 0.9l;
        BdData.iTrainBatchCnt  = 0;
        BdData.iTrainBatchIdx  = 0;
        BdData.dCoeBatchSize   = 0;
        BdData.dCoeDbBatchSize = 0;
        dBeta                  = 0;
        dGamma                 = 0;
        setInput.reset();
        setGrad.reset();
        vecBeta.reset();
        vecGamma.reset();
        vecNesterovBeta.reset();
        vecNesterovGamma.reset();
        vecGradBeta.reset();
        vecGradGamma.reset();
        advBeta.reset();
        advGamma.reset();
        adaBeta.reset();
        adaGamma.reset();
        BdData.setBarX.reset();
        BdData.setDist.reset();
        BdData.vecMuBeta.reset();
        BdData.vecSigma.reset();
        BdData.vecExpMuBeta.reset();
        BdData.vecExpSigmaSqr.reset();
        BdData.vecSigmaDom.reset();
        BdData.vecSigmaSqr.reset();
    }

    virtual ~LayerBN() { Reset(false); }

    virtual LayerBN &operator=(const LayerBN &lyrSrc) {
        if (this->iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(lyrSrc);
            ValueCopy(lyrSrc);
        }
        return *this;
    }

    virtual LayerBN &operator=(LayerBN &&lyrSrc) {
        if (this->iLayerType == lyrSrc.iLayerType) {
            Layer::operator=(std::move(lyrSrc));
            ValueMove(std::move(lyrSrc));
        }
        return *this;
    }
};

LAYER_END
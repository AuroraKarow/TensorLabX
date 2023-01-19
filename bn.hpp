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

    void ValueAssign(const BNData &BdSrc) {
        iTrainBatchCnt  = BdSrc.iTrainBatchCnt;
        iTrainBatchIdx  = BdSrc.iTrainBatchIdx;
        dCoeBatchSize   = BdSrc.dCoeBatchSize;
        dCoeDbBatchSize = BdSrc.dCoeDbBatchSize;
    }

    void ValueCopy(const BNData &BdSrc) {
        ValueAssign(BdSrc);
        vecMuBeta      = BdSrc.vecMuBeta;
        vecSigmaSqr    = BdSrc.vecSigmaSqr;
        vecSigma       = BdSrc.vecSigma;
        vecSigmaDom    = BdSrc.vecSigmaDom;
        vecExpMuBeta   = BdSrc.vecExpMuBeta;
        vecExpSigmaSqr = BdSrc.vecExpSigmaSqr;
    }

    void ValueMove(BNData &&BdSrc) {
        ValueAssign(BdSrc);
        vecMuBeta      = std::move(BdSrc.vecMuBeta);
        vecSigmaSqr    = std::move(BdSrc.vecSigmaSqr);
        vecSigma       = std::move(BdSrc.vecSigma);
        vecSigmaDom    = std::move(BdSrc.vecSigmaDom);
        vecExpMuBeta   = std::move(BdSrc.vecExpMuBeta);
        vecExpSigmaSqr = std::move(BdSrc.vecExpSigmaSqr);
    }

    bool BNDataInit(uint64_t iTrainBatchSize = 1, uint64_t iTrainBatchCnt = 1) {
        if (!iTrainBatchSize) return false;
        this->iTrainBatchCnt = iTrainBatchCnt;
        dCoeBatchSize        = 1.l / iTrainBatchSize;
        dCoeDbBatchSize      = 2.l / iTrainBatchSize;
        setBarX.init(iTrainBatchSize, false);
        setDist.init(iTrainBatchSize, false);
        return true;
    }

    BNData(uint64_t iTrainBatchSize = 0, uint64_t iTrainBatchCnt = 0) { if (iTrainBatchSize && iTrainBatchCnt) BNDataInit(iTrainBatchSize, iTrainBatchCnt); }
    BNData(const BNData &BdSrc) { ValueCopy(BdSrc); }
    BNData(BNData &&BdSrc) { ValueMove(std::move(BdSrc)); }

    BNData &operator=(const BNData &BdSrc) {
        ValueCopy(BdSrc);
        return *this;
    }
    BNData &operator=(BNData &&BdSrc) {
        ValueMove(std::move(BdSrc));
        return *this;
    }

    void Reset() {
        iTrainBatchCnt  = 0;
        iTrainBatchIdx  = 0;
        dCoeBatchSize   = 0;
        dCoeDbBatchSize = 0;
        vecMuBeta.reset();
        vecSigmaSqr.reset();
        vecSigma.reset();
        vecSigmaDom.reset();
        vecExpMuBeta.reset();
        vecExpSigmaSqr.reset();
        setBarX.reset();
        setDist.reset();
    }

    ~BNData() { Reset(); }
};

callback_matrix net_set<neunet_vect> BNTrain (BNData<matrix_elem_t> &BdData, const net_set<neunet_vect> &setInput, const neunet_vect &vecBeta, const neunet_vect &vecGamma) {
    // Average, mu
    BdData.vecMuBeta = BdData.dCoeBatchSize * matrix::vect_sum(setInput);
    // Variance, sigma square
    if (!BdData.setDist.length) BdData.setDist.init(setInput.length, false);
    BdData.vecSigmaSqr = neunet_vect(BdData.vecMuBeta.line_count, BdData.vecMuBeta.column_count);
    for (auto i = 0ull; i < setInput.length; ++i) {
        BdData.setDist[i]   = setInput[i] - BdData.vecMuBeta;
        BdData.vecSigmaSqr += BdData.setDist[i].elem_wise_opt(BdData.setDist[i], MATRIX_ELEM_MULT);
    }
    BdData.vecSigmaSqr *= BdData.dCoeBatchSize;
    BdData.vecSigma     = BdData.vecSigmaSqr.elem_wise_opt(0.5l, MATRIX_ELEM_POW);
    BdData.vecSigmaDom  = BdData.vecSigma;
    // Bar X, normalize x
    if (!BdData.setBarX.length) BdData.setBarX.init(setInput.length, false);
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

matrix_declare struct LayerBN final : LayerDerive<matrix_elem_t> {
    std::atomic_uint64_t iBackBatSzCnt = 0,
                         iForBatSzCnt  = 0;

    long double dDecay          = .9,
                dBetaLearnRate  = .0,
                dGammaLearnRate = .0;

    neunet_vect vecBeta,
                vecGamma,
                vecBetaNv,
                vecGammaNv;

    ada_nesterov<matrix_elem_t> advBeta,
                                advGamma;

    ada_delta<matrix_elem_t> adaBeta,
                             adaGamma;

    BNData<matrix_elem_t> BdData;

    async::async_controller asyForCtrl,
                            asyBackCtrl;

    virtual void ValueAssign(const LayerBN &lyrSrc) {
        dDecay          = lyrSrc.dDecay;
        dBetaLearnRate  = lyrSrc.dBetaLearnRate;
        dGammaLearnRate = lyrSrc.dGammaLearnRate;
        iBackBatSzCnt   = (uint64_t)lyrSrc.iBackBatSzCnt;
        iForBatSzCnt    = (uint64_t)lyrSrc.iForBatSzCnt;
    }

    virtual void ValueCopy(const LayerBN &lyrSrc) {
        ValueAssign(lyrSrc);
        vecBeta   = lyrSrc.vecBeta;
        vecGamma  = lyrSrc.vecGamma;
        vecBetaNv = lyrSrc.vecBetaNv;
        vecGammaNv= lyrSrc.vecGammaNv;
        advBeta   = lyrSrc.advBeta;
        advGamma  = lyrSrc.advGamma;
        adaBeta   = lyrSrc.adaBeta;
        adaGamma  = lyrSrc.adaGamma;
        BdData    = lyrSrc.BdData;
    }

    virtual void ValueMove(LayerBN &&lyrSrc) {
        ValueAssign(lyrSrc);
        vecBeta    = std::move(lyrSrc.vecBeta);
        vecGamma   = std::move(lyrSrc.vecGamma);
        advBeta    = std::move(lyrSrc.advBeta);
        advGamma   = std::move(lyrSrc.advGamma);
        adaBeta    = std::move(lyrSrc.adaBeta);
        adaGamma   = std::move(lyrSrc.adaGamma);
        vecBetaNv  = std::move(lyrSrc.vecBetaNv);
        vecGammaNv = std::move(lyrSrc.vecGammaNv);
        BdData     = std::move(lyrSrc.BdData);
    }

    LayerBN(long double dShiftPlaceholder = 0., long double dScalePlaceholder = 1., long double dShiftLearnRate = 0., long double dScaleLearnRate = 0., long double dMovAvgDecay = .9) : LayerDerive<matrix_elem_t>(NEUNET_LAYER_BN),
        dDecay(dMovAvgDecay),
        dBetaLearnRate(dShiftLearnRate),
        dGammaLearnRate(dScaleLearnRate),
        vecBeta(dShiftPlaceholder),
        vecGamma(dScalePlaceholder) {}
    LayerBN(const LayerBN &lyrSrc) : LayerDerive<matrix_elem_t>(lyrSrc) { ValueCopy(lyrSrc); }
    LayerBN(LayerBN &&lyrSrc) : LayerDerive<matrix_elem_t>(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void Shape(uint64_t iChannCnt, uint64_t iBatSz, uint64_t iBatCnt) {
        auto dBeta  = vecBeta.atom,
             dGamma = vecGamma.atom;
        vecGamma    = BNInitBetaGamma(iChannCnt, dGamma);
        vecBeta     = BNInitBetaGamma(iChannCnt, dBeta);
        if (dBetaLearnRate) vecBetaNv = advBeta.weight(vecBeta);
        if (dGammaLearnRate) vecGammaNv = advGamma.weight(vecGamma);
        BdData.BNDataInit(iBatSz, iBatCnt);
        LayerDerive<matrix_elem_t>::Shape(iBatSz);
    }

    void Update(const neunet_vect &vecBetaGrad, const neunet_vect &vecGammaGrad) {
        if (dBetaLearnRate) {
            vecBeta  -= advBeta.momentum(vecBetaGrad, dBetaLearnRate);
            vecBetaNv = advBeta.weight(vecBeta);
        } else vecBeta -= adaBeta.delta(vecBetaGrad);
        if (dGammaLearnRate) {
            vecGamma  -= advGamma.momentum(vecGammaGrad, dGammaLearnRate);
            vecGammaNv = advGamma.weight(vecGamma);
        } else vecGamma -= adaGamma.delta(vecGammaGrad);
    }

    void ForProp(neunet_vect &vecIn, uint64_t iBatSzIdx) {
        this->setIn[iBatSzIdx] = std::move(vecIn);
        if (++iForBatSzCnt == this->setIn.length) {
            this->setIn  = BNTrain(BdData, this->setIn, dBetaLearnRate ? vecBetaNv : vecBeta, dGammaLearnRate ? vecGammaNv : vecGamma);
            iForBatSzCnt = 0;
            asyForCtrl.thread_wake_all();
        } else while (iForBatSzCnt) asyForCtrl.thread_sleep(1000);
        vecIn = std::move(this->setIn[iBatSzIdx]);
    }

    void BackProp(neunet_vect &vecGrad, uint64_t iBatSzIdx) {
        this->setIn[iBatSzIdx] = std::move(vecGrad);
        if (++iBackBatSzCnt == this->setIn.length) {
            neunet_vect vecBetaGrad,
                        vecGammaGrad;
            this->setIn   = BNGradLossToInputGammaBeta(BdData, vecGammaGrad, vecBetaGrad, this->setIn, dGammaLearnRate ? vecGammaNv : vecGamma, dDecay);
            iBackBatSzCnt = 0;
            asyBackCtrl.thread_wake_all();
            Update(vecBetaGrad, vecGammaGrad);
        } else while (iBackBatSzCnt) asyBackCtrl.thread_sleep(1000);
        vecGrad = std::move(this->setIn[iBatSzIdx]);
    }

    void Deduce(neunet_vect &vecIn) { vecIn = BNDeduce(BdData, vecIn, vecBeta, vecGamma); }

    LayerBN &operator=(const LayerBN &lyrSrc) {
        LayerDerive<matrix_elem_t>::operator=(lyrSrc);
        ValueCopy(lyrSrc);
        return *this;
    }
    LayerBN &operator=(LayerBN &&lyrSrc) {
        LayerDerive<matrix_elem_t>::operator=(std::move(lyrSrc));
        ValueMove(std::move(lyrSrc));
        return *this;
    }

    ~LayerBN() {
        dDecay          = .9;
        iBackBatSzCnt   = 0;
        iForBatSzCnt    = 0;
        dBetaLearnRate  = 0;
        dGammaLearnRate = 0;
        vecBeta.reset();
        vecGamma.reset();
        advBeta.reset();
        advGamma.reset();
        adaBeta.reset();
        adaGamma.reset();
        vecBetaNv.reset();
        vecGammaNv.reset();
        BdData.Reset();
    }
};

LAYER_END
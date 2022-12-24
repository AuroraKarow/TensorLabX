NEUNET_BEGIN

/* Batch normalization */

// BN parameters
callback_matrix neunet_vect BNInitBetaGamma(uint64_t iChannCnt, const matrix_elem_t &iPlaceholder = 0) {
    neunet_vect vecAns(iChannCnt, 1);
    if (iPlaceholder != 0) vecAns.fill_elem(iPlaceholder);
    return vecAns;
}

/* Expectation Average, Expectation MiuBeta
 * Variance mini-batch variance, Variance SigmaSqr
 */
callback_matrix void BNDeduceInit(neunet_vect &vecMuBeta, neunet_vect &vecSigmaSqr, uint64_t iBatchCnt, uint64_t iTrainBatchSize, long double dEpsilon = 1e-8l) {
    if(iBatchCnt) {
        vecMuBeta   = vecMuBeta.elem_wise_opt(iBatchCnt, MATRIX_ELEM_DIV);
        vecSigmaSqr = vecSigmaSqr.elem_wise_opt(iBatchCnt, MATRIX_ELEM_DIV);
        if(iTrainBatchSize > 1) vecSigmaSqr *= (iTrainBatchSize / (iTrainBatchSize - 1.0l));
        vecSigmaSqr = divisor_dominate(vecSigmaSqr, dEpsilon);
        vecSigmaSqr = vecSigmaSqr.elem_wise_opt(0.5l, MATRIX_ELEM_POW);
    }
}

/* Before deducing procedure, the value expectation and variant should be initialized.
 * Use function BNDeduceInit 
 */
callback_matrix neunet_vect BNDeduce(const neunet_vect &vecMuBeta, const neunet_vect &vecSigmaSqr, const neunet_vect &vecInput, const neunet_vect &vecBeta, const neunet_vect &vecGamma, long double dEpsilon = 1e-8l)
{
    auto vecAns = (vecInput - vecMuBeta).elem_wise_opt(vecSigmaSqr, MATRIX_ELEM_DIV);
    for (auto i = 0ull; i < vecAns.line_count; ++i) for (auto j = 0ull; j < vecAns.column_count; ++j) vecAns[i][j] = vecGamma.index(j) * vecAns[i][j] + vecBeta.index(j);    
    return vecAns;
}

matrix_declare struct BNData final {
    neunet_vect vecMuBeta;
    neunet_vect vecSigmaSqr;
    neunet_vect vecSigma;
    neunet_vect vecSigmaDom;

    long double dCoeBatchSize   = 0,
                dCoeDbBatchSize = 0;

    net_set<neunet_vect> setBarX;
    net_set<neunet_vect> setDist;
};

callback_matrix net_set<neunet_vect> BNTrain (BNData<matrix_elem_t> &BdData, const net_set<neunet_vect> &setInput, const neunet_vect &vecBeta, const neunet_vect &vecGamma, const matrix_elem_t &dEpsilon = 1e-8l) {
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
    for (auto i = 0ull; i < BdData.vecSigmaDom.element_count; ++i) if (BdData.vecSigmaDom.index(i) == 0) BdData.vecSigmaDom.index(i) = dEpsilon;
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

callback_matrix net_set<neunet_vect> BNGradLossToInputGammaBeta(neunet_vect &vecGradGamma, neunet_vect &vecGradBeta, net_set<neunet_vect> &setGradLossToOutput, const BNData<matrix_elem_t> &BdData, const neunet_vect &vecGamma, const matrix_elem_t &dEpsilon = 1e-8l) {
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
    for (auto i = 0ull; i < vecSigmaSqOnePtFive.element_count; ++i) if (vecSigmaSqOnePtFive.index(i) == 0) vecSigmaSqOnePtFive.index(i) = dEpsilon;
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
    return setGradInput;
}

NEUNET_END

LAYER_BEGIN

matrix_declare struct LayerBN : Layer {
    long double dEpsilon = 1e-8l,
                dDecay   = 0.9l;

    matrix_elem_t dBeta  = 0,
                  dGamma = 1;

    uint64_t iTrainBatchCnt = 0,
             iTrainBatchIdx = 0;

    ada_nesterov<matrix_elem_t> advBeta,
                                advGamma;

    ada_delta<matrix_elem_t> adaBeta,
                             adaGamma;

    neunet_vect vecBeta,
                vecGamma,

                vecNesterovBeta,
                vecNesterovGamma,

                vecGradBeta,
                vecGradGamma,

                vecExpMuBeta,
                vecExpSigmaSqr;

    net_set<neunet_vect> setInput,
                         setGrad;

    BNData<matrix_elem_t> BdData;

    async::async_controller asyBNCtrl;

    virtual void ValueAssign(const LayerBN &lyrSrc) {
        dEpsilon               = lyrSrc.dEpsilon;
        dDecay                 = lyrSrc.dDecay;
        iTrainBatchCnt         = lyrSrc.iTrainBatchCnt;
        iTrainBatchIdx         = lyrSrc.iTrainBatchIdx;
        BdData.dCoeBatchSize   = lyrSrc.BdData.dCoeBatchSize;
        BdData.dCoeDbBatchSize = lyrSrc.BdData.dCoeDbBatchSize;
    }

    virtual void ValueCopy(const LayerBN &lyrSrc) {
        ValueAssign(lyrSrc);
        setInput           = lyrSrc.setInput;
        setGrad            = lyrSrc.setGrad;
        dBeta              = lyrSrc.dBeta;
        dGamma             = lyrSrc.dGamma;
        vecBeta            = lyrSrc.vecBeta;
        vecGamma           = lyrSrc.vecGamma;
        vecNesterovBeta    = lyrSrc.vecNesterovBeta;
        vecNesterovGamma   = lyrSrc.vecNesterovGamma;
        vecGradBeta        = lyrSrc.vecGradBeta;
        vecGradGamma       = lyrSrc.vecGradGamma;
        advBeta            = lyrSrc.advBeta;
        advGamma           = lyrSrc.advGamma;
        adaBeta            = lyrSrc.adaBeta;
        adaGamma           = lyrSrc.adaGamma;
        vecExpMuBeta       = lyrSrc.vecExpMuBeta;
        vecExpSigmaSqr     = lyrSrc.vecExpSigmaSqr;
        BdData.setBarX     = lyrSrc.BdData.setBarX;
        BdData.setDist     = lyrSrc.BdData.setDist;
        BdData.vecMuBeta   = lyrSrc.BdData.vecMuBeta;
        BdData.vecSigma    = lyrSrc.BdData.vecSigma;
        BdData.vecSigmaDom = lyrSrc.BdData.vecSigmaDom;
        BdData.vecSigmaSqr = lyrSrc.BdData.vecSigmaSqr;
    }

    virtual void ValueMove(LayerBN &&lyrSrc) {
        ValueAssign(lyrSrc);
        setInput           = std::move(lyrSrc.setInput);
        setGrad            = std::move(lyrSrc.setGrad);
        dBeta              = std::move(lyrSrc.dBeta);
        dGamma             = std::move(lyrSrc.dGamma);
        vecBeta            = std::move(lyrSrc.vecBeta);
        vecGamma           = std::move(lyrSrc.vecGamma);
        vecNesterovBeta    = std::move(lyrSrc.vecNesterovBeta);
        vecNesterovGamma   = std::move(lyrSrc.vecNesterovGamma);
        vecGradBeta        = std::move(lyrSrc.vecGradBeta);
        vecGradGamma       = std::move(lyrSrc.vecGradGamma);
        advBeta            = std::move(lyrSrc.advBeta);
        advGamma           = std::move(lyrSrc.advGamma);
        adaBeta            = std::move(lyrSrc.adaBeta);
        adaGamma           = std::move(lyrSrc.adaGamma);
        vecExpMuBeta       = std::move(lyrSrc.vecExpMuBeta);
        vecExpSigmaSqr     = std::move(lyrSrc.vecExpSigmaSqr);
        BdData.setBarX     = std::move(lyrSrc.BdData.setBarX);
        BdData.setDist     = std::move(lyrSrc.BdData.setDist);
        BdData.vecMuBeta   = std::move(lyrSrc.BdData.vecMuBeta);
        BdData.vecSigma    = std::move(lyrSrc.BdData.vecSigma);
        BdData.vecSigmaDom = std::move(lyrSrc.BdData.vecSigmaDom);
        BdData.vecSigmaSqr = std::move(lyrSrc.BdData.vecSigmaSqr);
        lyrSrc.Reset(false);
    }

    LayerBN(const matrix_elem_t &dShift = 0, const matrix_elem_t &dScale = 1, long double dInitLearnRate = 0, long double dDeduceDecay = 0.9l, long double dDmt = 1e-8l) : Layer(NEUNET_LAYER_BN, dInitLearnRate),
        dGamma(dScale),
        dBeta(dShift),
        dDecay(dDeduceDecay),
        dEpsilon(dDmt) {}
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
        BdData.dCoeBatchSize   = 1.l / iTrainBatchSize;
        BdData.dCoeDbBatchSize = 2.l / iTrainBatchSize;
        this->iTrainBatchCnt   = iTrainBatchCnt;
    }

    bool ForwProp() {
        if (dLearnRate) setGrad = BNTrain(BdData, setInput, vecNesterovBeta, vecNesterovGamma, dEpsilon);
        else setGrad = BNTrain(BdData, setInput, vecBeta, vecGamma, dEpsilon);
        return setGrad.length;
    }

    bool BackProp() {
        if (dLearnRate) setGrad = BNGradLossToInputGammaBeta(vecGradGamma, vecGradBeta, setGrad, BdData, vecNesterovGamma, dEpsilon);
        else setGrad = BNGradLossToInputGammaBeta(vecGradGamma, vecGradBeta, setGrad, BdData, vecGamma, dEpsilon);
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
        vecInput = BNDeduce(vecExpMuBeta, BdData.vecSigmaSqr, vecInput, vecBeta, vecGamma, dEpsilon);
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
        // moving average
        if (vecExpMuBeta.verify && vecExpSigmaSqr.verify) {
            vecExpMuBeta   = dDecay * vecExpMuBeta + (1 - dDecay) * BdData.vecMuBeta;
            vecExpSigmaSqr = dDecay * vecExpSigmaSqr + (1 - dDecay) * BdData.vecSigmaSqr;
        } else {
            vecExpMuBeta   = std::move(BdData.vecMuBeta);
            vecExpSigmaSqr = std::move(BdData.vecSigmaSqr);            
        }
        if (++iTrainBatchIdx == iTrainBatchCnt) {
            iTrainBatchIdx     = 0;
            BdData.vecSigmaSqr = vecExpSigmaSqr.elem_wise_opt(0.5l, MATRIX_ELEM_POW);
        }
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);
        dEpsilon               = 1e-8l;
        dDecay                 = 0.9l;
        iTrainBatchCnt         = 0;
        iTrainBatchIdx         = 0;
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
        vecExpMuBeta.reset();
        vecExpSigmaSqr.reset();
        BdData.setBarX.reset();
        BdData.setDist.reset();
        BdData.vecMuBeta.reset();
        BdData.vecSigma.reset();
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
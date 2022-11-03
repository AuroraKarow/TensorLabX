NEUNET_BEGIN

callback_matrix neunet_vect VectElemExp(const net_set<neunet_vect> &setSrc) { return (1.0l / setSrc.length) * setSrc.sum; }

callback_matrix neunet_vect VectElemVar(const net_set<neunet_vect> &setSrc, const neunet_vect &vecExp) {
    neunet_vect vecAns; 
    for (auto i = 0ull; i < setSrc.length; ++i) {
        auto vecDistSqr = (setSrc[i] - vecExp).elem_wise_opt(2, MATRIX_ELEM_POW);
        if (i) vecAns += vecDistSqr;
        else vecAns = std::move(vecDistSqr);
    }
    vecAns *= (1.0l / setSrc.length);
    return vecAns;
}
callback_matrix neunet_vect VectElemVar(const net_set<neunet_vect> &setSrc) { return VectElemVar(setSrc, VectElemExp(setSrc)); }

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
callback_matrix void BNDeduceInit(neunet_vect &vecMuBeta, neunet_vect &vecSigmaSqr, uint64_t iBatchCnt, uint64_t iBatchSize, long double dEpsilon = 1e-8l) {
    if(iBatchCnt) {
        vecMuBeta   = vecMuBeta.elem_wise_opt(iBatchCnt, MATRIX_ELEM_DIV);
        vecSigmaSqr = vecSigmaSqr.elem_wise_opt(iBatchCnt, MATRIX_ELEM_DIV);
        if(iBatchSize > 1) vecSigmaSqr *= (iBatchSize / (iBatchSize - 1.0l));
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

callback_matrix net_set<neunet_vect> BNTrain (neunet_vect &vecMuBeta, neunet_vect &vecSigmaSqr, net_set<neunet_vect> &setBarX, const net_set<neunet_vect> &setInput, const neunet_vect &vecBeta, const neunet_vect &vecGamma, const matrix_elem_t &dEpsilon = 1e-8l) {
    // Average, miu
    vecMuBeta   = VectElemExp(setInput);
    // Variance, sigma square
    vecSigmaSqr = divisor_dominate(VectElemVar(setInput, vecMuBeta), dEpsilon);
    // Bar X, normalize x
    setBarX.init(setInput.length, false);
    auto vecSigma = vecSigmaSqr.elem_wise_opt(0.5l, MATRIX_ELEM_POW);
    for (auto i = 0ull; i < setInput.length; ++i) setBarX[i] = (setInput[i] - vecMuBeta).elem_wise_opt(vecSigma, MATRIX_ELEM_DIV);
    // Y, Output
    auto setY = setBarX;
    for (auto i = 0ull; i < setY.length; ++i) for (auto j = 0ull; j < setY[i].line_count; ++j) for (auto k = 0ull; k < setY[i].column_count; ++k) setY[i][j][k] = vecGamma.index(k) * setY[i][j][k] + vecBeta.index(k);
    return setY;
}

callback_matrix net_set<neunet_vect> BNGradLossToInput(const neunet_vect &vecMuBeta, const neunet_vect &vecSigmaSqr, const net_set<neunet_vect> &setBarX, const net_set<neunet_vect> &setInput, const net_set<neunet_vect> &setGradLossToOutput, const neunet_vect &vecGamma, const matrix_elem_t &dEpsilon = 1e-8l) {
    auto vecSigmaSqrDom = divisor_dominate(vecSigmaSqr, dEpsilon),
         vecSigma       = vecSigmaSqrDom.elem_wise_opt(0.5l, MATRIX_ELEM_POW);
    auto setGradBarX    = setGradLossToOutput;
    // Gradient bar x
    for (auto i = 0ull; i<setInput.length; ++i) for (auto j = 0ull; j < setBarX[i].element_count; ++j) {
        uint64_t iCurrChann = 0;
        if (vecGamma.element_count > 1) iCurrChann = matrix::elem_pos(j, setGradBarX[i].column_count).col;
        setGradBarX[i].index(j) *= vecGamma.index(iCurrChann);
    }
    // Gradient variant
    neunet_vect vecGradSigmaSqr(vecSigma.line_count, vecSigma.column_count);
    auto vecSigmaSqrAndOne = vecSigmaSqrDom.elem_wise_opt(1.5l, MATRIX_ELEM_POW);
    for (auto i = 0ull; i < setInput.length; ++i) vecGradSigmaSqr += setGradBarX[i].elem_wise_opt((setInput[i] - vecMuBeta), MATRIX_ELEM_MULT).elem_wise_opt(vecSigmaSqrAndOne, MATRIX_ELEM_DIV);
    vecGradSigmaSqr *= (-0.5l);
    // Gradient expextation
    neunet_vect vecDistanceSum(vecSigma.line_count, vecSigma.column_count);
    for (auto i = 0ull; i < setInput.length; ++i) vecDistanceSum += (setInput[i] - vecMuBeta);
    neunet_vect vecGradMuBeta = (-1) * setGradBarX.sum.elem_wise_opt(vecSigma, MATRIX_ELEM_DIV) + ((-2.0l) / setInput.length) * vecGradSigmaSqr.elem_wise_opt(vecDistanceSum, MATRIX_ELEM_MULT);
    // Gradient input
    net_set<neunet_vect> setGradInput(setInput.length);
    vecGradMuBeta *= ((1.0l) / setInput.length);
    for (auto i = 0ull; i < setGradInput.length; ++i) {
        setGradInput[i] = setGradBarX[i].elem_wise_opt(vecSigma, MATRIX_ELEM_DIV) + ((2.0l) / setInput.length) * vecGradSigmaSqr.elem_wise_opt((setInput[i] - vecMuBeta), MATRIX_ELEM_MULT) + vecGradMuBeta;
        if (!setGradInput[i].verify) {
            setGradInput.reset();
            break;
        }
    }
    return setGradInput;
}

callback_matrix neunet_vect BNGradLossToScale(const net_set<neunet_vect> &setBarX, const net_set<neunet_vect> &setGradLossToOutput) {
    neunet_vect vecGrad(setGradLossToOutput[0].column_count, 1);
    for (auto i = 0ull; i < setBarX.length; ++i) for (auto j = 0ull; j < setBarX[i].element_count; ++j) {
        auto iCurrChann = 0ull;
        if (vecGrad.element_count > 1) iCurrChann = matrix::elem_pos(j, setBarX[i].column_count).col;
        vecGrad.index(iCurrChann) += setGradLossToOutput[i].index(j) * setBarX[i].index(j);
    }
    return vecGrad;
}

callback_matrix neunet_vect BNGradLossToShift(const net_set<neunet_vect> &setGradLossToOutput) {
    neunet_vect vecGrad(setGradLossToOutput[0].column_count, 1);
    for (auto i = 0ull; i < setGradLossToOutput.length; ++i) for (auto j = 0ull; j < setGradLossToOutput[i].element_count; ++j) {
        auto iCurrChann = 0ull;
        if (vecGrad.element_count > 1) iCurrChann = matrix::elem_pos(j, setGradLossToOutput[i].column_count).col;
        vecGrad.index(iCurrChann) += setGradLossToOutput[i].index(j);
    }
    return vecGrad;
}

NEUNET_END

LAYER_BEGIN

matrix_declare struct LayerBN : Layer {
    uint64_t iTrainBatchCnt = 0,
             iTrainBatchIdx = 0;

    long double dEpsilon = 1e-8l,
                dDecay   = 0.9l;

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
                vecGradGamma,
    
                vecMuBeta,
                vecSigmaSqr,
                vecExpMuBeta,
                vecExpSigmaSqr;

    net_set<neunet_vect> setInput,
                         setBarX;

    async::async_controller asyBNCtrl;
    std::atomic_bool bBNFlag = false;
    
    virtual void ValueAssign(const LayerBN &lyrSrc) {
        bBNFlag          = (bool)lyrSrc.bBNFlag;
        dEpsilon         = lyrSrc.dEpsilon;
        dDecay           = lyrSrc.dDecay;
        iTrainBatchCnt   = lyrSrc.iTrainBatchCnt;
        iTrainBatchIdx   = lyrSrc.iTrainBatchIdx;
    }

    virtual void ValueCopy(const LayerBN &lyrSrc) {
        ValueAssign(lyrSrc);
        setInput         = lyrSrc.setInput;
        dBeta            = lyrSrc.dBeta;
        dGamma           = lyrSrc.dGamma;
        vecBeta          = lyrSrc.vecBeta;
        vecGamma         = lyrSrc.vecGamma;
        vecNesterovBeta  = lyrSrc.vecNesterovBeta;
        vecNesterovGamma = lyrSrc.vecNesterovGamma;
        vecGradBeta      = lyrSrc.vecGradBeta;
        vecGradGamma     = lyrSrc.vecGradGamma;
        advBeta          = lyrSrc.advBeta;
        advGamma         = lyrSrc.advGamma;
        adaBeta          = lyrSrc.adaBeta;
        adaGamma         = lyrSrc.adaGamma;
        vecMuBeta        = lyrSrc.vecMuBeta;
        vecSigmaSqr      = lyrSrc.vecSigmaSqr;
        setBarX          = lyrSrc.setBarX;
        vecExpMuBeta     = lyrSrc.vecMuBeta;
        vecExpSigmaSqr   = lyrSrc.vecExpSigmaSqr;
    }

    virtual void ValueMove(LayerBN &&lyrSrc) {
        ValueAssign(lyrSrc);
        setInput         = std::move(lyrSrc.setInput);
        dBeta            = std::move(lyrSrc.dBeta);
        dGamma           = std::move(lyrSrc.dGamma);
        vecBeta          = std::move(lyrSrc.vecBeta);
        vecGamma         = std::move(lyrSrc.vecGamma);
        vecNesterovBeta  = std::move(lyrSrc.vecNesterovBeta);
        vecNesterovGamma = std::move(lyrSrc.vecNesterovGamma);
        vecGradBeta      = std::move(lyrSrc.vecGradBeta);
        vecGradGamma     = std::move(lyrSrc.vecGradGamma);
        advBeta          = std::move(lyrSrc.advBeta);
        advGamma         = std::move(lyrSrc.advGamma);
        adaBeta          = std::move(lyrSrc.adaBeta);
        adaGamma         = std::move(lyrSrc.adaGamma);
        vecMuBeta        = std::move(lyrSrc.vecMuBeta);
        vecSigmaSqr      = std::move(lyrSrc.vecSigmaSqr);
        setBarX          = std::move(lyrSrc.setBarX);
        vecExpMuBeta     = std::move(lyrSrc.vecMuBeta);
        vecExpSigmaSqr   = std::move(lyrSrc.vecExpSigmaSqr);
        lyrSrc.Reset(false);
    }

    LayerBN(const matrix_elem_t &dShift = 0, const matrix_elem_t &dScale = 1, long double dInitLearnRate = 0, long double dDeduceDecay = 0.9l, long double dDmt = 1e-8l) : Layer(NEUNET_LAYER_BN, dInitLearnRate),
        dGamma(dScale),
        dBeta(dShift),
        dDecay(dDeduceDecay),
        dEpsilon(dDmt) {}
    LayerBN(const LayerBN &lyrSrc) : Layer(lyrSrc) { ValueCopy(lyrSrc); }
    LayerBN(LayerBN &&lyrSrc) : Layer(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void RunInit(uint64_t iChannCnt, uint64_t iTrainBatchCnt) {
        vecBeta  = BNInitBetaGamma(iChannCnt, dBeta);
        vecGamma = BNInitBetaGamma(iChannCnt, dGamma);
        if (this->dLearnRate) {
            vecNesterovBeta  = advBeta.weight(vecBeta);
            vecNesterovGamma = advGamma.weight(vecGamma);
        }
        this->iTrainBatchCnt = iTrainBatchCnt;
    }

    bool ForwProp(net_set<neunet_vect> &setInput) {
        this->setInput = std::move(setInput);
        if (this->dLearnRate) setInput = BNTrain(vecMuBeta, vecSigmaSqr, setBarX, this->setInput, vecNesterovBeta, vecNesterovGamma, dEpsilon);
        else setInput = BNTrain(vecMuBeta, vecSigmaSqr, setBarX, this->setInput, vecBeta, vecGamma, dEpsilon);
        return setInput.length;
    }

    bool BackProp(net_set<neunet_vect> &setGrad) {
        vecGradBeta  = BNGradLossToShift(setGrad);
        vecGradGamma = BNGradLossToScale(setBarX, setGrad);
        if (this->dLearnRate) setGrad = BNGradLossToInput(vecMuBeta, vecSigmaSqr, setBarX, setInput, setGrad, vecNesterovGamma, dEpsilon);
        else setGrad = BNGradLossToInput(vecMuBeta, vecSigmaSqr, setBarX, setInput, setGrad, vecGamma, dEpsilon);
        return setGrad.length;
    }

    bool ForwPropAsync(net_set<neunet_vect> &setInput) {
        if (++iLayerBatchSizeIdx == setInput.length) {
            bBNFlag = ForwProp(setInput);
            asyBNCtrl.thread_wake_all();
        } else asyBNCtrl.thread_sleep();
        return bBNFlag;
    }

    bool BackPropAsync(net_set<neunet_vect> &setGrad) {
        if (--iLayerBatchSizeIdx) asyBNCtrl.thread_sleep();
        else {
            bBNFlag = BackProp(setGrad);
            asyBNCtrl.thread_wake_all();
            Update();
        }
        return bBNFlag;
    }

    bool Deduce(neunet_vect &vecInput) {
        vecInput = BNDeduce(vecExpMuBeta, vecSigmaSqr, vecInput, vecBeta, vecGamma, dEpsilon);
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
            vecExpMuBeta   = dDecay * vecExpMuBeta + (1 - dDecay) * vecMuBeta;
            vecExpSigmaSqr = dDecay * vecExpSigmaSqr + (1 - dDecay) * vecSigmaSqr;
        } else {
            vecExpMuBeta   = std::move(vecMuBeta);
            vecExpSigmaSqr = std::move(vecSigmaSqr);            
        }
        if (++iTrainBatchIdx == iTrainBatchCnt) {
            iTrainBatchIdx = 0;
            vecSigmaSqr    = vecExpSigmaSqr.elem_wise_opt(0.5l, MATRIX_ELEM_POW);
        }
    }

    virtual void Reset(bool bFull = true) {
        if (bFull) Layer::Reset(true);
        iTrainBatchCnt = 0;
        iTrainBatchIdx = 0;
        dEpsilon       = 1e-8l;
        dBeta          = 0;
        dGamma         = 1;
        dDecay         = 0.9l;
        bBNFlag        = false;
        setInput.reset();        
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
        vecMuBeta.reset();
        vecSigmaSqr.reset();
        setBarX.reset();
        vecExpMuBeta.reset();
        vecExpSigmaSqr.reset();
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
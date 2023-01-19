CONV_BEGIN

net_set<uint64_t> CaffeTransformData(uint64_t iChannCnt, uint64_t &iCaffeLnCnt, uint64_t &iCaffeColCnt, uint64_t iChannLnCnt, uint64_t iChannColCnt, uint64_t &iSampChannLnCnt, uint64_t &iSampChannColCnt, uint64_t iFilterLnCnt, uint64_t iFilterColCnt, uint64_t iLnStride, uint64_t iColStride, uint64_t iLnDilate, uint64_t iColDilate) {
    auto iFilterElemCnt   = iFilterLnCnt * iFilterColCnt;
         iSampChannLnCnt  = samp_output_dir_cnt(iChannLnCnt, iFilterLnCnt, iLnStride, iLnDilate);
         iSampChannColCnt = samp_output_dir_cnt(iChannColCnt, iFilterColCnt, iColStride, iColDilate);
         iCaffeLnCnt      = iSampChannLnCnt * iSampChannColCnt;
         iCaffeColCnt     = iChannCnt * iFilterElemCnt;
    net_set<uint64_t> setAns(iCaffeLnCnt * iCaffeColCnt);
    for (auto c = 0ull; c < iChannCnt; ++c) for (auto i = 0ull; i < iSampChannLnCnt; ++i) for (auto j = 0ull; j < iSampChannColCnt; ++j) for (auto k = 0ull; k < iFilterLnCnt; ++k) for (auto l = 0ull; l < iFilterColCnt; ++l) {
        auto iMetaLn   = samp_trace_pos(i, k, iLnStride, iLnDilate),
             iMetaCol  = samp_trace_pos(j, l, iColStride, iColDilate),
             iInputLn  = matrix::elem_pos(iMetaLn, iMetaCol, iChannColCnt),
             iInputCol = c,
             iCaffeLn  = matrix::elem_pos(i, j, iSampChannColCnt),
             iCaffeCol = c * iFilterElemCnt + matrix::elem_pos(k, l, iFilterColCnt);
        setAns[matrix::elem_pos(iCaffeLn, iCaffeCol, iCaffeColCnt)] = matrix::elem_pos(iInputLn, iInputCol, iChannCnt);
    }
    return setAns;
}

callback_matrix neunet_vect CaffeTransform(const neunet_vect &vecChann, const net_set<uint64_t> &setCaffeData, uint64_t iCaffeLnCnt, uint64_t iCaffeColCnt) {
    neunet_vect vecAns(iCaffeLnCnt, iCaffeColCnt);
    for (auto i = 0ull; i < setCaffeData.length; ++i) vecAns.index(i) = vecChann.index(setCaffeData[i]);
    return vecAns;
}
callback_matrix neunet_vect CaffeTransform(const neunet_vect &vecCaffe, const net_set<uint64_t> &setCaffeData, uint64_t iChannElemCnt, uint64_t iChannCnt, bool bIsGrad) {
    neunet_vect vecAns(iChannElemCnt, iChannCnt);
    for (auto i = 0ull; i < setCaffeData.length; ++i) if (bIsGrad) vecAns.index(setCaffeData[i]) += vecCaffe.index(i);
    else vecAns.index(setCaffeData[i]) = vecCaffe.index(i);
    return vecAns;
}

callback_matrix neunet_vect InitKernel(uint64_t iKernelQty, uint64_t iChannCnt, uint64_t iLnCnt, uint64_t iColCnt, const matrix_elem_t &dFstRng = 0, const matrix_elem_t &dSndRng = 0, uint64_t iAcc = 8) { return neunet_vect(iLnCnt * iColCnt * iChannCnt, iKernelQty, true, dFstRng, dSndRng, iAcc); }

callback_matrix neunet_vect Conv(const neunet_vect &vecCaffeInput, const neunet_vect &vecKernel) { return fc::Output(vecKernel, vecCaffeInput); }

callback_matrix neunet_vect GradLossToConvCaffeInput(const neunet_vect &vecGradLossToOutput, const neunet_vect &vecKernelTranspose) { return fc::GradLossToWeight(vecGradLossToOutput, vecKernelTranspose); }

callback_matrix neunet_vect GradLossToConvKernal(const neunet_vect &vecGradLossToOutput, const neunet_vect &vecCaffeInputTranspose) { return fc::GradLossToInput(vecGradLossToOutput, vecCaffeInputTranspose); }

callback_matrix neunet_vect PoolGlbAvg(const neunet_vect &vecChann) {
    neunet_vect vecAns(1, vecChann.column_count);
    for (auto i = 0ull; i < vecChann.column_count; ++i) {
        for (auto j = 0ull; j < vecChann.line_count; ++j) vecAns.index(i) += vecChann[j][i];
        vecAns.index(i) /= vecChann.line_count;
    }
    return vecAns;
}

callback_matrix neunet_vect PoolAvg(const neunet_vect &vecChann, const net_set<uint64_t> &setCaffeData, uint64_t iFilterElemCnt, uint64_t iCaffeLnCnt) {
    neunet_vect vecAns(iCaffeLnCnt, vecChann.column_count);
    uint64_t iAnsElemCnt = 0,
             iStrideCnt  = 0;
    for (auto i = 0ull; i < setCaffeData.length; ++i) {
        vecAns.index(iAnsElemCnt) += vecChann.index(setCaffeData[i]);
        if (++iStrideCnt == iFilterElemCnt) {
            vecAns.index(iAnsElemCnt++) /= iFilterElemCnt;
            iStrideCnt = 0;
        }
    }
    return vecAns;
}

callback_matrix neunet_vect PoolMax(const neunet_vect &vecChann, const net_set<uint64_t> &setCaffeData, uint64_t iFilterElemCnt, uint64_t iCaffeLnCnt, net_set<net_list<uint64_t>> &setElemIdx, bool bTrainFlag = true) {
    neunet_vect vecAns(iCaffeLnCnt, vecChann.column_count);
    if (bTrainFlag) setElemIdx.init(vecAns.element_count, false);
    uint64_t iAnsElemCnt = 0,
             iStrideCnt  = 0;
    net_list<uint64_t> lsIdxTemp;
    matrix_elem_t dMaxValTemp = 0;
    for (auto i = 0ull; i < setCaffeData.length; ++i) {
        auto iCurrIdx  = setCaffeData[i];
        auto dCurrElem = vecChann.index(iCurrIdx);
        if (!iStrideCnt) dMaxValTemp = dCurrElem;
        if (dCurrElem >= dMaxValTemp) {
            if (dCurrElem > dMaxValTemp) {
                dMaxValTemp = dCurrElem;
                if (bTrainFlag) lsIdxTemp.reset();
            }
            if (bTrainFlag) lsIdxTemp.push_back(iCurrIdx);
        }
        if (++iStrideCnt == iFilterElemCnt) {
            if (bTrainFlag) setElemIdx[iAnsElemCnt] = std::move(lsIdxTemp);
            vecAns.index(iAnsElemCnt++) = dMaxValTemp;
            iStrideCnt = 0;
        }
    }
    return vecAns;
}

callback_matrix neunet_vect GradLossToPoolGlbAvgChann(const neunet_vect &vecGradLossToOutput, uint64_t iInputElemCnt) {
    neunet_vect vecAns(iInputElemCnt, vecGradLossToOutput.column_count);
    for (auto i = 0ull; i < vecAns.column_count; ++i) {
        auto dGradChann = vecGradLossToOutput.index(i) / iInputElemCnt;
        for(auto j = 0ull; j < iInputElemCnt; ++j) vecAns[j][i] = dGradChann;
    }
    return vecAns;
}

callback_matrix neunet_vect GradLossToPoolAvgChann(const neunet_vect &vecGradLossToOutput, const net_set<uint64_t> &setCaffeData, uint64_t iFilterElemCnt, uint64_t iChannElemCnt) {
    neunet_vect vecAns(iChannElemCnt, vecGradLossToOutput.column_count);
    uint64_t iCaffeDataCnt = 0;
    for (auto i = 0ull; i < vecGradLossToOutput.element_count; ++i) {
        auto dGradElem = vecGradLossToOutput.index(i) / iFilterElemCnt;
        for (auto j = 0ull; j < iFilterElemCnt; ++j) vecAns.index(setCaffeData[iCaffeDataCnt++]) += dGradElem;
    }
    return vecAns;
}

callback_matrix neunet_vect GradLossToPoolMaxChann(const neunet_vect &vecGradLossToOutput, uint64_t iChannElemCnt, const net_set<net_list<uint64_t>> &setElemIdx) {
    neunet_vect vecAns(iChannElemCnt, vecGradLossToOutput.column_count);
    for(auto i = 0ull; i < setElemIdx.length; ++i) for (auto iIdxTemp : setElemIdx[i]) vecAns.index(iIdxTemp) += vecGradLossToOutput.index(i);
    return vecAns;
}

CONV_END

LAYER_BEGIN

struct LayerCaffe : virtual LayerChann {
    uint64_t iLnStride     = 0,
             iColStride    = 0,
             iLnDilate     = 0,
             iColDilate    = 0,

             iCaffeLnCnt   = 0,
             iCaffeColCnt  = 0,

             iFilterLnCnt  = 0,
             iFilterColCnt = 0;

    net_set<uint64_t> setCaffeData;

    void ValueAssign(const LayerCaffe &lyrSrc) {
        iLnStride     = iLnStride;
        iColStride    = iColStride;
        iLnDilate     = iLnDilate;
        iColDilate    = iColDilate;
        iCaffeLnCnt   = iCaffeLnCnt;
        iCaffeColCnt  = iCaffeColCnt;
        iFilterLnCnt  = iFilterLnCnt;
        iFilterColCnt = iFilterColCnt;
    }

    void ValueCopy(const LayerCaffe &lyrSrc) {
        ValueAssign(lyrSrc);
        setCaffeData = lyrSrc.setCaffeData;
    }

    void ValueMove(LayerCaffe &&lyrSrc) {
        ValueAssign(lyrSrc);
        setCaffeData = std::move(lyrSrc.setCaffeData);
    }

    LayerCaffe(uint64_t iLayerType = NEUNET_LAYER_NULL, uint64_t iFilterLnCnt = 0, uint64_t iFilterColCnt = 0, uint64_t iLnStride = 0, uint64_t iColStride = 0, uint64_t iLnDilate = 0, uint64_t iColDilate = 0) : LayerChann(iLayerType),
        iFilterLnCnt(iFilterLnCnt),
        iFilterColCnt(iFilterColCnt),
        iLnStride(iLnStride),
        iColStride(iColStride),
        iLnDilate(iLnDilate),
        iColDilate(iColDilate) {}
    LayerCaffe(const LayerCaffe &lyrSrc) : LayerChann(lyrSrc) { ValueCopy(lyrSrc); }
    LayerCaffe(LayerCaffe &&lyrSrc) : LayerChann(lyrSrc) { ValueMove(std::move(lyrSrc)); }

    void Shape(uint64_t &iInLnCnt, uint64_t &iInColCnt, uint64_t iChannCnt) {
        LayerChann::Shape(iInLnCnt, iInColCnt, iChannCnt);
        setCaffeData = conv::CaffeTransformData(iChannCnt, iCaffeLnCnt, iCaffeColCnt, iInLnCnt, iInColCnt, iInLnCnt, iInColCnt, iFilterLnCnt, iFilterColCnt, iLnStride, iColStride, iLnDilate, iColDilate);
    }

    LayerCaffe &operator=(const LayerCaffe &lyrSrc) {
        LayerChann::operator=(lyrSrc);
        ValueCopy(lyrSrc);
        return *this;
    }
    LayerCaffe &operator=(LayerCaffe &&lyrSrc) {
        LayerChann::operator=(lyrSrc);
        ValueMove(std::move(lyrSrc));
        return *this;
    }

    virtual ~LayerCaffe() {
        iLnStride     = 0;
        iColStride    = 0;
        iLnDilate     = 0;
        iColDilate    = 0;
        iCaffeLnCnt   = 0;
        iCaffeColCnt  = 0;
        iFilterLnCnt  = 0;
        iFilterColCnt = 0;
        setCaffeData.reset();
    }
};

matrix_declare struct LayerConv final : LayerDerive<matrix_elem_t>, LayerWeight<matrix_elem_t>, LayerCaffe {
    uint64_t iKernelQty = 0;

    void ValueAssign(const LayerConv &lyrSrc) { iKernelQty = lyrSrc.iKernelQty; }

    LayerConv(uint64_t iKernelQty = 0, uint64_t iKernelLnCnt = 0, uint64_t iKernelColCnt = 0, uint64_t iLnStride = 0, uint64_t iColStride = 0, uint64_t iLnDilate = 0, uint64_t iColDilate = 0, long double dLearnRate = 0, long double dRandFstRng = 0, long double dRandSndRng = 0, uint64_t dRandAcc = 0) : LayerDerive<matrix_elem_t>(NEUNET_LAYER_CONV), LayerWeight<matrix_elem_t>(NEUNET_LAYER_CONV, dLearnRate, dRandFstRng, dRandSndRng, dRandAcc), LayerCaffe(NEUNET_LAYER_CONV, iKernelLnCnt, iKernelColCnt, iLnStride, iColStride, iLnDilate, iColDilate),
        iKernelQty(iKernelQty) {}
    LayerConv(const LayerConv &lyrSrc) : LayerDerive<matrix_elem_t>(lyrSrc), LayerWeight<matrix_elem_t>(lyrSrc), LayerCaffe(lyrSrc) { ValueAssign(lyrSrc); }
    LayerConv(LayerConv &&lyrSrc) : LayerDerive<matrix_elem_t>(std::move(lyrSrc)), LayerWeight<matrix_elem_t>(std::move(lyrSrc)), LayerCaffe(std::move(lyrSrc)) { ValueAssign(lyrSrc); }

    void Shape(uint64_t &iInLncnt, uint64_t &iInColCnt, uint64_t &iChannCnt, uint64_t iBatSz) {
        this->vecWeight = conv::InitKernel(iKernelQty, iChannCnt, iFilterLnCnt, iFilterColCnt, this->dRandFstRng, this->dRandSndRng, this->iRandAcc);
        LayerDerive<matrix_elem_t>::Shape(iBatSz);
        LayerWeight<matrix_elem_t>::Shape(iBatSz, true);
        LayerCaffe::Shape(iInLncnt, iInColCnt, iChannCnt);
        iChannCnt = iKernelQty;
    }

    void ForProp(neunet_vect &vecIn, uint64_t iBatSzIdx) {
        this->setIn[iBatSzIdx] = conv::CaffeTransform(vecIn, setCaffeData, iCaffeLnCnt, iCaffeColCnt);
        if (this->dLearnRate) vecIn = conv::Conv(this->setIn[iBatSzIdx], this->vecWeightNv);
        else vecIn = conv::Conv(this->setIn[iBatSzIdx], this->vecWeight);
    }

    void BackProp(neunet_vect &vecGrad, uint64_t iBatSzIdx) {
        this->setWeightGrad[iBatSzIdx] = conv::GradLossToConvKernal(vecGrad, this->setIn[iBatSzIdx].transpose);
        vecGrad = conv::CaffeTransform(conv::GradLossToConvCaffeInput(vecGrad, this->vecWeightTp), setCaffeData, iInElemCnt, iChannCnt, true);
        if (++this->iBatSzCnt == this->setIn.length) {
            this->iBatSzCnt = 0;
            LayerWeight<matrix_elem_t>::Update(true);
        }
    }

    void Deduce(neunet_vect &vecIn) { vecIn = conv::Conv(conv::CaffeTransform(vecIn, setCaffeData, iCaffeLnCnt, iCaffeColCnt), this->vecWeight); }

    LayerConv &operator=(const LayerConv &lyrSrc) {
        ValueAssign(lyrSrc);
        LayerDerive<matrix_elem_t>::operator=(lyrSrc);
        LayerWeight<matrix_elem_t>::operator=(lyrSrc);
        LayerCaffe::operator=(lyrSrc);
        return *this;
    }
    LayerConv &operator=(LayerConv &&lyrSrc) {
        ValueAssign(lyrSrc);
        LayerDerive<matrix_elem_t>::operator=(std::move(lyrSrc));
        LayerWeight<matrix_elem_t>::operator=(std::move(lyrSrc));
        LayerCaffe::operator=(std::move(lyrSrc));
        return *this;
    }

    ~LayerConv() { iKernelQty = 0; }
};

struct LayerPool final : LayerCaffe {
    uint64_t iPoolType      = 0,

             iFilterElemCnt = 0;

    net_set<net_set<net_list<uint64_t>>> setElemIdx;

    void ValueAssign(const LayerPool &lyrSrc) {
        iPoolType      = lyrSrc.iPoolType;
        iFilterElemCnt = lyrSrc.iFilterElemCnt;
    }

    void ValueCopy(const LayerPool &lyrSrc) {
        ValueAssign(lyrSrc);
        if (iPoolType == NEUNET_POOL_MAX) setElemIdx = lyrSrc.setElemIdx;
    }

    void ValueMove(LayerPool &&lyrSrc) {
        ValueAssign(lyrSrc);
        if (iPoolType == NEUNET_POOL_MAX) setElemIdx = std::move(lyrSrc.setElemIdx);
    }

    LayerPool(uint64_t iPoolType = NEUNET_POOL_MAX, uint64_t iFilterLnCnt = 0, uint64_t iFilterColCnt = 0, uint64_t iLnStride = 0, uint64_t iColStride = 0, uint64_t iLnDilate = 0, uint64_t iColDilate = 0) : LayerCaffe(NEUNET_LAYER_POOL, iFilterLnCnt, iFilterColCnt, iLnStride, iColStride, iLnDilate, iColDilate),
        iPoolType(iPoolType),
        iFilterElemCnt(iFilterLnCnt * iFilterColCnt) {}
    LayerPool(const LayerPool &lyrSrc) : LayerCaffe(lyrSrc) { ValueCopy(lyrSrc); }
    LayerPool(LayerPool &&lyrSrc) : LayerCaffe(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void Shape(uint64_t &iInLnCnt, uint64_t &iInColCnt, uint64_t iChannCnt, uint64_t iBatSz = NULL) {
        if (iPoolType == NEUNET_POOL_GAG) {
            iInElemCnt = iInLnCnt * iInColCnt;
            iInLnCnt   = 1;
            iInColCnt  = 1;
        } else {
            LayerCaffe::Shape(iInLnCnt, iInColCnt, iChannCnt);
            if (iPoolType == NEUNET_POOL_MAX) setElemIdx.init(iBatSz, false);
        }
    }

    callback_matrix void ForProp(neunet_vect &vecIn, uint64_t iBatSzIdx = NULL, bool bTrainFlag = true) { switch (iPoolType) {
        case NEUNET_POOL_GAG: vecIn = conv::PoolGlbAvg(vecIn); break;
        case NEUNET_POOL_AVG: vecIn = conv::PoolAvg(vecIn, setCaffeData, iFilterElemCnt, iCaffeLnCnt); break;
        case NEUNET_POOL_MAX: vecIn = conv::PoolMax(vecIn, setCaffeData, iFilterElemCnt, iCaffeLnCnt, setElemIdx[iBatSzIdx], bTrainFlag) ; break;
        default: break;
    } }

    callback_matrix void BackProp(neunet_vect &vecGrad, uint64_t iBatSzIdx) { switch (iPoolType) {
        case NEUNET_POOL_GAG: vecGrad = conv::GradLossToPoolGlbAvgChann(vecGrad, iInElemCnt); break;
        case NEUNET_POOL_AVG: vecGrad = conv::GradLossToPoolAvgChann(vecGrad, setCaffeData, iFilterElemCnt, iInElemCnt); break;
        case NEUNET_POOL_MAX: vecGrad = conv::GradLossToPoolMaxChann(vecGrad, iInElemCnt, setElemIdx[iBatSzIdx]); break;
        default: break;
    } }

    callback_matrix void Deduce(neunet_vect &vecIn, uint64_t iBatSzIdx = NULL) { ForProp(vecIn, iBatSzIdx, false); }


    LayerPool &operator=(const LayerPool &lyrSrc) {
        LayerCaffe::operator=(lyrSrc);
        ValueCopy(lyrSrc);
        return *this;
    }
    LayerPool &operator=(LayerPool &&lyrSrc) {
        LayerCaffe::operator=(std::move(lyrSrc));
        ValueMove(std::move(lyrSrc));
        return *this;
    }

    ~LayerPool() {
        iPoolType      = 0;
        iFilterElemCnt = 0;
        if (iPoolType == NEUNET_POOL_MAX) setElemIdx.reset();
    }
};

LAYER_END
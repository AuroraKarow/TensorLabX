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
callback_matrix neunet_vect CaffeTransform(const neunet_vect &vecCaffe, const net_set<uint64_t> &setCaffeData, uint64_t iIm2ColLnCnt, uint64_t iIm2ColColCnt, bool bIsGrad) {
    neunet_vect vecAns(iIm2ColLnCnt, iIm2ColColCnt);
    for (auto i = 0ull; i < setCaffeData.length; ++i) if (bIsGrad) vecAns.index(setCaffeData[i]) += vecCaffe.index(i);
    else vecAns.index(setCaffeData[i]) = vecCaffe.index(i);
    return vecAns;
}

callback_matrix neunet_vect InitKernel(uint64_t iKernelQty, uint64_t iChannCnt, uint64_t iLnCnt, uint64_t iColCnt, const matrix_elem_t &dFstRng = 0, const matrix_elem_t &dSndRng = 0, uint64_t iAcc = 8) { return neunet_vect(iLnCnt * iColCnt * iChannCnt, iKernelQty, true, dFstRng, dSndRng, iAcc); }

callback_matrix neunet_vect Conv(const neunet_vect &vecCaffeInput, const neunet_vect &vecKernelChann) { return fc::Output(vecKernelChann, vecCaffeInput); }

callback_matrix neunet_vect GradLossToConvCaffeInput(const neunet_vect &vecGradLossToOutput, const neunet_vect &vecKernelChannTranspose) { return fc::GradLossToWeight(vecGradLossToOutput, vecKernelChannTranspose); }

callback_matrix neunet_vect GradLossToConvKernal(const neunet_vect &vecGradLossToOutput, const neunet_vect &vecCaffeInputTranspose) { return fc::GradLossToInput(vecGradLossToOutput, vecCaffeInputTranspose); }

callback_matrix neunet_vect PoolGlbAvg(const neunet_vect &vecChann) {
    neunet_vect vecAns(1, vecChann.column_count);
    for (auto i = 0ull; i < vecChann.column_count; ++i) {
        for (auto j = 0ull; j < vecChann.line_count; ++j) vecAns.index(i) += vecChann[j][i];
        vecAns.index(i) /= vecChann.line_count;
    }
    return vecAns;
}

callback_matrix neunet_vect PoolMaxAvg(uint64_t iPoolType, const neunet_vect &vecCaffeInput, uint64_t iChannCnt, uint64_t iFilterElemCnt, net_set<net_list<matrix::pos>> &setCaffeMaxPos) {
    neunet_vect vecAns(vecCaffeInput.line_count, iChannCnt);
    for (auto i = 0ull; i < iChannCnt; ++i)
        if (iPoolType == NEUNET_POOL_AVG) for (auto j = 0ull; j < vecAns.line_count; ++j) {
            // avg
            matrix_elem_t curr_val = 0;
            for (auto k = 0ull; k < iFilterElemCnt; ++k) curr_val += vecCaffeInput[j][i * iFilterElemCnt + k];
            vecAns[j][i] = curr_val / iFilterElemCnt;
        } else if (iPoolType == NEUNET_POOL_MAX) {
            if (!setCaffeMaxPos.length) setCaffeMaxPos.init(vecAns.element_count, false);
            // max
            auto iCurrColBegin = i * iFilterElemCnt,
                 iCurrColEnd   = iCurrColBegin + iFilterElemCnt - 1;
            for (auto j = 0ull; j < vecAns.line_count; ++j) {
                auto iCurrIdx = matrix::elem_pos(j, i, iChannCnt);
                setCaffeMaxPos[iCurrIdx] = vecCaffeInput.extremum_position(true, j, j, iCurrColBegin, iCurrColEnd);
                vecAns[j][i] = vecCaffeInput[setCaffeMaxPos[iCurrIdx][0].ln][setCaffeMaxPos[iCurrIdx][0].col];
            }
        } else {
            vecAns.reset();
            break;
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

callback_matrix neunet_vect GradLossToPoolMaxAvgCaffeInput(uint64_t iPoolType, const neunet_vect &vecGradLossToOutput, uint64_t iFilterElemCnt, const net_set<net_list<matrix::pos>> &setCaffeMaxPos) {
    neunet_vect vecAns(vecGradLossToOutput.line_count, iFilterElemCnt * vecGradLossToOutput.column_count);
    for (auto i = 0ull; i < vecGradLossToOutput.element_count; ++i) {
        auto posDim             = matrix::elem_pos(i, vecGradLossToOutput.column_count);
        auto iCurrChannBeginCol = posDim.col * iFilterElemCnt;
        if (iPoolType == NEUNET_POOL_AVG) {
            auto iAvgVal = vecGradLossToOutput.index(i) / iFilterElemCnt;
            for (auto j = 0ull; j < iFilterElemCnt; ++j) vecAns[posDim.ln][j + iCurrChannBeginCol] = iAvgVal;
        } else if (iPoolType == NEUNET_POOL_MAX && setCaffeMaxPos.length) for (auto temp : setCaffeMaxPos[i]) vecAns[temp.ln][temp.col] += vecGradLossToOutput.index(i);
        else {
            vecAns.reset();
            break;
        }
    }
    return vecAns;
}

CONV_END

LAYER_BEGIN

struct LayerCaffe : virtual LayerChann {
    uint64_t iLnStride  = 0,
             iColStride = 0,
             iLnDilate  = 0,
             iColDilate = 0,

             iCaffeLnCnt  = 0,
             iCaffeColCnt = 0,

             iFilterLnCnt  = 0,
             iFilterColCnt = 0;

    net_set<uint64_t> setCaffeData;

    void ValueAssign(const LayerCaffe &lyrSrc) {
        iLnStride  = iLnStride;
        iColStride = iColStride;
        iLnDilate  = iLnDilate;
        iColDilate = iColDilate;
        iCaffeLnCnt  = iCaffeLnCnt;
        iCaffeColCnt = iCaffeColCnt;
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
        iInElemCnt   = iInLnCnt * iInColCnt;
        setCaffeData = conv::CaffeTransformData(iChannCnt, iCaffeLnCnt, iCaffeColCnt, iInLnCnt, iInColCnt, iInLnCnt, iInColCnt, iFilterLnCnt, iFilterColCnt, iLnStride, iColStride, iLnDilate, iColDilate);
    }

    callback_matrix void ForProp(neunet_vect &vecIn) { vecIn = conv::CaffeTransform(vecIn, setCaffeData, iCaffeLnCnt, iCaffeColCnt); }

    callback_matrix void BackProp(neunet_vect &vecGrad) { vecGrad = conv::CaffeTransform(vecGrad, setCaffeData, iInElemCnt, vecGrad.column_count, true); }

    callback_matrix void Deduce(neunet_vect &vecIn) { ForProp(vecIn); }

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

matrix_declare struct LayerConv : LayerDerive<matrix_elem_t>, LayerWeight<matrix_elem_t>, LayerCaffe {
    uint64_t iKernelQty = 0;

    void ValueAssign(const LayerConv &lyrSrc) { iKernelQty = lyrSrc.iKernelQty; }

    LayerConv(uint64_t iKernelQty = 0, uint64_t iKernelLnCnt = 0, uint64_t iKernelColCnt = 0, uint64_t iLnStide = 0, uint64_t iColStride = 0, uint64_t iLnDilate = 0, uint64_t iColDilate = 0, long double dLearnRate = 0, long double dRandFstRng = 0, long double dRandSndRng = 0, uint64_t dRandAcc = 0, uint64_t iLayerType = NEUNET_LAYER_CONV) : LayerDerive<matrix_elem_t>(iLayerType), LayerWeight<matrix_elem_t>(iLayerType, dLearnRate, dRandFstRng, dRandSndRng, dRandAcc), LayerCaffe(iLayerType, iKernelLnCnt, iFilterColCnt, iLnStride, iColStride, iLnDilate, iColDilate),
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
        LayerCaffe::ForProp(vecIn);
        this->setIn[iBatSzIdx] = std::move(vecIn);
        if (this->dLearnRate) vecIn = conv::Conv(this->setIn[iBatSzIdx], this->vecWeightNv);
        else vecIn = conv::Conv(this->setIn[iBatSzIdx], this->vecWeight);
    }

    void BackProp(neunet_vect &vecGrad, uint64_t iBatSzIdx) {
        this->setWeightGrad[iBatSzIdx] = conv::GradLossToConvKernal(vecGrad, this->setIn[iBatSzIdx].transpose);
        vecGrad                        = conv::GradLossToConvCaffeInput(vecGrad, this->vecWeightTp);
        LayerCaffe::BackProp(vecGrad);
        if (++this->iBatSzCnt == this->setIn.length) {
            this->iBatSzCnt = 0;
            LayerWeight<matrix_elem_t>::Update(true);
        }
    }

    void Deduce(neunet_vect &vecIn) {
        LayerCaffe::ForProp(vecIn);
        vecIn = conv::Conv(vecIn, this->vecWeight);
    }

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

    virtual ~LayerConv() { iKernelQty = 0; }
};

struct LayerPool : LayerCaffe {
    uint64_t iPoolType      = 0,

             iFilterElemCnt = 0;

    net_set<net_set<net_list<matrix::pos>>> setPoolMaxPos;

    void ValueAssign(const LayerPool &lyrSrc) {
        iPoolType      = lyrSrc.iPoolType;
        iFilterElemCnt = lyrSrc.iFilterElemCnt;
    }

    void ValueCopy(const LayerPool &lyrSrc) {
        ValueAssign(lyrSrc);
        if (iPoolType == NEUNET_POOL_MAX) setPoolMaxPos = lyrSrc.setPoolMaxPos;
    }

    void ValueMove(LayerPool &&lyrSrc) {
        ValueAssign(lyrSrc);
        if (iPoolType == NEUNET_POOL_MAX) setPoolMaxPos = std::move(lyrSrc.setPoolMaxPos);
    }

    LayerPool(uint64_t iPoolType = NEUNET_POOL_MAX, uint64_t iFilterLnCnt = 0, uint64_t iFilterColCnt = 0, uint64_t iLnStride = 0, uint64_t iColStride = 0, uint64_t iLnDilate = 0, uint64_t iColDilate = 0, uint64_t iLayerType = NEUNET_LAYER_POOL) : LayerCaffe(iLayerType, iFilterLnCnt, iFilterColCnt, iLnStride, iColStride, iLnDilate, iColDilate),
        iPoolType(iPoolType) {}
    LayerPool(const LayerPool &lyrSrc) : LayerCaffe(lyrSrc) { ValueCopy(lyrSrc); }
    LayerPool(LayerPool &&lyrSrc) : LayerCaffe(std::move(lyrSrc)) { ValueMove(std::move(lyrSrc)); }

    void Shape(uint64_t &iInLnCnt, uint64_t &iInColCnt, uint64_t iChannCnt, uint64_t iBatSz = NULL) {
        iInElemCnt = iInLnCnt * iInColCnt;
        if (iPoolType == NEUNET_POOL_GAG) {
            iInLnCnt   = 1;
            iInColCnt  = 1;
        } else {
            LayerCaffe::Shape(iInLnCnt, iInColCnt, iChannCnt);
            if (iPoolType == NEUNET_POOL_MAX) setPoolMaxPos.init(iBatSz, false);
        }
    }

    callback_matrix void ForProp(neunet_vect &vecIn, uint64_t iBatSzIdx = NULL) {
        if (iPoolType == NEUNET_POOL_GAG) vecIn = conv::PoolGlbAvg(vecIn);
        else {
            auto iChannCnt = vecIn.column_count;
            net_set<net_list<matrix::pos>> setTemp;
            LayerCaffe::ForProp(vecIn);
            vecIn = conv::PoolMaxAvg(iPoolType, vecIn, iChannCnt, iFilterElemCnt, iPoolType == NEUNET_POOL_MAX ? setPoolMaxPos[iBatSzIdx] : setTemp);
        }
    }

    callback_matrix void BackProp(neunet_vect &vecGrad, uint64_t iBatSzIdx) {
        if (iPoolType == NEUNET_POOL_GAG) vecGrad = conv::GradLossToPoolGlbAvgChann(vecGrad, iInElemCnt);
        else {
            net_set<net_list<matrix::pos>> setTemp;
            vecGrad = conv::GradLossToPoolMaxAvgCaffeInput(iPoolType, vecGrad, iFilterElemCnt, iPoolType == NEUNET_POOL_MAX ? setPoolMaxPos[iBatSzIdx] : setTemp);
            LayerCaffe::BackProp(vecGrad);
        }
    }

    callback_matrix void Deduce(neunet_vect &vecIn, uint64_t iBatSzIdx = NULL) { ForProp(vecIn, iBatSzIdx); }


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

    virtual ~LayerPool() {
        iPoolType      = 0;
        iFilterElemCnt = 0;
        if (iPoolType == NEUNET_POOL_MAX) setPoolMaxPos.reset();
    }
};

LAYER_END
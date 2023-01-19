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

struct LayerChann : virtual Layer {
    uint64_t iInElemCnt = 0,
             iChannCnt  = 0;

    void ValueAssign(const LayerChann &lyrSrc) {
        iInElemCnt = lyrSrc.iInElemCnt;
        iChannCnt = lyrSrc.iChannCnt;
    }

    LayerChann(uint64_t iLayerType = NEUNET_LAYER_NULL) : Layer(iLayerType) {}
    LayerChann(const LayerChann &lyrSrc) : Layer(lyrSrc) { ValueAssign(lyrSrc); }

    void Shape(uint64_t iInLnCnt, uint64_t iInColCnt, uint64_t iChannCnt) {
        iInElemCnt      = iInLnCnt * iInColCnt;
        this->iChannCnt = iChannCnt;
    }

    LayerChann &operator=(const LayerChann &lyrSrc) {
        ValueAssign(lyrSrc);
        return *this;
    }

    virtual ~LayerChann() {
        iInElemCnt = 0;
        iChannCnt  = 0;
    }
};

struct LayerFlat final : LayerDim, LayerChann {
    LayerFlat() : LayerDim(NEUNET_LAYER_FLAT), LayerChann(NEUNET_LAYER_FLAT) {}
    LayerFlat(const LayerFlat &lyrSrc) : LayerDim(lyrSrc), LayerChann(lyrSrc) {}

    void Shape(uint64_t &iInLnCnt, uint64_t &iInColCnt, uint64_t &iChannCnt) {
        LayerChann::Shape(iInLnCnt, iInColCnt, iChannCnt);
        iOutLnCnt = iChannCnt * iInElemCnt;
        iInLnCnt  = iOutLnCnt;
        iInColCnt = 1;
        iChannCnt = 1;
    }
    
    callback_matrix void ForProp(neunet_vect &vecIn) { vecIn = vecIn.reshape(iOutLnCnt, 1); }

    callback_matrix void BackProp(neunet_vect &vecGrad) { vecGrad = vecGrad.reshape(iInElemCnt, iChannCnt); }

    callback_matrix void Deduce(neunet_vect &vecIn) { ForProp(vecIn); }

    LayerFlat &operator=(const LayerFlat &lyrSrc) {
        LayerDim::operator=(lyrSrc);
        LayerChann::operator=(lyrSrc);
        return *this;
    }

    ~LayerFlat() {}
};

matrix_declare struct LayerFC final : LayerDerive<matrix_elem_t>, LayerWeight<matrix_elem_t>, LayerDim {
    LayerFC(uint64_t iOutLnCnt = 1, long double dLearnRate = .0, long double dRandFstRng = .0, long double dRandSndRng = .0, uint64_t iRandAcc = 8) : LayerDerive<matrix_elem_t>(NEUNET_LAYER_FC), LayerWeight<matrix_elem_t>(NEUNET_LAYER_FC, dLearnRate, dRandFstRng, dRandSndRng, iRandAcc), LayerDim(NEUNET_LAYER_FC, iOutLnCnt) {}
    LayerFC(const LayerFC &lyrSrc) : LayerDerive<matrix_elem_t>(lyrSrc), LayerWeight<matrix_elem_t>(lyrSrc), LayerDim(lyrSrc) {}
    LayerFC(LayerFC &&lyrSrc) : LayerDerive<matrix_elem_t>(std::move(lyrSrc)), LayerWeight<matrix_elem_t>(std::move(lyrSrc)), LayerDim(lyrSrc) {}

    void Shape(uint64_t &iInLnCnt, uint64_t iBatSz) {
        this->vecWeight = fc::InitWeight(iInLnCnt, iOutLnCnt, this->dRandFstRng, this->dRandSndRng, this->iRandAcc);
        LayerDerive<matrix_elem_t>::Shape(iBatSz);
        LayerWeight<matrix_elem_t>::Shape(iBatSz, true);
        iInLnCnt = this->iOutLnCnt;
    }

    void ForProp(neunet_vect &vecIn, uint64_t iBatSzIdx) {
        this->setIn[iBatSzIdx] = std::move(vecIn);
        if (this->dLearnRate) vecIn = fc::Output(this->setIn[iBatSzIdx], this->vecWeightNv);
        else vecIn = fc::Output(this->setIn[iBatSzIdx], this->vecWeight);
    }

    void BackProp(neunet_vect &vecGrad, uint64_t iBatSzIdx) {
        this->setWeightGrad[iBatSzIdx] = fc::GradLossToWeight(vecGrad, this->setIn[iBatSzIdx].transpose);
        vecGrad                        = fc::GradLossToInput(vecGrad, this->vecWeightTp);
        if (++this->iBatSzCnt == this->setIn.length) {
            this->iBatSzCnt = 0;
            LayerWeight<matrix_elem_t>::Update(true);
        }
    }

    void Deduce(neunet_vect &vecIn) const { vecIn = fc::Output(vecIn, this->vecWeight); }

    LayerFC &operator=(const LayerFC &lyrSrc) {
        LayerDerive<matrix_elem_t>::operator=(lyrSrc);
        LayerWeight<matrix_elem_t>::operator=(lyrSrc);
        LayerDim::operator=(lyrSrc);
        return *this;
    }
    LayerFC &operator=(LayerFC &&lyrSrc) {
        LayerDerive<matrix_elem_t>::operator=(std::move(lyrSrc));
        LayerWeight<matrix_elem_t>::operator=(std::move(lyrSrc));
        LayerDim::operator=(lyrSrc);
        return *this;
    }

    ~LayerFC() {}
};

LAYER_END
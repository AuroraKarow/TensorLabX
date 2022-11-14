NEUNET_BEGIN

// parameter suite

struct NeunetCore {
    // property

    uint64_t iTrainBatchSize  = 0,
             iDeduceBatchSize = 0;
    
    long double dTrainAcc = 1e-8l;

    net_sequence<layer::NetLayerPtr> seqLayer;

    net_set<uint64_t> setTrainDataIdx;

    // tool for train & deduce

    std::atomic_uint64_t iNetStatCode   = 0,
                         iTrainProcCnt  = 0,
                         iDeduceProcCnt = 0,

                         iTrainAccCnt   = 0,
                         iTrainRcCnt    = 0,
                         iDeduceAccCnt  = 0,
                         iDeduceRcCnt   = 0;

    // asynchronous tool

    async::net_queue<uint64_t> queTrainAcc,
                               queTrainRc,
                               
                               queDeduceAcc,
                               queDeduceRc;

    async::async_pool asyPool;

    NeunetCore(uint64_t iTrainBatchSize = 16, uint64_t iDeduceBatchSize = 16, long double dTrainAcc = 1e-8l) :
        iTrainBatchSize(iTrainBatchSize),
        iDeduceBatchSize(iDeduceBatchSize),
        dTrainAcc(dTrainAcc),
        asyPool(iTrainBatchSize > iDeduceBatchSize ? iTrainBatchSize : iDeduceBatchSize),
        iNetStatCode(iTrainBatchSize && iDeduceBatchSize && dTrainAcc > 0 && dTrainAcc < 1 ? NEUNET_STAT_NRM : NEUNET_STAT_ERR) {}
};

// build-in function

/* Derivative class constructor parameter list for Layer instantiation (In-building)
 *
 * **Activation (LayerAct)**
 * 
 * uint64_t iCurrActType = NULL
 * 
 * **Padding & cropping (LayerPC)**
 * 
 * bool bIsPadMode      = true
 * uint64_t iTopCnt     = 0
 * uint64_t iRightCnt   = 0
 * uint64_t iBottomCnt  = 0
 * uint64_t iLeftCnt    = 0
 * uint64_t iLnDistCnt  = 0
 * uint64_t iColDistCnt = 0
 * 
 * **Transform (LayerTrans)**
 * 
 * void
 * 
 * **Fully connection (LayerFC)**
 * 
 * uint64_t iCurrOutputLnCnt        = 1
 * long double dInitLearnRate       = 0
 * const matrix_elem_t &dRandFstRng = 0
 * const matrix_elem_t &dRandSndRng = 0
 * uint64_t iRandAcc                = 8
 * 
 * **Convolution (LayerConv)**
 * 
 * uint64_t iCurrKernelAmt          = 0
 * uint64_t iCurrKernelLnCnt        = 0
 * uint64_t iCurrKernelColCnt       = 0
 * uint64_t iCurrLnStride           = 0
 * uint64_t iCurrColStride          = 0
 * uint64_t iCurrLnDilate           = 0
 * uint64_t iCurrColDilate          = 0
 * long double dInitLearnRate       = 0
 * const matrix_elem_t &dRandFstRng = 0
 * const matrix_elem_t &dRandSndRng = 0
 * uint64_t iRandAcc                = 8
 * 
 * **Pooling (LayerPool)**
 * 
 * uint64_t iCurrPoolType     = NEUNET_POOL_MAX
 * uint64_t iCurrFilterLnCnt  = 0
 * uint64_t iCurrFilterColCnt = 0
 * uint64_t iCurrLnStride     = 0
 * uint64_t iCurrColStride    = 0
 * uint64_t iCurrLnDilate     = 0
 * uint64_t iCurrColDilate    = 0
 * 
 * **Batch normalization (LayerBN)**
 * 
 * const matrix_elem_t &dShift = 0
 * const matrix_elem_t &dScale = 1
 * long double dInitLearnRate  = 0
 * long double dDmt            = 1e-8l
 * 
 * **Bias (LayerBias)**
 * 
 * long double dInitLearnRate       = 0
 * const matrix_elem_t &dRandFstRng = 0
 * const matrix_elem_t &dRandSndRng = 0
 * uint64_t iRandAcc                = 8
 */
template<typename LayerType, typename ... Args,  typename neunet_layer_type_v> bool AddLayer(NeunetCore &netCore, Args &&... argsLayerInit) { return netCore.seqLayer.emplace_back(std::make_shared<LayerType>(std::forward<Args>(argsLayerInit)...)); }

// initialization of neural network running
bool RunInit(NeunetCore &netCore, uint64_t iTrainDataCnt, uint64_t iDeduceDataCnt, uint64_t iInputLnCnt, uint64_t iInputColCnt, uint64_t iChannCnt) {
    if (netCore.iNetStatCode == NEUNET_STAT_ERR) return false;
    // data initialize
    if (iTrainDataCnt % netCore.iTrainBatchSize || iDeduceDataCnt % netCore.iDeduceBatchSize) {
        netCore.iNetStatCode = NEUNET_STAT_ERR;
        return false;
    }
    netCore.setTrainDataIdx.init(iTrainDataCnt);
    for (auto i = 0ull; i < iTrainDataCnt; ++i) netCore.setTrainDataIdx[i] = i;
    // layer
    auto iTrainBatchCnt = iTrainDataCnt / netCore.iTrainBatchSize;
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) switch (netCore.seqLayer[i]->iLayerType) {
    case NEUNET_LAYER_ACT: neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->RunInit(netCore.iTrainBatchSize); break;
    case NEUNET_LAYER_PC: neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt); break;
    case NEUNET_LAYER_TRANS: neunet_layer_cast<layer::NetLayerTrans>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iChannCnt); break;
    case NEUNET_LAYER_FC: neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->RunInit(iInputLnCnt, netCore.iTrainBatchSize, iTrainBatchCnt); break;
    case NEUNET_LAYER_CONV: neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iChannCnt, netCore.iTrainBatchSize, iTrainBatchCnt); break;
    case NEUNET_LAYER_POOL: neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iChannCnt, netCore.iTrainBatchSize); break;
    case NEUNET_LAYER_BN: neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->RunInit(iChannCnt, netCore.iTrainBatchSize, iTrainBatchCnt); break;
    case NEUNET_LAYER_BIAS: if (i) {
        // Bias layer could not be the first layer
        neunet_layer_cast<layer::NetLayerBias>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iChannCnt, netCore.iTrainBatchSize);
        break;
    } else return false;
    default: break;
    }
    return true;
}

// forward propagation
bool ForwProp(NeunetCore &netCore, vect &vecInput, uint64_t iLbl, uint64_t iIdx, uint64_t iEpoch, uint64_t iTrainBatchIdx) {
    if (netCore.iNetStatCode == NEUNET_STAT_ERR) return false;
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) {
        auto bFPFlag = true;
        switch (netCore.seqLayer[i]->iLayerType) {
        case NEUNET_LAYER_ACT: bFPFlag = neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->ForwProp(vecInput, iIdx); break;
        case NEUNET_LAYER_PC: bFPFlag = neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->ForwProp(vecInput); break;
        case NEUNET_LAYER_TRANS: bFPFlag = neunet_layer_cast<layer::NetLayerTrans>(netCore.seqLayer[i])->ForwProp(vecInput); break;
        case NEUNET_LAYER_FC: bFPFlag = neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->ForwProp(vecInput, iIdx, iEpoch, iTrainBatchIdx); break;
        case NEUNET_LAYER_CONV: bFPFlag = neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->ForwProp(vecInput, iIdx, iEpoch, iTrainBatchIdx); break;
        case NEUNET_LAYER_POOL: bFPFlag = neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->ForwProp(vecInput, iIdx); break;
        case NEUNET_LAYER_BN: bFPFlag = neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->ForwPropAsync(vecInput, iIdx, iEpoch, iTrainBatchIdx); break;
        case NEUNET_LAYER_BIAS: bFPFlag = neunet_layer_cast<layer::NetLayerBias>(netCore.seqLayer[i])->ForwProp(vecInput); break;
        default: break;
        }
        if (!bFPFlag) return false;
    }
    output_acc_rc(vecInput, netCore.dTrainAcc, iLbl, netCore.iTrainAccCnt, netCore.iTrainRcCnt);
    return true;
}

// backward propagation
bool BackProp(NeunetCore &netCore, vect &vecFPOutput, const vect &vecOrgn, uint64_t iIdx) {
    if (netCore.iNetStatCode == NEUNET_STAT_ERR) return false;
    if (netCore.iNetStatCode == NEUNET_STAT_END) return true;
    for (auto i = netCore.seqLayer.length; i; --i) {
        auto bBPFlag = true;
        auto iLyrIdx = i - 1;
        switch (netCore.seqLayer[iLyrIdx]->iLayerType) {
        case NEUNET_LAYER_ACT: bBPFlag = neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[iLyrIdx])->BackProp(vecFPOutput, vecOrgn, iIdx); break;
        case NEUNET_LAYER_PC: bBPFlag = neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[iLyrIdx])->BackProp(vecFPOutput); break;
        case NEUNET_LAYER_TRANS: bBPFlag = neunet_layer_cast<layer::NetLayerTrans>(netCore.seqLayer[iLyrIdx])->BackProp(vecFPOutput); break;
        case NEUNET_LAYER_FC: bBPFlag = neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[iLyrIdx])->BackProp(vecFPOutput, iIdx); break;
        case NEUNET_LAYER_CONV: bBPFlag = neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[iLyrIdx])->BackProp(vecFPOutput, iIdx); break;
        case NEUNET_LAYER_POOL: bBPFlag = neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[iLyrIdx])->BackProp(vecFPOutput, iIdx); break;
        case NEUNET_LAYER_BN: bBPFlag = neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[iLyrIdx])->BackPropAsync(vecFPOutput, iIdx); break;
        case NEUNET_LAYER_BIAS: bBPFlag = neunet_layer_cast<layer::NetLayerBias>(netCore.seqLayer[i])->BackProp(vecFPOutput); break;
        default: break;
        }
        if (!bBPFlag) return false;
    }
    return true;
}

// deduce
bool Deduce(NeunetCore &netCore, vect &vecInput, uint64_t iLbl, uint64_t iIdx, uint64_t iEpoch) {
    if (netCore.iNetStatCode == NEUNET_STAT_ERR) return false;
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) {
        auto bDdFlag = true;
        switch (netCore.seqLayer[i]->iLayerType) {
        case NEUNET_LAYER_ACT: bDdFlag = neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->Deduce(vecInput); break;
        case NEUNET_LAYER_PC: bDdFlag = neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->Deduce(vecInput); break;
        case NEUNET_LAYER_TRANS: bDdFlag = neunet_layer_cast<layer::NetLayerTrans>(netCore.seqLayer[i])->Deduce(vecInput); break;
        case NEUNET_LAYER_FC: bDdFlag = neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->Deduce(vecInput, iEpoch); break;
        case NEUNET_LAYER_CONV: bDdFlag = neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->Deduce(vecInput, iEpoch); break;
        case NEUNET_LAYER_POOL: bDdFlag = neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->Deduce(vecInput); break;
        case NEUNET_LAYER_BN: neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->Deduce(vecInput, iEpoch); break;
        case NEUNET_LAYER_BIAS: neunet_layer_cast<layer::NetLayerBias>(netCore.seqLayer[i])->Deduce(vecInput); break;
        default: break;
        }
        if (!bDdFlag) return false;
    }
    output_acc_rc(vecInput, netCore.dTrainAcc, iLbl, netCore.iDeduceAccCnt, netCore.iDeduceRcCnt);
    return true;
}

// network running

// error abortion
bool ErrorAbort(NeunetCore &netCore) {
    if (netCore.iNetStatCode != NEUNET_STAT_ERR) return false;
    netCore.queDeduceAcc.except_abort();
    netCore.queDeduceRc.except_abort();
    netCore.queTrainAcc.except_abort();
    netCore.queTrainRc.except_abort();
    return true;
}

// train & deduce process thread
void TrainDeduceThread(NeunetCore &netCore, const net_set<vect> &setTrainData, const net_set<uint64_t> &setTrainLbl, const net_set<vect> &setDeduceData, const net_set<uint64_t> &setDeduceLbl, uint64_t iLblTypeCnt) { for (auto i = 0ull; i < netCore.asyPool.size(); ++i) netCore.asyPool.add_task([&netCore, &setTrainData, &setTrainLbl, &setDeduceData, &setDeduceLbl, iLblTypeCnt, i](uint64_t iEpoch, uint64_t iTrainBatchCnt, uint64_t iDeduceBatchCnt) { while (netCore.iNetStatCode == NEUNET_STAT_NRM) {
    uint64_t iDataIdx  = i,
             iBatchIdx = 0;
    // train
    while (iBatchIdx < iTrainBatchCnt && i < netCore.iTrainBatchSize) {
        auto vecInput = setTrainData[netCore.setTrainDataIdx[iDataIdx]];
        auto iLbl     = setTrainLbl[netCore.setTrainDataIdx[iDataIdx]];
        if (!ForwProp(netCore, vecInput, iLbl, i, iEpoch, iBatchIdx)) {
            netCore.iNetStatCode = NEUNET_STAT_ERR;
            break;
        }
        iDataIdx += netCore.iTrainBatchSize;
        ++iBatchIdx;
        if (++netCore.iTrainProcCnt == netCore.iTrainBatchSize) {
            netCore.queTrainAcc.en_queue((uint64_t)netCore.iTrainAccCnt);
            netCore.queTrainRc.en_queue((uint64_t)netCore.iTrainRcCnt);
            netCore.iTrainAccCnt  = 0;
            netCore.iTrainRcCnt   = 0;
            netCore.iTrainProcCnt = 0;
            if (iBatchIdx == iTrainBatchCnt) netCore.setTrainDataIdx.shuffle();
        }
        auto vecOrgn = lbl_orgn(iLbl, iLblTypeCnt);
        if (!BackProp(netCore, vecInput, vecOrgn, i)) {
            netCore.iNetStatCode = NEUNET_STAT_ERR;
            break;
        }
    }
    if (ErrorAbort(netCore)) break;
    ++iEpoch;
    iDataIdx  = i;
    iBatchIdx = 0;
    while (iBatchIdx < iDeduceBatchCnt && i < netCore.iDeduceBatchSize) {
        auto vecInput = setDeduceData[iDataIdx];
        auto iLbl     = setDeduceLbl[iDataIdx];
        if (!Deduce(netCore, vecInput, iLbl, i, iEpoch)) {
            netCore.iNetStatCode = NEUNET_STAT_ERR;
            break;
        }
        iDataIdx += netCore.iDeduceBatchSize;
        ++iBatchIdx;
    }
    if (++netCore.iDeduceProcCnt == netCore.asyPool.size()) {
        netCore.queDeduceAcc.en_queue((uint64_t)netCore.iDeduceAccCnt);
        netCore.queDeduceRc.en_queue((uint64_t)netCore.iDeduceRcCnt);
        netCore.iDeduceAccCnt  = 0;
        netCore.iDeduceRcCnt   = 0;
        netCore.iDeduceProcCnt = 0;
    }
} }, 0, (setTrainData.length / netCore.iTrainBatchSize), (setDeduceData.length / netCore.iDeduceBatchSize)); }

// data show thread
void DataShowThread(NeunetCore &netCore, uint64_t iTrainDataCnt, uint64_t iDeduceDataCnt) {
    // train & deduce parameter
    long double dAcc = .0l,
                dRc  = .0l;
    // epoch
    uint64_t iTrainBatchCnt  = iTrainDataCnt / netCore.iTrainBatchSize,
             iEp             = 0ull;
    while (dRc < 1) {
        auto iEpBgTP = NEUNET_CHRONO_TIME_POINT;
        // train
        for (auto i = 0ull; i < iTrainBatchCnt; ++i) {
            auto iTrnBgTP = NEUNET_CHRONO_TIME_POINT;
            if (netCore.iNetStatCode == NEUNET_STAT_ERR) return;
            dAcc = (1.l * netCore.queTrainAcc.de_queue()) / netCore.iTrainBatchSize;
            dRc = (1.l * netCore.queTrainRc.de_queue()) / netCore.iTrainBatchSize;
            auto iTrnEdTP = NEUNET_CHRONO_TIME_POINT;
            print_train_progress((i + 1), iTrainBatchCnt, dAcc, dRc, (iTrnEdTP - iTrnBgTP));
        }
        // deduce
        std::printf("\r[Deduce] ... ");
        dAcc = (1.l * netCore.queDeduceAcc.de_queue()) / iDeduceDataCnt;
        dRc  = (1.l * netCore.queDeduceRc.de_queue()) / iDeduceDataCnt;
        auto iEpEdTP = NEUNET_CHRONO_TIME_POINT;
        print_epoch_status(++iEp, dAcc, dRc, iEpEdTP - iEpBgTP);
    }
    netCore.iNetStatCode = NEUNET_STAT_END;
}

bool TestShow(NeunetCore &netSrc, vect &vecInput, uint64_t iLbl, uint64_t iInputLnCnt, uint64_t iInputColCnt) {
    if (RunInit(netSrc, 32, 32, iInputLnCnt, iInputColCnt, 1) && Deduce(netSrc, vecInput, iLbl, 0, 0)) {
        print_output(vecInput, iLbl);
        return true;
    } else return false;
}

NEUNET_END
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

    uint64_t iTrainDataIdx   = 0,
             iDeduceDataIdx  = 0;

    std::atomic_uint64_t iNetStatCode = 0,
                         iAccCnt      = 0,
                         iRcCnt       = 0;
    
    net_set<vect> setInput,
                  setOrgn;

    net_set<uint64_t> setLbl;

    // asynchronous tool

    async::net_queue<uint64_t> queTrainAcc,
                               queTrainRc,
                               
                               queDeduceAcc,
                               queDeduceRc;

    async::async_pool pool;

    async::async_concurrent asyConcurr;

    NeunetCore(uint64_t iTrainBatchSize = 16, uint64_t iDeduceBatchSize = 16, long double dTrainAcc = 1e-8l) :
        iTrainBatchSize(iTrainBatchSize),
        iDeduceBatchSize(iDeduceBatchSize),
        dTrainAcc(dTrainAcc),
        setInput(iTrainBatchSize),
        setOrgn(iTrainBatchSize),
        setLbl(iTrainBatchSize),
        pool(iTrainBatchSize > iDeduceBatchSize ? iTrainBatchSize + 1 : iDeduceBatchSize + 1),
        iNetStatCode(iTrainBatchSize && iDeduceBatchSize && dTrainAcc > 0 && dTrainAcc < 1 ? NEUNET_STAT_TRN : NEUNET_STAT_EXC)  { asyConcurr.batch_size = pool.size() - 1; }
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
 */
template<typename LayerType, typename ... Args,  typename neunet_layer_type_v> bool AddLayer(NeunetCore &netCore, Args &&... argsLayerInit) { return netCore.seqLayer.emplace_back(std::make_shared<LayerType>(std::forward<Args>(argsLayerInit)...)); }

// initialization of neural network running
bool RunInit(NeunetCore &netCore, uint64_t iTrainDataCnt, uint64_t iDeduceDataCnt, uint64_t iInputLnCnt, uint64_t iInputColCnt, uint64_t iChannCnt) {
    if (netCore.iNetStatCode == NEUNET_STAT_EXC) return false;
    // data initialize
    if (iTrainDataCnt % netCore.iTrainBatchSize || iDeduceDataCnt % netCore.iDeduceBatchSize) {
        netCore.iNetStatCode = NEUNET_STAT_EXC;
        return false;
    }
    netCore.setTrainDataIdx.init(iTrainDataCnt);
    for (auto i = 0ull; i < iTrainDataCnt; ++i) netCore.setTrainDataIdx[i] = i;
    // layer
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) switch (netCore.seqLayer[i]->iLayerType) {
    case NEUNET_LAYER_ACT: neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->RunInit(netCore.iTrainBatchSize); break;
    case NEUNET_LAYER_PC: neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt); break;
    case NEUNET_LAYER_TRANS: neunet_layer_cast<layer::NetLayerTrans>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iChannCnt); break;
    case NEUNET_LAYER_FC: neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->RunInit(iInputLnCnt, netCore.iTrainBatchSize); break;
    case NEUNET_LAYER_CONV: neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iChannCnt, netCore.iTrainBatchSize); break;
    case NEUNET_LAYER_POOL: neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, netCore.iTrainBatchSize); break;
    case NEUNET_LAYER_BN: neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->RunInit(iChannCnt, (iTrainDataCnt / netCore.iTrainBatchSize)); break;
    default: break;
    }
    return true;
}

// forward propagation
bool ForwProp(NeunetCore &netCore, uint64_t iIdx) {
    if (netCore.iNetStatCode == NEUNET_STAT_EXC) return false;
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) {
        auto bFPFlag = true;
        switch (netCore.seqLayer[i]->iLayerType) {
        case NEUNET_LAYER_ACT: bFPFlag = neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->ForwProp(netCore.setInput[iIdx], iIdx); break;
        case NEUNET_LAYER_PC: bFPFlag = neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->ForwProp(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_TRANS: bFPFlag = neunet_layer_cast<layer::NetLayerTrans>(netCore.seqLayer[i])->ForwProp(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_FC: bFPFlag = neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->ForwProp(netCore.setInput[iIdx], iIdx); break;
        case NEUNET_LAYER_CONV: bFPFlag = neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->ForwProp(netCore.setInput[iIdx], iIdx); break;
        case NEUNET_LAYER_POOL: bFPFlag = neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->ForwProp(netCore.setInput[iIdx], iIdx); break;
        case NEUNET_LAYER_BN: bFPFlag = neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->ForwPropAsync(netCore.setInput); break;
        default: break;
        }
        if (!bFPFlag) return false;
    }
    output_acc_rc(netCore.setInput[iIdx], netCore.dTrainAcc, netCore.setLbl[iIdx], netCore.iAccCnt, netCore.iRcCnt);
    return true;
}

// backward propagation
bool BackProp(NeunetCore &netCore, uint64_t iIdx) {
    if (netCore.iNetStatCode == NEUNET_STAT_EXC) return false;
    if (netCore.iNetStatCode == NEUNET_STAT_END) return true;
    for (auto i = netCore.seqLayer.length; i; --i) {
        auto bBPFlag = true;
        auto iLyrIdx = i - 1;
        switch (netCore.seqLayer[iLyrIdx]->iLayerType) {
        case NEUNET_LAYER_ACT: bBPFlag = neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[iLyrIdx])->BackProp(netCore.setInput[iIdx], netCore.setOrgn[iIdx], iIdx); break;
        case NEUNET_LAYER_PC: bBPFlag = neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[iLyrIdx])->BackProp(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_TRANS: bBPFlag = neunet_layer_cast<layer::NetLayerTrans>(netCore.seqLayer[iLyrIdx])->BackProp(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_FC: bBPFlag = neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[iLyrIdx])->BackProp(netCore.setInput[iIdx], iIdx); break;
        case NEUNET_LAYER_CONV: bBPFlag = neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[iLyrIdx])->BackProp(netCore.setInput[iIdx], iIdx); break;
        case NEUNET_LAYER_POOL: bBPFlag = neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[iLyrIdx])->BackProp(netCore.setInput[iIdx], iIdx); break;
        case NEUNET_LAYER_BN: bBPFlag = neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[iLyrIdx])->BackPropAsync(netCore.setInput); break;
        default: break;
        }
        if (!bBPFlag) return false;
    }
    return true;
}

// deduce
bool Deduce(NeunetCore &netCore, uint64_t iIdx) {
    if (netCore.iNetStatCode == NEUNET_STAT_EXC) return false;
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) {
        auto bDdFlag = true;
        switch (netCore.seqLayer[i]->iLayerType) {
        case NEUNET_LAYER_ACT: bDdFlag = neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->Deduce(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_PC: bDdFlag = neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->Deduce(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_TRANS: bDdFlag = neunet_layer_cast<layer::NetLayerTrans>(netCore.seqLayer[i])->Deduce(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_FC: bDdFlag = neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->Deduce(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_CONV: bDdFlag = neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->Deduce(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_POOL: bDdFlag = neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->Deduce(netCore.setInput[iIdx]); break;
        case NEUNET_LAYER_BN: neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->Deduce(netCore.setInput[iIdx]); break;
        default: break;
        }
        if (!bDdFlag) return false;
    }
    output_acc_rc(netCore.setInput[iIdx], netCore.dTrainAcc, netCore.setLbl[iIdx], netCore.iAccCnt, netCore.iRcCnt);
    return true;
}

// network running

// exception abortion
bool ExceptionAbortion(NeunetCore &netCore) {
    if (netCore.iNetStatCode != NEUNET_STAT_EXC) return false;
    netCore.asyConcurr.main_thread_exception();
    netCore.queDeduceAcc.except_abort();
    netCore.queDeduceRc.except_abort();
    netCore.queTrainAcc.except_abort();
    netCore.queTrainRc.except_abort();
    return true;
}

// train & deduce process thread
void TrainDeduceThread(NeunetCore &netCore, const net_set<vect> &setTrainData, const net_set<uint64_t> &setTrainLbl, const net_set<vect> &setDeduceData, const net_set<uint64_t> &setDeduceLbl, uint64_t iLblTypeCnt) { for (auto i = 0ull; i < netCore.asyConcurr.batch_size; ++i) netCore.pool.add_task([&netCore, &setTrainData, &setTrainLbl, &setDeduceData, &setDeduceLbl, iLblTypeCnt, i]{ while (true) {
    netCore.asyConcurr.batch_thread_attach();
    if (netCore.iNetStatCode == NEUNET_STAT_END || netCore.iNetStatCode == NEUNET_STAT_EXC) break;
    // train
    if (netCore.iNetStatCode == NEUNET_STAT_TRN && i < netCore.iTrainBatchSize) {
        netCore.setInput[i] = setTrainData[netCore.setTrainDataIdx[i + netCore.iTrainDataIdx]];
        netCore.setLbl[i]   = setTrainLbl[netCore.setTrainDataIdx[i + netCore.iTrainDataIdx]];
        netCore.setOrgn[i]  = lbl_orgn(netCore.setLbl[i], iLblTypeCnt);
        if (!(ForwProp(netCore, i) && BackProp(netCore, i))) netCore.iNetStatCode = NEUNET_STAT_EXC;
    }
    // deduce
    if (netCore.iNetStatCode == NEUNET_STAT_DED && i < netCore.iDeduceBatchSize) {
        netCore.setInput[i] = setDeduceData[i + netCore.iDeduceDataIdx];
        netCore.setLbl[i]   = setDeduceLbl[i + netCore.iDeduceDataIdx];
        if (!Deduce(netCore, i)) netCore.iNetStatCode = NEUNET_STAT_EXC;
    }
    netCore.asyConcurr.batch_thread_detach();
} }); netCore.pool.add_task([&netCore](uint64_t iTrainDataCnt, uint64_t iDeduceDataCnt){ while (true) {
    // train
    netCore.iNetStatCode  = NEUNET_STAT_TRN;
    netCore.iTrainDataIdx = 0;
    netCore.setTrainDataIdx.shuffle();
    netCore.setInput.init(netCore.iTrainBatchSize, false);
    netCore.setLbl.init(netCore.iTrainBatchSize, false);
    while (netCore.iTrainDataIdx != iTrainDataCnt) {
        netCore.asyConcurr.main_thread_deploy_batch_thread();
        if (ExceptionAbortion(netCore)) break;
        netCore.queTrainAcc.en_queue((uint64_t)netCore.iAccCnt);
        netCore.queTrainRc.en_queue((uint64_t)netCore.iRcCnt);
        netCore.iAccCnt = 0;
        netCore.iRcCnt  = 0;
        netCore.iTrainDataIdx += netCore.iTrainBatchSize;
    }
    if (netCore.iNetStatCode == NEUNET_STAT_EXC) break;
    // deduce
    netCore.iNetStatCode   = NEUNET_STAT_DED;
    netCore.iDeduceDataIdx = 0;
    while (netCore.iDeduceDataIdx != iDeduceDataCnt) {
        netCore.asyConcurr.main_thread_deploy_batch_thread();
        if (ExceptionAbortion(netCore)) break;
        netCore.iDeduceDataIdx += netCore.iDeduceBatchSize;
    }
    if (netCore.iNetStatCode == NEUNET_STAT_EXC) break;
    netCore.queDeduceAcc.en_queue((uint64_t)netCore.iAccCnt);
    netCore.queDeduceRc.en_queue((uint64_t)netCore.iRcCnt);
    netCore.iAccCnt = 0;
    netCore.iRcCnt  = 0;
} }, setTrainData.length, setDeduceData.length); }

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
            if (netCore.iNetStatCode == NEUNET_STAT_EXC) return;
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

NEUNET_END
NEUNET_BEGIN

// parameter suite

struct NeunetCore {
    // Train & test batch size
    uint64_t iTrnBatSz = 32,
             iTstBatSz = 32;
    // train precision
    long double dTrnPrec = .1;
    // network status
    std::atomic_uint64_t iNetStat  = 0,
    // batch size counter
                         iBatSzCnt = 0,
    // accuracy counter
                         iAccCnt   = 0,
    // recall counter
                         iRcCnt    = 0;
    // layer sequence
    net_sequence<layer::NetLayerPtr> seqLayer;
    // train accuracy counter queue
    async::net_queue<uint64_t> queTrnAcc,
    // train recall counter queue
                               queTrnRc,
    // test accuracy counter queue
                               queTstAcc,
    // test recall counter queue
                               queTstRc;
    // asynchronous train controller
    async::async_controller asyTrnCtrl,
    // asynchronous test controller
                            asyTstCtrl;
    // thread pool
    async::async_pool asyThdPool;

    NeunetCore(uint64_t iTrnBatSz = 32, uint64_t iTstBatSz = 32, long double dTrnPrec = .1) :
        iTrnBatSz(iTrnBatSz),
        iTstBatSz(iTstBatSz),
        dTrnPrec(dTrnPrec),
        asyThdPool(iTrnBatSz > iTstBatSz ? iTrnBatSz : iTstBatSz) {}
};

// build-in function

/* Derivative class constructor parameter list for Layer instantiation (In-building)
 *
 * **Activation (NetLayerAct)**
 * 
 * uint64_t iActFnType = NULL
 * 
 * **Padding & cropping (NetLayerPC)**
 * 
 * bool bIsPadMode      = true
 * uint64_t iTopCnt     = 0
 * uint64_t iRightCnt   = 0
 * uint64_t iBottomCnt  = 0
 * uint64_t iLeftCnt    = 0
 * uint64_t iLnDistCnt  = 0
 * uint64_t iColDistCnt = 0
 * 
 * **Transform (LayerFlat)**
 * 
 * void
 * 
 * **Fully connection (NetLayerFC)**
 * 
 * uint64_t    iOutLnCnt   =  1
 * long double dLearnRate  = .0
 * long double dRandFstRng = .0
 * long double dRandSndRng = .0
 * uint64_t    iRandAcc    =  8
 * 
 * **Convolution (NetLayerConv)**
 * 
 * uint64_t    iKernelQty    = 0
 * uint64_t    iKernelLnCnt  = 0
 * uint64_t    iKernelColCnt = 0
 * uint64_t    iLnStide      = 0
 * uint64_t    iColStride    = 0
 * uint64_t    iLnDilate     = 0
 * uint64_t    iColDilate    = 0
 * long double dLearnRate    = 0
 * long double dRandFstRng   = 0
 * long double dRandSndRng   = 0
 * uint64_t    dRandAcc      = 0
 * 
 * **Pooling (NetLayerPool)**
 * 
 * uint64_t iPoolType     = NEUNET_POOL_MAX
 * uint64_t iFilterLnCnt  = 0
 * uint64_t iFilterColCnt = 0
 * uint64_t iLnStride     = 0
 * uint64_t iColStride    = 0
 * uint64_t iLnDilate     = 0
 * uint64_t iColDilate    = 0
 * 
 * **Batch normalization (NetLayerBN)**
 * 
 * long double dShiftPlaceholder = 0.
 * long double dScalePlaceholder = 1.
 * long double dShiftLearnRate   = 0.
 * long double dScaleLearnRate   = 0.
 * long double dMovAvgDecay      = .9
 * 
 * **Bias (NetLayerBias)**
 * 
 * long double dLearnRate  = .0
 * long double dRandFstRng = .0
 * long double dRandSndRng = .0
 * uint64_t    iRandAcc    =  0
 */
template<typename LayerType, typename ... Args,  typename neunet_layer_type_v> bool AddLayer(NeunetCore &netCore, Args &&... argsLayerInit) { return netCore.seqLayer.emplace_back(std::make_shared<LayerType>(std::forward<Args>(argsLayerInit)...)); }

// initialization of neural network running
void Shape(NeunetCore &netCore, uint64_t iTrnBatCnt, uint64_t iInLnCnt, uint64_t iInColCnt, uint64_t iChannCnt) {
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) switch (netCore.seqLayer[i]->iLayerType) {
    case NEUNET_LAYER_ACT: neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->Shape(netCore.iTrnBatSz); break;
    case NEUNET_LAYER_PC: neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->Shape(iInLnCnt, iInColCnt); break;
    case NEUNET_LAYER_FLAT: neunet_layer_cast<layer::NetLayerFlat>(netCore.seqLayer[i])->Shape(iInLnCnt, iInColCnt, iChannCnt); break;
    case NEUNET_LAYER_FC: neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->Shape(iInLnCnt, netCore.iTrnBatSz); break;
    case NEUNET_LAYER_CONV: neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->Shape(iInLnCnt, iInColCnt, iChannCnt, netCore.iTrnBatSz); break;
    case NEUNET_LAYER_POOL: neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->Shape(iInLnCnt, iInColCnt, iChannCnt, netCore.iTrnBatSz); break;
    case NEUNET_LAYER_BN: neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->Shape(iChannCnt, netCore.iTrnBatSz, iTrnBatCnt); break;
    case NEUNET_LAYER_BIAS: neunet_layer_cast<layer::NetLayerBias>(netCore.seqLayer[i])->Shape(iInLnCnt, iInColCnt, iChannCnt, netCore.iTrnBatSz); break;
    default: break;
    }
}

// forward propagation
void ForProp(NeunetCore &netCore, vect &vecIn, uint64_t iBatSzIdx) {
    if (netCore.iNetStat != NEUNET_STAT_NRM) return;
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) { switch (netCore.seqLayer[i]->iLayerType) {
    case NEUNET_LAYER_ACT: neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->ForProp(vecIn, iBatSzIdx); break;
    case NEUNET_LAYER_PC: neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->ForProp(vecIn); break;
    case NEUNET_LAYER_FLAT: neunet_layer_cast<layer::NetLayerFlat>(netCore.seqLayer[i])->ForProp(vecIn); break;
    case NEUNET_LAYER_FC: neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->ForProp(vecIn, iBatSzIdx); break;
    case NEUNET_LAYER_CONV: neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->ForProp(vecIn, iBatSzIdx); break;
    case NEUNET_LAYER_POOL: neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->ForProp(vecIn, iBatSzIdx); break;
    case NEUNET_LAYER_BN: neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->ForProp(vecIn, iBatSzIdx); break;
    case NEUNET_LAYER_BIAS: neunet_layer_cast<layer::NetLayerBias>(netCore.seqLayer[i])->ForProp(vecIn); break;
    default: break;
    } }
    if (!vecIn.verify) netCore.iNetStat = NEUNET_STAT_ERR;
}

// backward propagation
void BackProp(NeunetCore &netCore, vect &vecForPropOut, const vect &vecOrgn, uint64_t iBatSzIdx) {
    if (netCore.iNetStat != NEUNET_STAT_NRM) return;
    for (auto i = netCore.seqLayer.length; i; --i) {
        auto iLyrIdx = i - 1;
        switch (netCore.seqLayer[iLyrIdx]->iLayerType) {
        case NEUNET_LAYER_ACT: neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[iLyrIdx])->BackProp(vecForPropOut, vecOrgn, iBatSzIdx); break;
        case NEUNET_LAYER_PC: neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[iLyrIdx])->BackProp(vecForPropOut); break;
        case NEUNET_LAYER_FLAT: neunet_layer_cast<layer::NetLayerFlat>(netCore.seqLayer[iLyrIdx])->BackProp(vecForPropOut); break;
        case NEUNET_LAYER_FC: neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[iLyrIdx])->BackProp(vecForPropOut, iBatSzIdx); break;
        case NEUNET_LAYER_CONV: neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[iLyrIdx])->BackProp(vecForPropOut, iBatSzIdx); break;
        case NEUNET_LAYER_POOL: neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[iLyrIdx])->BackProp(vecForPropOut, iBatSzIdx); break;
        case NEUNET_LAYER_BN: neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[iLyrIdx])->BackProp(vecForPropOut, iBatSzIdx); break;
        case NEUNET_LAYER_BIAS: neunet_layer_cast<layer::NetLayerBias>(netCore.seqLayer[i])->BackProp(vecForPropOut, iBatSzIdx); break;
        default: break;
        }
    }
    if (!vecForPropOut.verify) netCore.iNetStat = NEUNET_STAT_ERR;
}

// deduce
void Deduce(NeunetCore &netCore, vect &vecIn) {
    if (netCore.iNetStat != NEUNET_STAT_NRM) return;
    for (auto i = 0ull; i < netCore.seqLayer.length; ++i) { switch (netCore.seqLayer[i]->iLayerType) {
    case NEUNET_LAYER_ACT: neunet_layer_cast<layer::NetLayerAct>(netCore.seqLayer[i])->Deduce(vecIn); break;
    case NEUNET_LAYER_PC: neunet_layer_cast<layer::NetLayerPC>(netCore.seqLayer[i])->Deduce(vecIn); break;
    case NEUNET_LAYER_FLAT: neunet_layer_cast<layer::NetLayerFlat>(netCore.seqLayer[i])->Deduce(vecIn); break;
    case NEUNET_LAYER_FC: neunet_layer_cast<layer::NetLayerFC>(netCore.seqLayer[i])->Deduce(vecIn); break;
    case NEUNET_LAYER_CONV: neunet_layer_cast<layer::NetLayerConv>(netCore.seqLayer[i])->Deduce(vecIn); break;
    case NEUNET_LAYER_POOL: neunet_layer_cast<layer::NetLayerPool>(netCore.seqLayer[i])->Deduce(vecIn); break;
    case NEUNET_LAYER_BN: neunet_layer_cast<layer::NetLayerBN>(netCore.seqLayer[i])->Deduce(vecIn); break;
    case NEUNET_LAYER_BIAS: neunet_layer_cast<layer::NetLayerBias>(netCore.seqLayer[i])->Deduce(vecIn); break;
    default: break;
    } }
    if (!vecIn.verify) netCore.iNetStat = NEUNET_STAT_ERR;
}

// network running

// error abortion
bool ErrorAbort(NeunetCore &netCore) {
    if (netCore.iNetStat != NEUNET_STAT_ERR) return false;
    netCore.queTstAcc.err_abort();
    netCore.queTstRc.err_abort();
    netCore.queTrnAcc.err_abort();
    netCore.queTrnRc.err_abort();
    return true;
}

// stop pool
bool TaskOver(NeunetCore &netCore) { return netCore.iNetStat == NEUNET_STAT_END || netCore.iNetStat == NEUNET_STAT_ERR; }

// train & test process thread
void TrainTestThread(NeunetCore &netCore, const net_set<vect> &setTrnData, const net_set<uint64_t> &setTrnLbl, net_set<uint64_t> &setTrnDataIdx, const net_set<vect> &setTstData, const net_set<uint64_t> &setTstLbl, uint64_t iLblTypeCnt, uint64_t iTrnBatCnt, uint64_t iTstBatCnt) { for (auto i = 0ull; i < netCore.asyThdPool.size(); ++i) netCore.asyThdPool.add_task([&netCore, &setTrnData, &setTrnLbl, &setTstData, &setTstLbl, &setTrnDataIdx, iLblTypeCnt, iTrnBatCnt, iTstBatCnt, i](uint64_t iEpoch){ while (netCore.iNetStat == NEUNET_STAT_NRM) {
    uint64_t iDataIdx = i,
             iBatCnt  = 0;
    bool bLastMark = false;
    while (iBatCnt < iTrnBatCnt && i < netCore.iTrnBatSz) {
        // train
        auto iLbl    = setTrnLbl[setTrnDataIdx[iDataIdx]];
        auto vecIn   = setTrnData[setTrnDataIdx[iDataIdx]],
             vecOrgn = lbl_orgn(iLbl, iLblTypeCnt);
        if (iBatCnt || iEpoch) {
            if (bLastMark) bLastMark = false;
            else netCore.asyTrnCtrl.thread_sleep(1000);
        }
        // fp
        ForProp(netCore, vecIn, i);
        if (ErrorAbort(netCore)) break;
        output_acc_rc(vecIn, netCore.dTrnPrec, iLbl, netCore.iAccCnt, netCore.iRcCnt);
        // bp
        BackProp(netCore, vecIn, vecOrgn, i);
        if (ErrorAbort(netCore)) break;
        ++iBatCnt;
        iDataIdx += netCore.iTrnBatSz;
        if (++netCore.iBatSzCnt == iTrnBatCnt) {
            netCore.queTrnAcc.en_queue(netCore.iAccCnt);
            netCore.queTrnRc.en_queue(netCore.iRcCnt);
            netCore.iBatSzCnt = 0;
            bLastMark         = true;
            netCore.iAccCnt   = 0;
            netCore.iRcCnt    = 0;
            if (iBatCnt == iTrnBatCnt) {
                setTrnDataIdx.shuffle();
                netCore.asyTstCtrl.thread_wake_all();
            } else netCore.asyTrnCtrl.thread_wake_all();
        }
    }
    if (TaskOver(netCore)) break;
    // test
    iDataIdx = i;
    iBatCnt  = 0;
    while (iBatCnt < iTstBatCnt && i < netCore.iTstBatSz) {
        auto iLbl  = setTstLbl[iDataIdx];
        auto vecIn = setTstData[iDataIdx];
        if (!(iBatCnt || bLastMark)) netCore.asyTstCtrl.thread_sleep();
        if (ErrorAbort(netCore)) break;
        // deduce
        Deduce(netCore, vecIn);
        output_acc_rc(vecIn, netCore.dTrnPrec, iLbl, netCore.iAccCnt, netCore.iRcCnt);
        iDataIdx += netCore.iTstBatSz;
        ++iBatCnt;
    }
    if (TaskOver(netCore)) break;
    // next epoch
    if (++netCore.iBatSzCnt == netCore.asyThdPool.size()) {
        netCore.queTstAcc.en_queue(netCore.iAccCnt);
        netCore.queTstRc.en_queue(netCore.iRcCnt);
        netCore.iBatSzCnt = 0;
        netCore.iAccCnt   = 0;
        netCore.iRcCnt    = 0;
        netCore.asyTrnCtrl.thread_wake_all();
    }
    ++iEpoch;
} }, 0); }

// data show thread
void DataShowThread(NeunetCore &netCore, uint64_t iTrnBatCnt, uint64_t iTstDataCnt) {
    long double dRcRt  = .0;
    uint64_t    iEpCnt = 0;
    while (dRcRt < 1) {
        auto cEpTmPt = NEUNET_CHRONO_TIME_POINT;
        // train
        for (auto i = 0ull; i < iTrnBatCnt; ++i) {
            auto cTrnTmPt = NEUNET_CHRONO_TIME_POINT;
            auto dAcc     = netCore.queTrnAcc.de_queue() / (netCore.iTrnBatSz * 1.);
                 dRcRt    = netCore.queTrnRc.de_queue() / (netCore.iTrnBatSz * 1.);
            print_train_progress((i + 1), iTrnBatCnt, dAcc, dRcRt, (NEUNET_CHRONO_TIME_POINT - cTrnTmPt));
        }
        // test
        std::printf("\r[Deducing]...");
        auto dAcc  = netCore.queTstAcc.de_queue() / (iTstDataCnt * 1.);
             dRcRt = netCore.queTstRc.de_queue() / (iTstDataCnt * 1.);
        neunet::print_epoch_status(++iEpCnt, dAcc, dRcRt, (NEUNET_CHRONO_TIME_POINT - cEpTmPt));
    }
}

bool TestShow(NeunetCore &netSrc, vect &vecIn, uint64_t iLbl, uint64_t iInLnCnt, uint64_t iInColCnt) {
    Shape(netSrc, 16, iInLnCnt, iInColCnt, 1);
    Deduce(netSrc, vecIn);
    if (TaskOver(netSrc)) return false;
    print_output(vecIn, iLbl);
    return true;
}

NEUNET_END
NEUNET_BEGIN

class Neunet {
protected:
    virtual bool RunInit(uint64_t iNetTrainDataCnt, uint64_t iNetDeduceDataCnt, uint64_t iDataLblTypeCnt, uint64_t iInputLnCnt, uint64_t iInputColCnt, uint64_t iChannCnt) {
        iLblTypeCnt = iDataLblTypeCnt;
        // data initialize
        if (iTrainDataCnt % iTrainBatchSize) return false;
        iTrainDataCnt  = iNetTrainDataCnt;
        iTrainBatchCnt = iTrainDataCnt / iTrainBatchSize;
        iTrainDataIdx  = 0;
        setCurrOrgn.init(iTrainBatchSize);
        setTrainDataIdx.init(iTrainDataCnt);
        for (auto i = 0ull; i < setTrainDataIdx.length; ++i) setTrainDataIdx[i] = i;
        if (iDeduceDataCnt % iDeduceBatchSize) return false;
        iDeduceDataCnt  = iNetDeduceDataCnt;
        iDeduceBatchCnt = iDeduceDataCnt / iDeduceBatchSize;
        iDeduceDataIdx  = 0;
        // layer initialize
        for (auto i = 0ull; i < seqLayer.length; ++i) {
            switch (seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_ACT: std::dynamic_pointer_cast<NetLayerAct>(seqLayer[i])->RunInit(iTrainBatchSize); break;
            case NEUNET_LAYER_PC: std::dynamic_pointer_cast<NetLayerPC>(seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt); break;
            case NEUNET_LAYER_TRANS: std::dynamic_pointer_cast<NetLayerTrans>(seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iChannCnt); break;
            case NEUNET_LAYER_FC: std::dynamic_pointer_cast<NetLayerFC>(seqLayer[i])->RunInit(iInputLnCnt, iTrainBatchSize); break;
            case NEUNET_LAYER_CONV: std::dynamic_pointer_cast<NetLayerConv>(seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iChannCnt, iTrainBatchSize); break;
            case NEUNET_LAYER_POOL: std::dynamic_pointer_cast<NetLayerPool>(seqLayer[i])->RunInit(iInputLnCnt, iInputColCnt, iTrainBatchSize); break;
            case NEUNET_LAYER_BN: std::dynamic_pointer_cast<NetLayerBN>(seqLayer[i])->RunInit(iChannCnt); break;
            default: break;
            }
        }
        return true;
    }

    virtual bool ForwProp(uint64_t iIdx) {
        for (auto i = 0ull; i < seqLayer.length; ++i) {
            auto bFPFlag = true;
            switch (seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_ACT: bFPFlag = std::dynamic_pointer_cast<NetLayerAct>(seqLayer[i])->ForwProp(setCurrData[iIdx], iIdx); break;
            case NEUNET_LAYER_PC: bFPFlag = std::dynamic_pointer_cast<NetLayerPC>(seqLayer[i])->ForwProp(setCurrData[iIdx]); break;
            case NEUNET_LAYER_TRANS: bFPFlag = std::dynamic_pointer_cast<NetLayerTrans>(seqLayer[i])->ForwProp(setCurrData[iIdx]); break;
            case NEUNET_LAYER_FC: bFPFlag = std::dynamic_pointer_cast<NetLayerFC>(seqLayer[i])->ForwProp(setCurrData[iIdx], iIdx); break;
            case NEUNET_LAYER_CONV: bFPFlag = std::dynamic_pointer_cast<NetLayerConv>(seqLayer[i])->ForwProp(setCurrData[iIdx], iIdx); break;
            case NEUNET_LAYER_POOL: bFPFlag = std::dynamic_pointer_cast<NetLayerPool>(seqLayer[i])->ForwProp(setCurrData[iIdx], iIdx); break;
            case NEUNET_LAYER_BN: bFPFlag = std::dynamic_pointer_cast<NetLayerBN>(seqLayer[i])->ForwPropAsync(setCurrData); break;
            default: return true;
            }
            if (!bFPFlag) return false;
        }
        output_acc_rc(setCurrData[iIdx], dTrainAcc, setCurrLbl[iIdx], iTrainDeduceAccCnt, iTrainDeduceRcCnt);
        return true;
    }

    virtual bool BackProp(uint64_t iIdx) {
        for (auto i = seqLayer.length; i; --i) {
            auto bBPFlag = true;
            auto iLyrIdx = i - 1;
            switch (seqLayer[iLyrIdx]->iLayerType) {
            case NEUNET_LAYER_ACT: bBPFlag = std::dynamic_pointer_cast<NetLayerAct>(seqLayer[iLyrIdx])->BackProp(setCurrData[iIdx], setCurrOrgn[iIdx], iIdx); break;
            case NEUNET_LAYER_PC: bBPFlag = std::dynamic_pointer_cast<NetLayerPC>(seqLayer[iLyrIdx])->BackProp(setCurrData[iIdx]); break;
            case NEUNET_LAYER_TRANS: bBPFlag = std::dynamic_pointer_cast<NetLayerTrans>(seqLayer[iLyrIdx])->BackProp(setCurrData[iIdx]); break;
            case NEUNET_LAYER_FC: bBPFlag = std::dynamic_pointer_cast<NetLayerFC>(seqLayer[iLyrIdx])->BackProp(setCurrData[iIdx], iIdx); break;
            case NEUNET_LAYER_CONV: bBPFlag = std::dynamic_pointer_cast<NetLayerConv>(seqLayer[iLyrIdx])->BackProp(setCurrData[iIdx], iIdx); break;
            case NEUNET_LAYER_POOL: bBPFlag = std::dynamic_pointer_cast<NetLayerPool>(seqLayer[iLyrIdx])->BackProp(setCurrData[iIdx], iIdx); break;
            case NEUNET_LAYER_BN: bBPFlag = std::dynamic_pointer_cast<NetLayerBN>(seqLayer[iLyrIdx])->BackPropAsync(setCurrData, iCurrTrainBatchIdx, iTrainBatchCnt); break;
            default: return true;
            }
            if (!bBPFlag) return false;
        }
        return true;
    }

    virtual bool Deduce(uint64_t iIdx) {
        for (auto i = 0ull; i < seqLayer.length; ++i) {
            auto bDdFlag = true;
            switch (seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_ACT: bDdFlag = std::dynamic_pointer_cast<NetLayerAct>(seqLayer[i])->Deduce(setCurrData[iIdx]); break;
            case NEUNET_LAYER_PC: bDdFlag = std::dynamic_pointer_cast<NetLayerPC>(seqLayer[i])->Deduce(setCurrData[iIdx]); break;
            case NEUNET_LAYER_TRANS: bDdFlag = std::dynamic_pointer_cast<NetLayerTrans>(seqLayer[i])->Deduce(setCurrData[iIdx]); break;
            case NEUNET_LAYER_FC: bDdFlag = std::dynamic_pointer_cast<NetLayerFC>(seqLayer[i])->Deduce(setCurrData[iIdx]); break;
            case NEUNET_LAYER_CONV: bDdFlag = std::dynamic_pointer_cast<NetLayerConv>(seqLayer[i])->Deduce(setCurrData[iIdx]); break;
            case NEUNET_LAYER_POOL: bDdFlag = std::dynamic_pointer_cast<NetLayerPool>(seqLayer[i])->Deduce(setCurrData[iIdx]); break;
            case NEUNET_LAYER_BN: std::dynamic_pointer_cast<NetLayerBN>(seqLayer[i])->Deduce(setCurrData[iIdx]); break;
            default: bDdFlag = false; break;
            }
            if (!bDdFlag) return false;
        }
        output_acc_rc(setCurrData[iIdx], dTrainAcc, setCurrLbl[iIdx], iTrainDeduceAccCnt, iTrainDeduceRcCnt);
        return true;
    }

    // thread size - batch size
    virtual void ThreadBatchProcess(const net_set<vect> &setTrainData, const net_set<uint64_t> &setTrainLbl, const net_set<vect> &setDeduceData, const net_set<uint64_t> &setDeduceLbl) { for (auto i = 0ull; i < iRunBatchSize; ++i) asyPool.add_task([this, i, &setTrainData, &setTrainLbl, &setDeduceData, &setDeduceLbl] { while (true) {
        // batch process mode
        asyConcurr.batch_thread_attach();
        if (iCurrNetStat == NEUNET_STAT_END || iCurrNetStat == NEUNET_STAT_EXC) break;
        // train
        if (iCurrNetStat == NEUNET_STAT_TRN && i < iTrainBatchSize) {
            // load data
            setCurrData[i] = setTrainData[setTrainDataIdx[i + iTrainDataIdx]];
            setCurrLbl[i]  = setTrainLbl[setTrainDataIdx[i + iTrainDataIdx]];
            setCurrOrgn[i] = lbl_orgn(setCurrLbl[i], iLblTypeCnt);
            if (!(ForwProp(i) && BackProp(i))) iCurrNetStat = NEUNET_STAT_EXC;
        }
        // deduce
        if (iCurrNetStat == NEUNET_STAT_DED && i < iDeduceBatchSize) {
            // load data
            setCurrData[i] = setDeduceData[i + iDeduceDataIdx];
            setCurrLbl[i]  = setDeduceLbl[i + iDeduceDataIdx];
            if (!Deduce(i)) iCurrNetStat = NEUNET_STAT_EXC;
        }
        asyConcurr.batch_thread_detach();
    } }); }

    virtual void ThreadTrainDeduce(async::net_queue<uint64_t> &queTrainAcc, async::net_queue<uint64_t> &queTrainRc, async::net_queue<uint64_t> &queDeduceAcc, async::net_queue<uint64_t> &queDeduceRc) { asyPool.add_task([this, &queTrainAcc, &queTrainRc, &queDeduceAcc, &queDeduceRc] { while (true) {
        // train
        iCurrNetStat = NEUNET_STAT_TRN;
        setCurrData.init(iTrainBatchSize, false);
        setCurrLbl.init(iTrainBatchSize, false);
        for (iCurrTrainBatchIdx = 0; iCurrTrainBatchIdx < iTrainBatchCnt; ++iCurrTrainBatchIdx) {
            iTrainDeduceAccCnt = 0;
            iTrainDeduceRcCnt  = 0;
            asyConcurr.main_thread_deploy_batch_thread();
            if (iCurrNetStat == NEUNET_STAT_EXC) break;
            // output data
            queTrainAcc.en_queue((uint64_t)iTrainDeduceAccCnt);
            queTrainRc.en_queue((uint64_t)(iTrainDeduceRcCnt));
            // next batch
            iTrainDataIdx += iTrainBatchSize;
        }
        if (iCurrNetStat == NEUNET_STAT_EXC || iCurrNetStat == NEUNET_STAT_END) {
            asyConcurr.main_thread_exception();
            break;
        }
        iTrainDataIdx = 0;
        setTrainDataIdx.shuffle();
        // deduce
        iCurrNetStat = NEUNET_STAT_DED;
        iTrainDeduceAccCnt = 0;
        iTrainDeduceRcCnt  = 0;
        setCurrData.init(iDeduceBatchSize, false);
        setCurrLbl.init(iDeduceBatchSize, false);
        for (iCurrDeduceBatchIdx = 0; iCurrDeduceBatchIdx < iDeduceBatchCnt; ++iCurrDeduceBatchIdx) {
            asyConcurr.main_thread_deploy_batch_thread();
            if (iCurrNetStat == NEUNET_STAT_EXC) break;
            // next batch
            iDeduceDataIdx += iDeduceBatchSize;
        }
        if (iCurrNetStat == NEUNET_STAT_EXC || iCurrNetStat == NEUNET_STAT_END) {
            asyConcurr.main_thread_exception();
            break;
        }
        // output data
        queDeduceAcc.en_queue((uint64_t)iTrainDeduceAccCnt);
        queDeduceRc.en_queue((uint64_t)iTrainDeduceRcCnt);
        iDeduceDataIdx = 0;
    } }); }

    virtual void ThreadDataShow(async::net_queue<uint64_t> &queTrainAcc, async::net_queue<uint64_t> &queTrainRc, async::net_queue<uint64_t> &queDeduceAcc, async::net_queue<uint64_t> &queDeduceRc) {
        // train & deduce parameter
        long double dAcc = 0,
                    dRc  = 0;
        // epoch
        uint64_t iEp = 0ull;
        while (dRc < 1) {
            auto iEpBgTP = NEUNET_CHRONO_TIME_POINT;
            ++iEp;
            // train
            for (auto i = 0ull; i < iTrainBatchCnt; ++i) {
                auto iTrnBgTp = NEUNET_CHRONO_TIME_POINT;
                if (iCurrNetStat == NEUNET_STAT_EXC) return;
                dAcc = (1.l * queTrainAcc.de_queue()) / iTrainBatchSize;
                dRc  = (1.l * queTrainRc.de_queue()) / iTrainBatchSize;
                auto iTrnEdTP = NEUNET_CHRONO_TIME_POINT;
                print_train_progress((i + 1), iTrainBatchCnt, dAcc, dRc, (iTrnEdTP - iTrnBgTp));
            }
            // deduce
            for (auto i = 0ull; i < iDeduceBatchCnt; ++i) {
                if (iCurrNetStat == NEUNET_STAT_EXC) return;
                print_deduce_progress((i + 1), iDeduceBatchCnt);
            }
            dAcc = (1.l * queDeduceAcc.de_queue()) / iDeduceDataCnt;
            dRc  = (1.l * queDeduceRc.de_queue()) / iDeduceDataCnt;
            auto iEpEdTP = NEUNET_CHRONO_TIME_POINT;
            print_epoch_status(iEp, dAcc, dRc, iEpEdTP - iEpBgTP);
        }
        iCurrNetStat = NEUNET_STAT_END;
    }

public:
    Neunet(uint64_t iNetTrainBatchSize = 0, uint64_t iNetDeduceBatchSize = 0, long double dNetTrainAcc = 1e-8l, uint64_t iNetMaxBatchCntInit = 5) :
        iTrainBatchSize(iNetTrainBatchSize),
        iDeduceBatchSize(iNetDeduceBatchSize),
        dTrainAcc(dNetTrainAcc),
        iMaxBatchCntInit(iNetMaxBatchCntInit),
        asyPool(iTrainBatchSize > iDeduceBatchSize ? iTrainBatchSize + 1 : iDeduceBatchSize + 1) {
            // batch process size
            iRunBatchSize = asyPool.size() - 1;
            asyConcurr.set_batch_size(iRunBatchSize);
        }

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
    template<typename LayerType, typename ... Args,  typename neunet_layer_type_v> bool AddLayer(Args &&... argsLayerInit) { return seqLayer.emplace_back(std::make_shared<LayerType>(std::forward<Args>(argsLayerInit)...)); }

    uint64_t Depth() { return seqLayer.length; }

    // If the input data is the format of traditional matrix sequence for channels denoting, it needs to transform to im2col shape by function neunet::im2col_trans included in head file neunet.
    virtual bool Run(const net_set<vect> &setIm2ColTrainData, const net_set<uint64_t> &setTrainLbl, const net_set<vect> &setIm2ColTestData, const net_set<uint64_t> &setTestLbl, uint64_t iDataLblTypeCnt, uint64_t iDataLnCnt, uint64_t iDataColCnt, uint64_t iDataChannCnt) {
        if (!(setIm2ColTrainData.length == setTrainLbl.length && setIm2ColTestData.length == setTestLbl.length)) return false;
        // initialization
        if (!RunInit(setIm2ColTrainData.length, setIm2ColTestData.length, iDataLblTypeCnt, iDataLnCnt, iDataColCnt, iDataChannCnt)) return false;
        // data queue for data show
        async::net_queue<uint64_t> queTrainAcc,
                                   queTrainRc,
                                   queDeduceAcc,
                                   queDeduceRc;
        // net thread
        ThreadBatchProcess(setIm2ColTrainData, setTrainLbl, setIm2ColTestData, setTestLbl);
        ThreadTrainDeduce(queTrainAcc, queTrainRc, queDeduceAcc, queDeduceRc);
        ThreadDataShow(queTrainAcc, queTrainRc, queDeduceAcc, queDeduceRc);
        return iCurrNetStat == NEUNET_STAT_END;
    }

    virtual void Reset(bool bAllReset = true) {
        iTrainBatchSize     = 0;
        iTrainBatchCnt      = 0;
        iTrainDataCnt       = 0;
        iTrainDataIdx       = 0;
        iDeduceBatchSize    = 0;
        iDeduceBatchCnt     = 0;
        iDeduceDataCnt      = 0;
        iDeduceDataIdx      = 0;
        iLblTypeCnt         = 0;
        iRunBatchSize       = 0;
        iMaxBatchCntInit    = 0;
        iCurrNetStat        = 0;
        iCurrTrainBatchIdx  = 0;
        iCurrDeduceBatchIdx = 0;
        iTrainDeduceAccCnt  = 0;
        iTrainDeduceRcCnt   = 0;
        dTrainAcc           = 1e-8l;

        seqLayer.reset();
        setCurrData.reset();
        setCurrOrgn.reset();
        setTrainDataIdx.reset();
        setCurrLbl.reset();
        setCurrData.reset();
    }

    virtual ~Neunet() { Reset(false); }

protected:
    uint64_t iTrainBatchSize  = 0,
             iTrainBatchCnt   = 0,
             iTrainDataCnt    = 0,
             iTrainDataIdx    = 0,

             iDeduceBatchSize = 0,
             iDeduceBatchCnt  = 0,
             iDeduceDataCnt   = 0,
             iDeduceDataIdx   = 0,

             iLblTypeCnt      = 0,
             iMaxBatchCntInit = 0,
             iRunBatchSize    = 0;

    std::atomic_uint64_t iCurrNetStat        = 0,
                         iCurrTrainBatchIdx  = 0,
                         iCurrDeduceBatchIdx = 0,
                         iTrainDeduceAccCnt  = 0,
                         iTrainDeduceRcCnt   = 0;

    long double dTrainAcc = 1e-8l;

    net_sequence<NetLayerPtr> seqLayer;

    // data
    net_set<vect> setCurrData,
                  // label vector
                  setCurrOrgn;
    
    // data index
    net_set<uint64_t> setTrainDataIdx,
                      // label
                      setCurrLbl;

    async::async_concurrent asyConcurr;

    async::async_controller asyTrainCtrl,
                            asyDeduceCtrl;

    async::async_pool asyPool;
};

NEUNET_END
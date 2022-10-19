NEUNET_BEGIN

class Neunet {
protected:
    virtual void BatchTrainDataInit(const net_set<vect> &setTrainData, const net_set<uint64_t> &setTrainLbl) {
        if (setCurrData.length != iTrainBatchSize) setCurrData.init(iTrainBatchSize, false);
        if (setCurrLbl.length != iTrainBatchSize) setCurrLbl.init(iTrainBatchSize, false);
        for (auto i = 0ull; i < iTrainBatchSize; ++i, ++iTrainDataIdx) {
            setCurrData[i] = setTrainData[setTrainDataIdx[iTrainDataIdx]];
            setCurrLbl[i]  = setTrainLbl[setTrainDataIdx[iTrainDataIdx]];
        }
        setCurrOrgn = lbl_orgn(setCurrLbl, iLblTypeCnt);
        if (iTrainDataIdx == setTrainData.length) {
            iTrainDataIdx = 0;
            setTrainDataIdx.shuffle();
        }
    }

    virtual void BatchDeduceDataInit(const net_set<vect> &setDeduceData, const net_set<uint64_t> &setDeduceLbl) {
        if (setCurrData.length != iDeduceBatchSize) setCurrData.init(iDeduceBatchSize, false);
        if (setCurrLbl.length != iDeduceBatchSize) setCurrLbl.init(iDeduceBatchSize, false);
        for (auto i = 0ull; i < iDeduceBatchSize; ++i, ++iDeduceDataIdx) {
            setCurrData[i] = setDeduceData[iDeduceDataIdx];
            setCurrLbl[i]  = setDeduceLbl[iDeduceDataIdx];
        }
        if (iDeduceDataIdx == setDeduceData.length) iDeduceDataIdx = 0;
    }

    virtual bool RunInit(uint64_t iNetTrainDataCnt, uint64_t iNetDeduceDataCnt, uint64_t iDataLblTypeCnt, uint64_t iInputLnCnt, uint64_t iInputColCnt, uint64_t iChannCnt) {
        iLblTypeCnt = iDataLblTypeCnt;
        // data initialize
        if (iTrainDataCnt % iTrainBatchSize) return false;
        iTrainDataCnt  = iNetTrainDataCnt;
        iTrainBatchCnt = iTrainDataCnt / iTrainBatchSize;
        iTrainDataIdx  = 0;
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
        setCurrTrainOutput[iIdx] = setCurrData[iIdx];
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
        return true;
    }

    virtual bool ThreadTrainDeduceExitCheck() {
        if (iCurrNetStat == NEUNET_STAT_EXC || iCurrNetStat == NEUNET_STAT_END) {
            asyConcurr.main_thread_exception();
            return true;
        } else return false;
    }

    // thread size - batch size
    virtual void ThreadBatchProcess() { for (auto i = 0ull; i < iRunBatchSize; ++i) asyPool.add_task([this, i] { while (true) {
        // batch process mode
        asyConcurr.batch_thread_attach();
        if (iCurrNetStat == NEUNET_STAT_END || iCurrNetStat == NEUNET_STAT_EXC) break;
        // train
        if (iCurrNetStat == NEUNET_STAT_TRN && i < iTrainBatchSize) if (!(ForwProp(i) && BackProp(i))) iCurrNetStat = NEUNET_STAT_EXC;
        // deduce
        if (iCurrNetStat == NEUNET_STAT_DED && i < iDeduceBatchSize) if (!Deduce(i)) iCurrNetStat = NEUNET_STAT_EXC;
        asyConcurr.batch_thread_detach();
    } }); }

    virtual void ThreadTrainDeduce(const net_set<vect> &setTrainData, const net_set<uint64_t> &setTrainLbl, const net_set<vect> &setDeduceData, const net_set<uint64_t> &setDeduceLbl, async::net_queue<net_set<vect>> &queTrainOutput, async::net_queue<net_set<uint64_t>> &queTrainLbl, async::net_queue<net_set<vect>> &queDeduceOutput, async::net_queue<net_set<uint64_t>> &queDeduceLbl) { asyPool.add_task([this, &setTrainData, &setTrainLbl, &setDeduceData, &setDeduceLbl, &queTrainOutput, &queTrainLbl, &queDeduceOutput, &queDeduceLbl] { while (true) {
        // train
        iCurrNetStat = NEUNET_STAT_TRN;
        for (iCurrTrainBatchIdx = 0; iCurrTrainBatchIdx < iTrainBatchCnt; ++iCurrTrainBatchIdx) {
            // load data;
            BatchTrainDataInit(setTrainData, setTrainLbl);
            setCurrTrainOutput.init(iTrainBatchSize);
            asyConcurr.main_thread_deploy_batch_thread();
            if (iCurrNetStat == NEUNET_STAT_EXC) break;
            // output data
            queTrainOutput.en_queue(std::move(setCurrTrainOutput));
            queTrainLbl.en_queue(std::move(setCurrLbl));
        }
        if (ThreadTrainDeduceExitCheck()) break;
        // deduce
        iCurrNetStat = NEUNET_STAT_DED;
        for (iCurrDeduceBatchIdx = 0; iCurrDeduceBatchIdx < iDeduceBatchCnt; ++iCurrDeduceBatchIdx) {
            // load data
            BatchDeduceDataInit(setDeduceData, setDeduceLbl);
            asyConcurr.main_thread_deploy_batch_thread();
            if (iCurrNetStat == NEUNET_STAT_EXC) break;
            // output data
            queDeduceOutput.en_queue(std::move(setCurrData));
            queDeduceLbl.en_queue(std::move(setCurrLbl));
        }
        if (ThreadTrainDeduceExitCheck()) break;
    } }); }

    virtual void ThreadDataShow(async::net_queue<net_set<vect>> &queTrainOutput, async::net_queue<net_set<uint64_t>> &queTrainLbl, async::net_queue<net_set<vect>> &queDeduceOutput, async::net_queue<net_set<uint64_t>> &queDeduceLbl) {
        // train & deduce parameter
                    // Accuracy
        long double dAcc  = .0l,
                    // Precision
                    dPrec = .0l,
                    // Recall rate
                    dRc   = .0l;
        // epoch
        uint64_t iEp = 0ull;
        while (dRc < 1) {
            auto iEpBgTP = NEUNET_CHRONO_TIME_POINT;
            ++iEp;
            // train
            for (auto i = 0ull; i < iTrainBatchCnt; ++i) {
                auto iTrnBgTp = NEUNET_CHRONO_TIME_POINT;
                dAcc  = .0l;
                dPrec = .0l;
                dRc   = .0l;
                if (iCurrNetStat == NEUNET_STAT_EXC) return;
                auto setCurrOutput = queTrainOutput.de_queue();
                auto setCurrLbl    = queTrainLbl.de_queue();
                output_acc_prec_rc(setCurrOutput, setCurrLbl, dTrainAcc, dAcc, dPrec, dRc, iTrainBatchSize);
                auto iTrnEdTP = NEUNET_CHRONO_TIME_POINT;
                print_train_progress((i + 1), iTrainBatchCnt, dAcc, dPrec, dRc, (iTrnEdTP - iTrnBgTp));
            }
            // deduce
            dAcc  = .0l;
            dPrec = .0l;
            dRc   = .0l;
            for (auto i = 0ull; i < iDeduceBatchCnt; ++i) {
                if (iCurrNetStat == NEUNET_STAT_EXC) return;
                auto setCurrOutput = queDeduceOutput.de_queue();
                auto setCurrLbl    = queDeduceLbl.de_queue();
                output_acc_prec_rc(setCurrOutput, setCurrLbl, dTrainAcc, dAcc, dPrec, dRc);
                print_deduce_progress((i + 1), iDeduceBatchCnt);
            }
            dAcc  /= iDeduceDataCnt;
            dPrec /= iDeduceDataCnt;
            dRc   /= iDeduceDataCnt;
            auto iEpEdTP = NEUNET_CHRONO_TIME_POINT;
            print_epoch_status(iEp, dAcc, dPrec, dRc, iEpEdTP - iEpBgTP);
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
        async::net_queue<net_set<vect>> queTrainOutput,
                                        queDeduceOutput;
        async::net_queue<net_set<uint64_t>> queTrainLbl,
                                            queDeduceLbl;
        // net thread
        ThreadBatchProcess();
        ThreadTrainDeduce(setIm2ColTrainData, setTrainLbl, setIm2ColTestData, setTestLbl, queTrainOutput, queTrainLbl, queDeduceOutput, queDeduceLbl);
        ThreadDataShow(queTrainOutput, queTrainLbl, queDeduceOutput, queDeduceLbl);
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
        dTrainAcc           = 1e-8l;

        seqLayer.reset();
        setCurrData.reset();
        setCurrOrgn.reset();
        setTrainDataIdx.reset();
        setCurrLbl.reset();
        setCurrData.reset();
        setCurrTrainOutput.reset();
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
                         iCurrDeduceBatchIdx = 0;

    long double dTrainAcc = 1e-8l;

    net_sequence<NetLayerPtr> seqLayer;
    // data
    net_set<vect> setCurrData,
                  // label vector
                  setCurrOrgn,
                  // train output
                  setCurrTrainOutput;
    
    // data index
    net_set<uint64_t> setTrainDataIdx,
                      // label
                      setCurrLbl;

    async::net_async_concurrent asyConcurr;

    async::net_async_controller asyTrainCtrl,
                                asyDeduceCtrl;

    async::net_async_pool asyPool;
};

NEUNET_END
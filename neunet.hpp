NEUNET_BEGIN

class Neunet {
protected:
    callback_matrix void RunInit(uint64_t iInputLnCnt, uint64_t iInputColCnt, uint64_t iChannCnt) {
        for (auto i = 0ull; i < seqLayer.length; ++i) {
            switch (seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_ACT: std::dynamic_pointer_cast<layer::LayerAct<matrix_elem_t>>(seqLayer[i])->RunInit(iBatchSize); break;
            case NEUNET_LAYER_PC: {
                auto pCurrLayer = std::dynamic_pointer_cast<layer::LayerPC>(seqLayer[i]);
                pCurrLayer->RunInit(iInputLnCnt, iInputColCnt);
                iInputLnCnt = pCurrLayer->iOutputLnCnt;
                iInputColCnt = pCurrLayer->iOutputColCnt;
            } break;
            case NEUNET_LAYER_TRANS: {
                auto pCurrLayer = std::dynamic_pointer_cast<layer::LayerTrans>(seqLayer[i]);
                pCurrLayer->RunInit(iInputLnCnt, iInputColCnt, iChannCnt);
                iInputLnCnt  = iInputLnCnt * iInputColCnt * iChannCnt;
                iInputColCnt = 1;
                iChannCnt    = 1;
            } break;
            case NEUNET_LAYER_FC: {
                auto pCurrLayer = std::dynamic_pointer_cast<layer::LayerFC<matrix_elem_t>>(seqLayer[i]);
                pCurrLayer->RunInit(iInputLnCnt, iBatchSize);
                iInputLnCnt = pCurrLayer->iOutputLnCnt;
            } break;
            case NEUNET_LAYER_CONV: {
                auto pCurrLayer = std::dynamic_pointer_cast<layer::LayerConv<matrix_elem_t>>(seqLayer[i]);
                pCurrLayer->RunInit(iInputLnCnt, iInputColCnt, iChannCnt, iBatchSize);
                iInputLnCnt  = pCurrLayer->iOutputLnCnt;
                iInputColCnt = pCurrLayer->iOutputColCnt;
                iChannCnt    = pCurrLayer->iKernelAmt;
            } break;
            case NEUNET_LAYER_POOL: {
                auto pCurrLayer = std::dynamic_pointer_cast<layer::LayerPool>(seqLayer[i]);
                pCurrLayer->RunInit(iInputLnCnt, iInputColCnt, iBatchSize);
                iInputLnCnt  = pCurrLayer->iOutputLnCnt;
                iInputColCnt = pCurrLayer->iOutputColCnt;
            } break;
            case NEUNET_LAYER_BN: std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(seqLayer[i])->RunInit(iChannCnt); break;
            default: break;
            }
        }
    }

public:
    Neunet(uint64_t iNetBatchSize = 0, long double dNetAcc = 1e-8) :
        iBatchSize(iNetBatchSize),
        dAcc(dNetAcc) {}

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
    template<typename LayerType, typename ... Args,  typename = std::enable_if_t<std::is_base_of_v<layer::Layer, LayerType>>> bool AddLayer(Args &&... argsLayerInit) { return seqLayer.emplace_back(std::make_shared<LayerType>(std::forward<Args>(argsLayerInit)...)); }

    uint64_t Depth() const { return seqLayer.length; }

    virtual void Reset(bool bFull = true) {
        iBatchSize = 0;
        dAcc       = 1e-8;
        seqLayer.reset();
    }

    virtual ~Neunet() { Reset(false); }

protected:
    uint64_t iBatchSize = 0;

    long double dAcc = 1e-8;

    net_sequence<NetLayerPtr> seqLayer;
};

matrix_declare struct NetOutput final {
    net_set<neunet_vect> setOutput;
    net_set<uint64_t>    setLblOrgn;
};

class NeunetMNIST final : public Neunet {
public:
    NeunetMNIST(uint64_t iNetBatchSize = 0, long double dNetAcc = 1e-8) : Neunet(iNetBatchSize, dNetAcc),
        asyConcurr(iNetBatchSize),
        asyPool(iNetBatchSize + 1) {}

    callback_matrix bool ForwProp(net_set<neunet_vect> &setInput, uint64_t iIdx) {
        for (auto i = 0ull; i < this->seqLayer.length; ++i) {
            auto bFlag = true;
            switch (this->seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_ACT: bFlag = std::dynamic_pointer_cast<layer::LayerAct<matrix_elem_t>>(this->seqLayer[i])->ForwProp(setInput[iIdx], iIdx); break;
            case NEUNET_LAYER_PC: bFlag = std::dynamic_pointer_cast<layer::LayerPC>(this->seqLayer[i])->ForwProp(setInput[iIdx]); break;
            case NEUNET_LAYER_TRANS: bFlag = std::dynamic_pointer_cast<layer::LayerTrans>(this->seqLayer[i])->ForwProp(setInput[iIdx]); break;
            case NEUNET_LAYER_FC: bFlag = std::dynamic_pointer_cast<layer::LayerFC<matrix_elem_t>>(this->seqLayer[i])->ForwProp(setInput[iIdx], iIdx); break;
            case NEUNET_LAYER_CONV: bFlag = std::dynamic_pointer_cast<layer::LayerConv<matrix_elem_t>>(this->seqLayer[i])->ForwProp(setInput[iIdx], iIdx); break;
            case NEUNET_LAYER_POOL: bFlag = std::dynamic_pointer_cast<layer::LayerPool>(this->seqLayer[i])->ForwProp(setInput[iIdx], iIdx); break;
            case NEUNET_LAYER_BN:
                asyConcurr.batch_thread_detach([this, i]{
                    iCurrLayerIdx = i;
                    iAsyncTsk     = NEUNET_TSK_BN_FP;
                });
                asyConcurr.batch_thread_attach(); break;
            default: bFlag = false;
            }
            if (!bFlag) return false;
        }
        return true;
    }

    callback_matrix bool BackProp(net_set<neunet_vect> &setGrad, const net_set<neunet_vect> &setOrgn, uint64_t iIdx) {
        for (auto i = this->seqLayer.length; i; --i) {
            auto bFlag   = true;
            auto iLyrIdx = i - 1;
            switch (this->seqLayer[iLyrIdx]->iLayerType) {
            case NEUNET_LAYER_ACT: bFlag = std::dynamic_pointer_cast<layer::LayerAct<matrix_elem_t>>(this->seqLayer[iLyrIdx])->BackProp(setGrad[iIdx], setOrgn[iIdx], iIdx); break;
            case NEUNET_LAYER_PC: bFlag = std::dynamic_pointer_cast<layer::LayerPC>(this->seqLayer[iLyrIdx])->BackProp(setGrad[iIdx]); break;
            case NEUNET_LAYER_TRANS: bFlag = std::dynamic_pointer_cast<layer::LayerTrans>(this->seqLayer[iLyrIdx])->BackProp(setGrad[iIdx]); break;
            case NEUNET_LAYER_FC: bFlag = std::dynamic_pointer_cast<layer::LayerFC<matrix_elem_t>>(this->seqLayer[iLyrIdx])->BackProp(setGrad[iIdx], iIdx); break;
            case NEUNET_LAYER_CONV: bFlag = std::dynamic_pointer_cast<layer::LayerConv<matrix_elem_t>>(this->seqLayer[iLyrIdx])->BackProp(setGrad[iIdx], iIdx); break;
            case NEUNET_LAYER_POOL: bFlag = std::dynamic_pointer_cast<layer::LayerPool>(this->seqLayer[iLyrIdx])->BackProp(setGrad[iIdx], iIdx); break;
            case NEUNET_LAYER_BN:
                asyConcurr.batch_thread_detach([this, iLyrIdx]{
                    iCurrLayerIdx = iLyrIdx;
                    iAsyncTsk     = NEUNET_TSK_BN_BP;
                });
                asyConcurr.batch_thread_attach();
                break;
            default: bFlag = false;
            }
            if (!bFlag) return false;
        }
        return true;
    }

    callback_matrix void Update(uint64_t iCurrBatch, uint64_t iBatchCnt) {
        for (auto i = 0ull; i < this->seqLayer.length; ++i) {
            switch (this->seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_FC: std::dynamic_pointer_cast<layer::LayerFC<matrix_elem_t>>(this->seqLayer[i])->Update(); break;
            case NEUNET_LAYER_CONV: std::dynamic_pointer_cast<layer::LayerConv<matrix_elem_t>>(this->seqLayer[i])->Update(); break;
            case NEUNET_LAYER_BN: std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[i])->Update(iCurrBatch + 1 == iBatchCnt, iBatchCnt, this->iBatchSize); break;
            default: continue;
            }
        }
    }

    callback_matrix bool Deduce(neunet_vect &vecInput) {
        for (auto i = 0ull; i < this->seqLayer.length; ++i) {
            auto bFlag = true;
            switch (this->seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_ACT: bFlag = std::dynamic_pointer_cast<layer::LayerAct<matrix_elem_t>>(this->seqLayer[i])->Deduce(vecInput); break;
            case NEUNET_LAYER_PC: bFlag = std::dynamic_pointer_cast<layer::LayerPC>(this->seqLayer[i])->Deduce(vecInput); break;
            case NEUNET_LAYER_TRANS: bFlag = std::dynamic_pointer_cast<layer::LayerTrans>(this->seqLayer[i])->Deduce(vecInput); break;
            case NEUNET_LAYER_FC: bFlag = std::dynamic_pointer_cast<layer::LayerFC<matrix_elem_t>>(this->seqLayer[i])->Deduce(vecInput); break;
            case NEUNET_LAYER_CONV: bFlag = std::dynamic_pointer_cast<layer::LayerConv<matrix_elem_t>>(this->seqLayer[i])->Deduce(vecInput); break;
            case NEUNET_LAYER_POOL: bFlag = std::dynamic_pointer_cast<layer::LayerPool>(this->seqLayer[i])->Deduce(vecInput); break;
            case NEUNET_LAYER_BN: std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[i])->Deduce(vecInput); break;
            default: bFlag = false; break;
            }
            if (!bFlag) return false;
        }
        return true;
    }

    callback_matrix bool Run(dataset::mnist<matrix_elem_t> &dsTrainSet, dataset::mnist<matrix_elem_t> &dsTestSet) {
        // Data preparation
        async::net_async_digit<bool> bTrainMode = true,
                                     bComplete  = false,
                                     bException = false;

        async::net_async_digit<uint64_t> iTrainCnt  = 0,
                                         iDeduceCnt = 0;

        dsTrainSet.init_batch(this->iBatchSize);
        dsTestSet.init_batch(this->iBatchSize);

        RunInit<matrix_elem_t>(dsTrainSet.element_line_count, dsTrainSet.element_column_count, 1);

        // Batch
        NetOutput<matrix_elem_t> netCurrTrainOutput,
                                 netCurrDeduceOutput;
        
        for (auto i = 0ull; i < this->iBatchSize; ++i) asyPool.add_task([this, &netCurrTrainOutput, &netCurrDeduceOutput, &bTrainMode, &bComplete, &bException, &dsTrainSet, &dsTestSet, &iTrainCnt, &iDeduceCnt] (uint64_t idx) { while (true) {
            asyConcurr.batch_thread_attach();
            if (bComplete || bException) break;
            if (bTrainMode) {
                if (ForwProp(dsTrainSet.curr_batch_elem, idx)) {
                    netCurrTrainOutput.setOutput[idx] = dsTrainSet.curr_batch_elem[idx];
                    if (BackProp(dsTrainSet.curr_batch_elem, dsTrainSet.curr_batch_orgn, idx)) ++iTrainCnt;
                }
            } else if (Deduce(dsTestSet.curr_batch_elem[idx])) {
                netCurrDeduceOutput.setOutput[idx] = std::move(dsTestSet.curr_batch_elem[idx]);
                ++iDeduceCnt;
            }
            asyConcurr.batch_thread_detach([this]{ iAsyncTsk = NEUNET_TSK_UPDATE; });
        } }, i);
        
        // Main
        async::net_queue<NetOutput<matrix_elem_t>> queTrainOutput,
                                                   queTestOutput;

        asyPool.add_task([this, &netCurrTrainOutput, &netCurrDeduceOutput, &bTrainMode, &bComplete, &bException, &dsTrainSet, &dsTestSet, &iTrainCnt, &iDeduceCnt, &queTrainOutput, &queTestOutput]{ while(!bComplete) {
            dsTrainSet.shuffle();
            // Train
            bTrainMode = true;
            for (auto i = 0ull; i < dsTrainSet.batch_cnt; ++i) {
                dsTrainSet.init_curr_batch(i);
                iTrainCnt = 0;
                netCurrTrainOutput.setLblOrgn.init(this->iBatchSize);
                netCurrTrainOutput.setOutput.init(this->iBatchSize);
                do {
                    asyConcurr.main_thread_deploy_batch_thread();
                    switch (iAsyncTsk) {
                    case NEUNET_TSK_BN_FP: bException = !std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[iCurrLayerIdx])->ForwProp(dsTrainSet.curr_batch_elem); break;
                    case NEUNET_TSK_BN_BP: bException = !std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[iCurrLayerIdx])->BackProp(dsTrainSet.curr_batch_elem); break;
                    case NEUNET_TSK_UPDATE: Update<matrix_elem_t>(i, dsTrainSet.batch_cnt); break;
                    default: break;
                    }
                    if (bException) break;
                } while(iAsyncTsk != NEUNET_TSK_UPDATE);
                // Transfer data
                if (!bException && iTrainCnt == this->iBatchSize || iTrainCnt == dsTrainSet.rear_batch_size) {
                    netCurrTrainOutput.setLblOrgn = std::move(dsTrainSet.curr_batch_lbl);
                    queTrainOutput.en_queue(std::move(netCurrTrainOutput));
                    asyControl.thread_wake_one();
                } else {
                    asyConcurr.main_thread_exception();
                    break;
                }
            }
            if (bException) break;
            // Deduce
            bTrainMode = false;            
            for (auto i = 0ull; i < dsTestSet.batch_cnt; ++i) {
                dsTestSet.init_curr_batch(i);
                iDeduceCnt = 0;
                netCurrDeduceOutput.setOutput.init(this->iBatchSize);
                asyConcurr.main_thread_deploy_batch_thread();
                if (iDeduceCnt == this->iBatchSize || iDeduceCnt == dsTestSet.rear_batch_size) {
                    netCurrDeduceOutput.setLblOrgn = std::move(dsTestSet.curr_batch_lbl);
                    queTestOutput.en_queue(std::move(netCurrDeduceOutput));
                    asyControl.thread_wake_one();
                } else {
                    bException = true;
                    asyConcurr.main_thread_exception();
                    break;
                }
            }
        } });

        // Data show
        long double dCurrAcc  = 0,
                    dPrec     = 0,
                    dRc       = 0;
        
        uint64_t iEp = 0;
        while (!bComplete) {
            auto iEpBegin = NEUNET_CHRONO_TIME_POINT;
            ++iEp;
            // Train
            for (auto i = 0ull; i < dsTrainSet.batch_cnt; ++i) {
                auto iTrainBegin = NEUNET_CHRONO_TIME_POINT;
                dAcc  = 0;
                dPrec = 0;
                dRc   = 0;
                if (queTrainOutput.length == 0) asyControl.thread_sleep();
                if (bException) return false;
                auto netCurrTrainRs = queTrainOutput.de_queue();
                deduce_acc_prec_rc(dCurrAcc, dPrec, dRc, netCurrTrainRs.setOutput, netCurrTrainRs.setLblOrgn, dAcc);
                auto iTrainEnd = NEUNET_CHRONO_TIME_POINT;
                print_train_status(iEp, i + 1, dsTrainSet.batch_cnt, dAcc, dPrec, dRc, iTrainEnd - iTrainBegin);
            }
            // Deduce
            dAcc  = 0;
            dPrec = 0;
            dRc   = 0;
            for (auto i = 0ull; i < dsTestSet.batch_cnt; ++i) {
                if (queTestOutput.length == 0) asyControl.thread_sleep();
                if (bException) return false;
                auto netCurrTestRs = queTestOutput.de_queue();
                deduce_acc_prec_rc(dCurrAcc, dPrec, dRc, netCurrTestRs.setOutput, netCurrTestRs.setLblOrgn, dAcc, false);
            }
            // Summary
            dAcc  /= dsTestSet.element_count;
            dPrec /= dsTestSet.element_count;
            dRc   /= dsTestSet.element_count;
            auto iEpEnd = NEUNET_CHRONO_TIME_POINT;
            print_deduce_status(iEp, dAcc, dPrec, dRc, iEpEnd - iEpBegin);
            bComplete = (dRc == 1);
        }
        return true;
    }

private:
    uint64_t iCurrLayerIdx = 0,
             iAsyncTsk     = NEUNET_TSK_UPDATE;
    
    async::net_async_concurrent asyConcurr;
    async::net_async_controller asyControl;
    async::net_async_pool       asyPool;

};

NEUNET_END
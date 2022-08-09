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
                iInputLnCnt  = pCurrLayer->iOutputLnCnt;
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

    callback_matrix bool ForwProp(net_set<neunet_vect> &setInput) {
        for (auto i = 0ull; i < this->seqLayer.length; ++i) {
            auto bFlag = true;
            if (this->seqLayer[i]->iLayerType == NEUNET_LAYER_BN) bFlag = std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[i])->ForwProp(setInput);
            else for (auto j = 0ull; j < setInput.length; ++j) switch (this->seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_ACT: bFlag = std::dynamic_pointer_cast<layer::LayerAct<matrix_elem_t>>(this->seqLayer[i])->ForwProp(setInput[j], j); break;
            case NEUNET_LAYER_PC: bFlag = std::dynamic_pointer_cast<layer::LayerPC>(this->seqLayer[i])->ForwProp(setInput[j]); break;
            case NEUNET_LAYER_TRANS: bFlag = std::dynamic_pointer_cast<layer::LayerTrans>(this->seqLayer[i])->ForwProp(setInput[j]); break;
            case NEUNET_LAYER_FC: bFlag = std::dynamic_pointer_cast<layer::LayerFC<matrix_elem_t>>(this->seqLayer[i])->ForwProp(setInput[j], j); break;
            case NEUNET_LAYER_CONV: bFlag = std::dynamic_pointer_cast<layer::LayerConv<matrix_elem_t>>(this->seqLayer[i])->ForwProp(setInput[j], j); break;
            case NEUNET_LAYER_POOL: bFlag = std::dynamic_pointer_cast<layer::LayerPool>(this->seqLayer[i])->ForwProp(setInput[j], j); break;
            default: bFlag = false;
            }
            if (!bFlag) return false;
        }
        return true;
    }

    callback_matrix bool BackProp(net_set<neunet_vect> &setGrad, const net_set<neunet_vect> &setOrgn) {
        for (auto i = this->seqLayer.length; i; --i) {
            auto bFlag   = true;
            auto iLyrIdx = i - 1;
            if (this->seqLayer[iLyrIdx]->iLayerType == NEUNET_LAYER_BN) bFlag = std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[iLyrIdx])->BackProp(setGrad);
            else for (auto j = 0ull; j < setGrad.length; ++j) switch (this->seqLayer[iLyrIdx]->iLayerType) {
            case NEUNET_LAYER_ACT: bFlag = std::dynamic_pointer_cast<layer::LayerAct<matrix_elem_t>>(this->seqLayer[iLyrIdx])->BackProp(setGrad[j], setOrgn[j], j); break;
            case NEUNET_LAYER_PC: bFlag = std::dynamic_pointer_cast<layer::LayerPC>(this->seqLayer[iLyrIdx])->BackProp(setGrad[j]); break;
            case NEUNET_LAYER_TRANS: bFlag = std::dynamic_pointer_cast<layer::LayerTrans>(this->seqLayer[iLyrIdx])->BackProp(setGrad[j]); break;
            case NEUNET_LAYER_FC: bFlag = std::dynamic_pointer_cast<layer::LayerFC<matrix_elem_t>>(this->seqLayer[iLyrIdx])->BackProp(setGrad[j], j); break;
            case NEUNET_LAYER_CONV: bFlag = std::dynamic_pointer_cast<layer::LayerConv<matrix_elem_t>>(this->seqLayer[iLyrIdx])->BackProp(setGrad[j], j); break;
            case NEUNET_LAYER_POOL: bFlag = std::dynamic_pointer_cast<layer::LayerPool>(this->seqLayer[iLyrIdx])->BackProp(setGrad[j], j); break;
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
            case NEUNET_LAYER_BN: std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[i])->Update(iCurrBatch, iBatchCnt, this->iBatchSize); break;
            default: continue;
            }
        }
    }

    callback_matrix bool Deduce(net_set<neunet_vect> &setInput, uint64_t iIdx) {
        for (auto i = 0ull; i < this->seqLayer.length; ++i) {
            auto bFlag = true;
            switch (this->seqLayer[i]->iLayerType) {
            case NEUNET_LAYER_ACT: bFlag = std::dynamic_pointer_cast<layer::LayerAct<matrix_elem_t>>(this->seqLayer[i])->Deduce(setInput[iIdx]); break;
            case NEUNET_LAYER_PC: bFlag = std::dynamic_pointer_cast<layer::LayerPC>(this->seqLayer[i])->Deduce(setInput[iIdx]); break;
            case NEUNET_LAYER_TRANS: bFlag = std::dynamic_pointer_cast<layer::LayerTrans>(this->seqLayer[i])->Deduce(setInput[iIdx]); break;
            case NEUNET_LAYER_FC: bFlag = std::dynamic_pointer_cast<layer::LayerFC<matrix_elem_t>>(this->seqLayer[i])->Deduce(setInput[iIdx]); break;
            case NEUNET_LAYER_CONV: bFlag = std::dynamic_pointer_cast<layer::LayerConv<matrix_elem_t>>(this->seqLayer[i])->Deduce(setInput[iIdx]); break;
            case NEUNET_LAYER_POOL: bFlag = std::dynamic_pointer_cast<layer::LayerPool>(this->seqLayer[i])->Deduce(setInput[iIdx]); break;
            case NEUNET_LAYER_BN: std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[i])->Deduce(setInput[iIdx]); break;
            default: bFlag = false; break;
            }
            if (!bFlag) return false;
        }
        return true;
    }

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
                    iAsyncStat    = NEUNET_STAT_BNF;
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
                    iAsyncStat    = NEUNET_STAT_BNB;
                });
                asyConcurr.batch_thread_attach();
                break;
            default: bFlag = false;
            }
            if (!bFlag) return false;
        }
        return true;
    }

    callback_matrix bool Run(dataset::mnist<matrix_elem_t> &dsTrainSet, dataset::mnist<matrix_elem_t> &dsTestSet) {
        // batch process
        net_set<neunet_vect> setBatchOutput;
        for (auto i = 0ull; i < this->iBatchSize; ++i) asyPool.add_task([this, &dsTrainSet, &dsTestSet, &setBatchOutput](uint64_t idx) { while (true) {
            // attach thread
            asyConcurr.batch_thread_attach();
            if (iAsyncStat == NEUNET_STAT_END || iAsyncStat == NEUNET_STAT_EXC) break;
            // train
            else if (iAsyncStat == NEUNET_STAT_TRN && ForwProp(dsTrainSet.curr_batch_elem, idx)) {
                setBatchOutput[idx] = dsTrainSet.curr_batch_elem[idx];
                if (BackProp(dsTrainSet.curr_batch_elem, dsTrainSet.curr_batch_orgn, idx)) {
                    // detach thread, update
                    asyConcurr.batch_thread_detach([this]{ iAsyncStat = NEUNET_STAT_UPT; });
                }
            }
            // deduce
            else if (iAsyncStat == NEUNET_STAT_DED && this->Deduce(dsTestSet.curr_batch_elem, idx)) {
                setBatchOutput[idx] = dsTestSet.curr_batch_elem[idx];
                asyConcurr.batch_thread_detach();
            }
            // exception
            else asyConcurr.batch_thread_detach([this]{ iAsyncStat = NEUNET_STAT_EXC; });
        } }, i);

        // control process
        async::net_queue<net_set<neunet_vect>> queTrainOutput,
                                               queTestOutput;
        async::net_queue<net_set<uint64_t>> queTrainLbl,
                                            queTestLbl;
        // dataset initialization
        dsTrainSet.init_batch(this->iBatchSize);
        dsTestSet.init_batch(this->iBatchSize);
        this->RunInit<matrix_elem_t>(dsTrainSet.element_line_count, dsTrainSet.element_column_count, 1);
        // task
        asyPool.add_task([this, &dsTrainSet, &dsTestSet, &setBatchOutput, &queTrainOutput, &queTestOutput, &queTrainLbl, &queTestLbl]{ do {
            dsTrainSet.shuffle();
            // train, batch
            for (auto i = 0ull; i < dsTrainSet.batch_cnt; ++i) {
                dsTrainSet.init_curr_batch(i);
                setBatchOutput.init(this->iBatchSize, false);
                auto bWaitStat = true;
                do {
                    iAsyncStat = NEUNET_STAT_TRN;
                    asyConcurr.main_thread_deploy_batch_thread();
                    switch (iAsyncStat) {
                    case NEUNET_STAT_UPT:
                        this->Update<matrix_elem_t>(i, dsTrainSet.batch_cnt);
                        bWaitStat = false;
                        break;
                    case NEUNET_STAT_BNF:
                        bWaitStat = std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[iCurrLayerIdx])->ForwProp(dsTrainSet.curr_batch_elem);
                        if (!bWaitStat) iAsyncStat = NEUNET_STAT_EXC;
                        break;
                    case NEUNET_STAT_BNB:
                        bWaitStat = std::dynamic_pointer_cast<layer::LayerBN<matrix_elem_t>>(this->seqLayer[iCurrLayerIdx])->BackProp(dsTrainSet.curr_batch_elem);
                        if (!bWaitStat) iAsyncStat = NEUNET_STAT_EXC;
                        break;
                    default:
                        bWaitStat = false;
                        break;
                    }
                } while (bWaitStat);
                // package train output
                if (iAsyncStat == NEUNET_STAT_EXC) {
                    // exception
                    asyControl.thread_wake_one();
                    asyConcurr.main_thread_exception();
                    break;
                }
                queTrainOutput.en_queue(std::move(setBatchOutput));
                queTrainLbl.en_queue(std::move(dsTrainSet.curr_batch_lbl));
                asyControl.thread_wake_one();
            }
            // deduce
            iAsyncStat = NEUNET_STAT_DED;
            for (auto i = 0ull; i < dsTestSet.batch_cnt; ++i) {
                dsTestSet.init_curr_batch(i);
                setBatchOutput.init(this->iBatchSize, false);
                asyConcurr.main_thread_deploy_batch_thread();
                if (iAsyncStat == NEUNET_STAT_EXC) {
                    // exception
                    asyControl.thread_wake_one();
                    asyConcurr.main_thread_exception();
                    break;
                }
                queTestOutput.en_queue(std::move(setBatchOutput));
                queTestLbl.en_queue(std::move(dsTestSet.curr_batch_lbl));
                asyControl.thread_wake_one();
            }
        } while (!(iAsyncStat == NEUNET_STAT_END || iAsyncStat == NEUNET_STAT_EXC)); });

        // data show
        // parameter
        long double dHAcc = 0,
                    dPrec = 0,
                    dRc   = 0;
        // epoch
        uint64_t iEp = 0;
        do {
            auto iEpBegin = NEUNET_CHRONO_TIME_POINT;
            ++iEp;
            // train
            for (auto i = 0ull; i < dsTrainSet.batch_cnt; ++i) {
                auto iTrBegin = NEUNET_CHRONO_TIME_POINT;
                if (!(queTrainLbl.length && queTrainOutput.length)) asyControl.thread_sleep();
                if (iAsyncStat == NEUNET_STAT_EXC) return false;
                auto setCurrOutput = queTrainOutput.de_queue();
                auto setCurrLbl    = queTrainLbl.de_queue();
                dHAcc = 0;
                dPrec = 0;
                dRc   = 0;
                dataset::mnist<matrix_elem_t>::output_para(setCurrOutput, setCurrLbl, this->dAcc, dHAcc, dPrec, dRc, true, this->iBatchSize);
                auto iTrEnd = NEUNET_CHRONO_TIME_POINT;
                print_train_status(iEp, i + 1, dsTrainSet.batch_cnt, dHAcc, dPrec, dRc, iTrEnd - iTrBegin);
            }
            // deduce
            dHAcc = 0;
            dPrec = 0;
            dRc   = 0;
            for (auto i = 0ull; i < dsTestSet.batch_cnt; ++i) {
                if (!(queTestLbl.length && queTestOutput.length)) asyControl.thread_sleep();
                if (iAsyncStat == NEUNET_STAT_EXC) return false;
                auto setCurrOutput = queTestOutput.de_queue();
                auto setCurrLbl    = queTestLbl.de_queue();
                dataset::mnist<matrix_elem_t>::output_para(setCurrOutput, setCurrLbl, this->dAcc, dHAcc, dPrec, dRc);
                print_deduce_progress(i + 1, dsTestSet.batch_cnt);
            }
            dHAcc /= dsTestSet.element_count;
            dPrec /= dsTestSet.element_count;
            dRc   /= dsTestSet.element_count;
            auto iEpEnd = NEUNET_CHRONO_TIME_POINT;
            print_deduce_status(iEp, dHAcc, dPrec, dRc, iEpEnd - iEpBegin);
            if (dRc == 1) iAsyncStat = NEUNET_STAT_END;
        } while (iAsyncStat != NEUNET_STAT_END);
        return true;
    }

private:
    async::net_async_digit<uint64_t> iCurrLayerIdx = 0,
                                     iAsyncStat    = NEUNET_STAT_UPT;
    
    async::net_async_concurrent asyConcurr;
    async::net_async_controller asyControl;
    async::net_async_pool       asyPool;

};

NEUNET_END
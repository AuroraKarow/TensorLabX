ASYNC_BEGIN

/* Function encapsulate */
template<typename _rtnT, typename ... _parasT> std::function<_rtnT(_parasT...)> capsulate_function(_rtnT(*func_val)(_parasT...))
{
    std::function<_rtnT(_parasT...)> func_temp = static_cast<_rtnT(*)(_parasT...)>(func_val);
    return func_temp;
}
template<typename _funcT, typename ... _parasT> auto package_function(_funcT &&func_val, _parasT &&...paras) -> PACK_PTR<typename std::result_of<_funcT(_parasT...)>::type>
{
    using _rtnT = typename std::result_of<_funcT(_parasT...)>::type;
    return std::make_shared<std::packaged_task<_rtnT()>>(std::bind(std::forward<_funcT>(func_val), std::forward<_parasT>(paras)...));
}

/* Basic variable */
template<typename _varT> inline async_variable<_varT>::async_variable(_varT init_val) : value(init_val) {}
template<typename _varT> inline async_variable<_varT>::async_variable(async_variable &src) : value(src._value()) {}
template<typename _varT> inline void async_variable<_varT>::operator=(_varT val) { _value(val); }
template<typename _varT> inline void async_variable<_varT>::operator=(async_variable &src) { _value(src._value()); }
template<typename _varT> inline async_variable<_varT>::operator _varT() { return _value(); }
template<typename _varT> inline _varT async_variable<_varT>::_value()
{
    std::shared_lock<std::shared_mutex> lock(shrd_mtx);
    return value;
}
template<typename _varT> inline void async_variable<_varT>::_value(_varT tar_sgn)
{
    std::unique_lock<std::shared_mutex> lock(shrd_mtx);
    value = tar_sgn;
}
/* Digital variable */
template<typename digitT, typename _Ty> inline async_digit<digitT, _Ty>::async_digit(digitT init_val) : async_variable(init_val) {}
template<typename digitT, typename _Ty> inline async_digit<digitT, _Ty>::async_digit(async_variable<digitT> &src) : async_variable(src) {}
template<typename digitT, typename _Ty> inline void async_digit<digitT, _Ty>::operator=(digitT val) { async_variable::operator=(val); }
template<typename digitT, typename _Ty> inline void async_digit<digitT, _Ty>::operator=(async_variable<digitT> &src) { async_variable::operator=(src); }
template<typename digitT, typename _Ty> inline void async_digit<digitT, _Ty>::operator+=(async_variable<digitT> &src)
{
    static_assert(!std::is_same<digitT, bool>::value, "Type \"bool\" could not be incremented.");
    value += src;
}
template<typename digitT, typename _Ty> inline void async_digit<digitT, _Ty>::operator+=(digitT &src)
{
    static_assert(!std::is_same<digitT, bool>::value, "Type \"bool\" could not be incremented.");
    value += src;
}
template<typename digitT, typename _Ty> inline void async_digit<digitT, _Ty>::operator-=(async_variable<digitT> &src)
{
    static_assert(!std::is_same<digitT, bool>::value, "Type \"bool\" could not be decremented.");
    value -= src;
}
template<typename digitT, typename _Ty> inline void async_digit<digitT, _Ty>::operator-=(digitT &src)
{
    static_assert(!std::is_same<digitT, bool>::value, "Type \"bool\" could not be decremented.");
    value -= src;
}
template<typename digitT, typename _Ty> inline async_digit<digitT, _Ty> &async_digit<digitT, _Ty>::operator++() { increment(); return *this; }
template<typename digitT, typename _Ty> inline async_digit<digitT, _Ty> async_digit<digitT, _Ty>::operator++(int) { auto temp = *this; increment(); return temp; }
template<typename digitT, typename _Ty> inline async_digit<digitT, _Ty> &async_digit<digitT, _Ty>::operator--() { decrement(); return *this; }
template<typename digitT, typename _Ty> inline async_digit<digitT, _Ty> async_digit<digitT, _Ty>::operator--(int) { auto temp = *this; decrement(); return temp; }
template<typename digitT, typename _Ty> inline void async_digit<digitT, _Ty>::increment()
{
    static_assert(!std::is_same<digitT, bool>::value, "Type \"bool\" could not be incremented.");
    std::unique_lock<std::shared_mutex> lock(shrd_mtx);
    ++ value;
}
template<typename digitT, typename _Ty> inline void async_digit<digitT, _Ty>::decrement()
{
    static_assert(!std::is_same<digitT, bool>::value, "Type \"bool\" could not be decremented.");
    std::unique_lock<std::shared_mutex> lock(shrd_mtx);
    -- value;
}

/* Asynchronizing control */
inline void async_control::thread_sleep()
{
    std::unique_lock<std::mutex> lk(td_mtx);
    cond.wait(lk);
}
inline void async_control::thread_wake_all() { cond.notify_all(); }
inline void async_control::thread_wake_one() { cond.notify_one(); }

/* Asynchronizing concurrent */
inline async_concurrent::async_concurrent(uint64_t batch_size) { set_batch_size(batch_size);}
inline void async_concurrent::set_batch_size(uint64_t batch_size) { async_batch_size = batch_size; }
inline void async_concurrent::batch_thread_attach()
{
    if((++ready_cnt) == async_batch_size) ctrl_main.thread_wake_one();
    ctrl_batch.thread_sleep();
}
inline void async_concurrent::batch_thread_detach(std::function<void()> concurr_opt)
{
    if((++proc_cnt) == async_batch_size)
    {
        concurr_opt();
        ctrl_main.thread_wake_one();
    }
}
inline void async_concurrent::main_thread_deploy_batch_thread()
{
    if(ready_cnt != async_batch_size) ctrl_main.thread_sleep();
    proc_cnt = 0; ready_cnt = 0;
    ctrl_batch.thread_wake_all();
    if(proc_cnt != async_batch_size) ctrl_main.thread_sleep();
}
inline void async_concurrent::main_thread_exception() { ctrl_batch.thread_wake_all(); }

/* Asynchronizing queue */
template<typename _Ty> inline async_queue<_Ty>::async_queue(async_queue &src)
{
    std::unique_lock<std::shared_mutex> lck(src.tdmtx);
    ls_val = src.ls_val;
}
template<typename _Ty> inline async_queue<_Ty>::async_queue(async_queue &&src)
{
    std::unique_lock<std::shared_mutex> lck(src.tdmtx);
    ls_val = std::move(src.ls_val);
    src.reset();
}
template<typename _Ty> inline void async_queue<_Ty>::operator=(async_queue &src)
{
    std::unique_lock<std::shared_mutex> lck(src.tdmtx);
    ls_val = src.ls_val;
}
template<typename _Ty> inline void async_queue<_Ty>::operator=(async_queue &&src)
{
    std::unique_lock<std::shared_mutex> lck(src.tdmtx);
    ls_val = std::move(src.ls_val);
    src.reset();
}
template<typename _Ty> inline bool async_queue<_Ty>::operator==(async_queue &src)
{
    std::unique_lock<std::shared_mutex> lck(src.tdmtx);
    return (this->ls_val == src.ls_val);
}
template<typename _Ty> inline bool async_queue<_Ty>::operator!=(async_queue &src) { return !(*this != src); }
template<typename _Ty> inline async_queue<_Ty>::async_queue() {}
template<typename _Ty> inline uint64_t async_queue<_Ty>::size()
{
    std::shared_lock<std::shared_mutex> lck(tdmtx);
    return ls_val.size();
}
template<typename _Ty> template<typename...args> inline bool async_queue<_Ty>::en_queue(args &&...paras)
{
    std::unique_lock<std::shared_mutex> lck(tdmtx);
    return ls_val.emplace_back(std::forward<args>(paras)...);
}
template<typename _Ty> inline _Ty async_queue<_Ty>::de_queue()
{
    std::unique_lock<std::shared_mutex> lck(tdmtx);
    return ls_val.erase(IDX_ZERO);
}
template<typename _Ty> inline void async_queue<_Ty>::reset()
{
    std::unique_lock<std::shared_mutex> lck(tdmtx);
    ls_val.reset();
}
template<typename _Ty> inline async_queue<_Ty>::~async_queue() { reset(); }

/* Asynchronizing batch */
inline async_batch::async_batch(uint64_t batch_size) : asyn_batch_size(batch_size), td_set(batch_size), tsk_set(batch_size), proc_set(batch_size)
{
    for(auto i=0; i<asyn_batch_size; ++i) td_set[i] = std::thread([this](int idx){ while(true)
    {
        {
            std::unique_lock<std::mutex> lkTsk(td_mtx_tsk);
            while(!(proc_set[idx] || stop)) cond_tsk.wait(lkTsk);
        }
        if(stop) return;
        tsk_set[idx]();
        -- tsk_cnt;
        {
            std::unique_lock<std::mutex> lkProc(td_mtx_proc);
            proc_set[idx] = false;
            cond_proc.notify_all();
        }
    }}, i);
}
template<typename _func, typename ... _para> inline auto async_batch::set_task(uint64_t bat_idx, _func &&func_val, _para &&...args) -> std::future<typename std::result_of<_func(_para...)>::type>
{
    using ret_type = typename std::result_of<_func(_para...)>::type;
    auto p_curr_task = package_function(std::forward<_func>(func_val), std::forward<_para>(args)...);
    std::future<ret_type> res = p_curr_task->get_future();
    if(bat_idx < asyn_batch_size)
    {
        std::unique_lock<std::mutex> lkProc(td_mtx_proc);
        while(proc_set[bat_idx] && !stop) cond_proc.wait(lkProc);
        tsk_set[bat_idx] = [p_curr_task]() { (*p_curr_task)(); };
    }
    else stop = true;
    ++ tsk_cnt;
    {
        std::unique_lock<std::mutex> lkTsk(td_mtx_tsk);
        proc_set[bat_idx] = true;
        cond_tsk.notify_all();
    }
    return res;
}
inline uint64_t async_batch::task_cnt() { return tsk_cnt; }
inline uint64_t async_batch::batch_size() { return asyn_batch_size; }
inline async_batch::~async_batch()
{
    stop = true;
    cond_tsk.notify_all();
    for(auto i=0; i<td_set.size(); ++i) if(td_set[i].joinable()) td_set[i].join();
}

/* Asynchronizing pool */
inline async_pool::async_pool(uint64_t thread_size) : stop(false), td_set(thread_size)
{
    for(auto i=0; i<td_set.size(); ++i) td_set[i] = std::thread([this] { while(true)
    {
        std::function<void()> curr_tsk;
        {
            std::unique_lock<std::mutex> lck(td_mtx);
            while(!(this->tsk_set.size() || stop)) cond.wait(lck);
            if(this->stop && !this->tsk_set.size()) return;
            curr_tsk = std::move(this->tsk_set.de_queue());
        }
        curr_tsk();
    }});
}
template<typename _funcT, typename ... _parasT> inline auto async_pool::add_task(_funcT &&func_val, _parasT &&...paras) -> std::future<typename std::result_of<_funcT(_parasT ...)>::type>
{
    // Thread function result type name (For deduce)
    using _rtnT = std::result_of<_funcT(_parasT...)>::type;
    // Function task package
    auto task = package_function(std::forward<_funcT>(func_val), std::forward<_parasT>(paras)...);
    // Get task result
    std::future<_rtnT> rtn = task->get_future();
    {
        // Mutex lock
        std::unique_lock<std::mutex> lock(td_mtx);
        if(stop) throw std::runtime_error("Stop thread pool.");
        // Add task
        tsk_set.en_queue([task](){ (*task)(); });
    }
    cond.notify_one();
    return rtn;
}
inline async_pool::~async_pool()
{
    stop = true;
    cond.notify_all();
    for(auto i=0; i<td_set.size(); ++i) if(td_set[i].joinable()) td_set[i].join();
}

/* Asynchronizing bacth tast */
template<typename funcT, typename ... argsT> void set_batch_thread(uint64_t iAsyncBatchSize, async::async_digit<uint64_t> &asyncCnt, funcT &&funcVal, argsT &&...paras)
{
    async::async_batch asyncBatch(iAsyncBatchSize);
    for(auto i=0; i<iAsyncBatchSize; ++i) asyncBatch.set_task(i, 
    [&asyncCnt, &funcVal]
    (int idx, argsT &&...args)
    {
        funcVal(idx, std::forward<argsT>(args)...);
        ++ asyncCnt;
    }, i, std::forward<argsT>(paras)...);
    while(asyncCnt != iAsyncBatchSize);
}

ASYNC_END
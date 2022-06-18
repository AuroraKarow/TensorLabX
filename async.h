ASYNC_BEGIN

/* Function encapsulate */
template<typename _rtnT, typename ... _parasT> std::function<_rtnT(_parasT...)> capsulate_function(_rtnT(*func_val)(_parasT...));
template<typename _funcT, typename ... _parasT> auto package_function(_funcT &&func_val, _parasT &&...paras) -> PACK_PTR<typename std::result_of<_funcT(_parasT...)>::type>;

/* Asynchronizing variable */
/* Basic variable */
template<typename _varT> struct async_variable
{
public:
    async_variable(_varT init_val = _varT());
    async_variable(async_variable &src);
    void operator=(_varT val);
    void operator=(async_variable &src);
    operator _varT();
    _varT _value();
    void _value(_varT tar_sgn);
protected:
    mutable std::shared_mutex shrd_mtx;
    _varT value;
};
/* Digital variable */
template<typename digitT, typename = std::enable_if_t<std::is_floating_point<digitT>::value || std::is_integral<digitT>::value>> struct async_digit : public async_variable<digitT>
{
public:
    async_digit(digitT init_val = 0);
    async_digit(async_variable &src);
    void operator=(digitT val);
    void operator=(async_variable &src);
    void operator+=(async_variable &src);
    void operator+=(digitT &src);
    void operator-=(async_variable &src);
    void operator-=(digitT &src);
    async_digit &operator++();
    async_digit operator++(int);
    async_digit &operator--();
    async_digit operator--(int);
    void increment();
    void decrement();
};

/* Asynchronizing operation */
/* Asynchronizing control */
struct async_control
{
public:
    void thread_sleep();
    void thread_wake_all();
    void thread_wake_one();
private:
    std::mutex td_mtx;
    std::condition_variable cond;
};

/* Asynchronizing concurrent */
struct async_concurrent
{
public:
    async_concurrent(uint64_t batch_size = ASYNC_CORE_CNT);
    void set_batch_size(uint64_t batch_size);
    void batch_thread_attach();
    void batch_thread_detach(std::function<void()> concurr_opt = []{ return; });
    void main_thread_deploy_batch_thread();
    void main_thread_exception();
private:
    uint64_t async_batch_size = 0;
    async::async_digit<uint64_t> proc_cnt = 0, ready_cnt = 0;
    async_control ctrl_batch, ctrl_main;
};

/* Asynchronizing queue */
template<typename _Ty> class async_queue final
{
public:
    async_queue(async_queue &src);
    async_queue(async_queue &&src);
    void operator=(async_queue &src);
    void operator=(async_queue &&src);
    bool operator==(async_queue &src);
    bool operator!=(async_queue &src);
    async_queue();
    uint64_t size();
    template<typename...args> bool en_queue(args &&...paras);
    _Ty de_queue();
    friend std::ostream &operator<<(std::ostream &output, async_queue &src)
    {
        std::shared_lock<std::shared_mutex> lck(src.tdmtx);
        output << src.ls_val;
        return output;
    }
    void reset();
    ~async_queue();
private:
    _BAGRT net_list<_Ty> ls_val;
    mutable std::shared_mutex tdmtx;
public:
    __declspec(property(get=size)) uint64_t length;
};

/* Asynchronizing batch */
class async_batch
{
public:
    async_batch(uint64_t batch_size = ASYNC_CORE_CNT);
    template<typename _func, typename ... _para> auto set_task(uint64_t bat_idx, _func &&func_val, _para &&...args) -> std::future<typename std::result_of<_func(_para...)>::type>;
    uint64_t task_cnt();
    uint64_t batch_size();
    ~async_batch();
private:
    _BAGRT net_set<std::thread> td_set;
    _BAGRT net_set<std::function<void()>> tsk_set;
    async_digit<uint64_t> tsk_cnt = 0;
    async_digit<bool> stop = false;
    _BAGRT net_set<async_digit<bool>> proc_set;
    std::mutex td_mtx_tsk, td_mtx_proc;
    std::condition_variable cond_tsk, cond_proc;
    uint64_t asyn_batch_size = ASYNC_CORE_CNT;
};

/* Asynchronizing pool */
class async_pool
{
public:
    async_pool(uint64_t thread_size = ASYNC_CORE_CNT);
    template<typename _funcT, typename ... _parasT> auto add_task(_funcT &&func_val, _parasT &&...paras) -> std::future<typename std::result_of<_funcT(_parasT ...)>::type>;
    ~async_pool();
private:
    _BAGRT net_set<std::thread> td_set;
    async_queue<std::function<void()>> tsk_set;
    std::mutex td_mtx;
    std::condition_variable cond;
    async_digit<bool> stop = false;
};

/* Asynchronizing bacth tast */
template<typename funcT, typename ... argsT> void set_batch_thread(uint64_t iAsyncBatchSize, async::async_digit<uint64_t> &asyncCnt, funcT &&funcVal, argsT &&...paras);

ASYNC_END

#include "async.hpp"
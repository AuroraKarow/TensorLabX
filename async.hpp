ASYNC_BEGIN

template<typename r_arg> using net_pack_ptr = std::shared_ptr<std::packaged_task<r_arg()>>;

template<typename r_arg, typename ... args> std::function<r_arg(args...)> function_capsulate(r_arg (*func)(args...)) {
    std::function<r_arg(args...)> temp = static_cast<r_arg(*)(args...)>(func);
    return temp;
}

template<typename f_arg, typename ... args> auto function_package(f_arg &&func, args &&...paras) -> net_pack_ptr<typename std::invoke_result_t<f_arg, args...>> {
    using r_arg = typename std::invoke_result_t<f_arg, args...>;
    return std::make_shared<std::packaged_task<r_arg()>>(std::bind(std::forward<f_arg>(func), std::forward<args>(paras)...));
}

template<typename arg> struct net_async_variable {
protected:
    void value(const arg &src) {
        std::unique_lock<std::shared_mutex> lk(sh_mtx);
        val = src;
    }
    arg value() const {
        std::shared_lock<std::shared_mutex> lk(sh_mtx);
        return val;
    }

public:
    net_async_variable(const arg &src = arg()) :
        val(src) {}
    net_async_variable(const net_async_variable &src) :
        val(src.val) {}

    net_async_variable &operator=(const net_async_variable &src) {
        value(src.value());
        return *this;
    }

    operator arg() const { return value(); }
    
protected:
    mutable std::shared_mutex sh_mtx;
    arg val;
};

template <typename arg, typename number_arg> struct net_async_digit : public net_async_variable<arg> {
protected:
    void self_cal_assign(const arg &src, bool increase) {
        static_assert(!std::is_same_v<bool, arg>, "\"Boolean\" variable could not be incremented or decremented.");
        {
            std::unique_lock<std::shared_mutex> lk(this->sh_mtx);
            if (increase) this->val += src;
            else this->val -= src;
        }
    }

public:
    net_async_digit(const arg &src = 0) : net_async_variable<arg>(src) {}
    net_async_digit(const net_async_digit &src) : net_async_variable<arg>(src) {}
    
    net_async_digit &operator=(const net_async_digit &src) {
        net_async_variable<arg>::operator=(src);
        return *this;
    }

    void operator+=(const net_async_digit &src) {
        auto temp = src.value();
        self_cal_assign(temp, true);
    }

    void operator-=(const net_async_digit &src) {
        auto temp = src.value();
        self_cal_assign(temp, false);
    }

    net_async_digit &operator++() {
        self_cal_assign(1, true);
        return *this;
    }
    net_async_digit operator++(int) {
        auto temp = *this;
        ++*this;
        return temp;
    }
    
    net_async_digit &operator--() {
        self_cal_assign(1, false);
        return *this;
    }
    net_async_digit operator--(int) {
        auto temp = *this;
        ++*this;
        return temp;
    }
};

struct net_async_controller final {
public:
    void thread_sleep() {
        std::unique_lock<std::mutex> lk(td_mtx);
        cond.wait(lk);
    }

    void thread_wake_all() { cond.notify_all(); }

    void thread_wake_one() { cond.notify_one(); }

private:
    std::mutex td_mtx;
    std::condition_variable cond;
};

struct net_async_concurrent final {
public:
    net_async_concurrent(uint64_t batch_size = NEUNET_ASYNC_CORE) :
        batch_cnt(batch_size) {}

    void set_batch_size(uint64_t batch_size) { batch_cnt = batch_size; }

    void batch_thread_attach() {
        if ((++ready_cnt) == batch_cnt) ctrl_main.thread_wake_one();
        ctrl_batch.thread_sleep();
    }

    void batch_thread_detach(std::function<void()> concurr_opt = []{ return; }) {
        if ((++proc_cnt) == batch_cnt) {
            concurr_opt();
            ctrl_main.thread_wake_one();
        }
    }

    void main_thread_deploy_batch_thread() {
        if (ready_cnt != batch_cnt) ctrl_main.thread_sleep();
        proc_cnt  = 0;
        ready_cnt = 0;
        ctrl_batch.thread_wake_all();
        if (proc_cnt != batch_cnt) ctrl_main.thread_sleep();
    }

    void main_thread_exception() { ctrl_batch.thread_wake_all(); }

private:
    uint64_t batch_cnt = 0;

    net_async_digit<uint64_t> proc_cnt  = 0,
                              ready_cnt = 0;

    net_async_controller ctrl_batch,
                         ctrl_main;
};

// Multi-thread safty
template<typename arg> class net_queue final {
public:
    net_queue() {}
    net_queue(net_queue &&src) :
        elem_ls(std::move(src.elem_ls)) { src.reset(); }
    net_queue(const net_queue &src) :
        elem_ls(src.elem_ls) {}
    
    uint64_t size() const {
        std::shared_lock<std::shared_mutex> lck(td_mtx);
        return elem_ls.length;
    }

    template<typename...args> bool en_queue(args &&...paras) {
        std::unique_lock<std::shared_mutex> lck(td_mtx);
        return elem_ls.emplace_back(std::forward<args>(paras)...);
    }

    arg de_queue() {
        std::unique_lock<std::shared_mutex> lck(td_mtx);
        return elem_ls.erase(0);
    }

    void reset() {
        std::unique_lock<std::shared_mutex> lck(td_mtx);
        elem_ls.reset();
    }

    ~net_queue () { reset(); }

private:
    net_list<arg> elem_ls;
    mutable std::shared_mutex td_mtx;
     
public:
    __declspec(property(get=size)) uint64_t length;

    net_queue &operator=(net_queue &&src) {
        std::unique_lock<std::shared_mutex> lck(td_mtx);
        elem_ls = std::move(src.elem_ls);
        return *this;
    }
    net_queue &operator=(const net_queue &src) {
        std::unique_lock<std::shared_mutex> lck(td_mtx);
        elem_ls = src;
        return *this;
    }

    bool operator==(const net_queue &src) const { return src.elem_ls == elem_ls; }

    bool operator!=(const net_queue &src) const { return src.elem_ls != elem_ls; }
};

// Batch processing
class net_async_batch final {
public:
    net_async_batch(uint64_t batch_size = NEUNET_ASYNC_CORE) :
        batch_cnt(batch_size),
        td_set(batch_size),
        tsk_set(batch_size),
        proc_set(batch_size),
        stop(false) {
        for (auto i = 0ull; i < batch_cnt; ++i) td_set[i] = std::thread([this](uint64_t idx) { while (true){
            {
                std::unique_lock<std::mutex> lk_tsk(td_mtx_tsk);
                while (!(proc_set[idx] || stop)) cond_tsk.wait(lk_tsk);
            }
            if (stop) return;
            tsk_set[idx]();
            --tsk_cnt;
            {
                std::unique_lock<std::mutex> lk_proc(td_mtx_proc);
                proc_set[idx] = false;
                cond_proc.notify_all();
            }
        }}, i);
    }
    
    template<typename f_arg, typename ... args> auto set_task(uint64_t bat_idx, f_arg &&func, args &&...paras) -> std::future<std::invoke_result_t<f_arg, args...>>
    {
        using r_arg = std::invoke_result_t<f_arg, args...>;
        auto p_curr_task = function_package(std::forward<f_arg>(func), std::forward<args>  (paras)...);
        std::future<r_arg> res = p_curr_task->get_future();
        if(bat_idx < batch_cnt) {
            std::unique_lock<std::mutex> lk_proc(td_mtx_proc);
            while (proc_set[bat_idx] && !stop) cond_proc.wait(lk_proc);
            tsk_set[bat_idx] = [p_curr_task]() { (*p_curr_task)(); };
        }
        else stop = true;
        ++ tsk_cnt;
        {
            std::unique_lock<std::mutex> lk_tsk(td_mtx_tsk);
            proc_set[bat_idx] = true;
            cond_tsk.notify_all();
        }
        return res;
    }

    uint64_t curr_task_cnt() const { return tsk_cnt; }

    uint64_t curr_batch_size() const { return batch_cnt; }

    ~net_async_batch() {
        stop = true;
        cond_tsk.notify_all();
        for (auto i = 0ull; i < td_set.length; ++i) if (td_set[i].joinable()) td_set[i].join();
    }

private:
    net_set<std::thread>           td_set;
    net_set<std::function<void()>> tsk_set;
    net_set<net_async_digit<bool>> proc_set;

    net_async_digit<uint64_t> tsk_cnt = 0;
    net_async_digit<bool>     stop    = false;

    uint64_t batch_cnt = NEUNET_ASYNC_CORE;

    std::mutex td_mtx_tsk,
               td_mtx_proc;

    std::condition_variable cond_tsk, 
                            cond_proc;
};

// Thread pool. This class should be used in the thread for contolling only.
class net_async_pool final {
public:
    net_async_pool(uint64_t thread_size = NEUNET_ASYNC_CORE) :
        stop(false), td_set(thread_size) {
        for (auto i  =0ull; i < td_set.size(); ++i) td_set[i] = std::thread([this] { while(true) {
            std::function<void()> curr_tsk;
            {
                std::unique_lock<std::mutex> lck(td_mtx);
                while (!(this->tsk_set.size() || stop)) cond.wait(lck);
                if (this->stop && !this->tsk_set.size()) return;
                curr_tsk = std::move(this->tsk_set.de_queue());
            }
            curr_tsk();
        }});
    }

    template<typename f_arg, typename ... args> auto add_task(f_arg &&func, args &&...paras) -> std::future<std::invoke_result_t<f_arg, args...>> {
        using r_arg = std::invoke_result_t<f_arg, args...>;
        auto p_curr_task = function_package(std::forward<f_arg>(func), std::forward<args>  (paras)...);
        std::future<r_arg> res = p_curr_task->get_future();
        {
            std::unique_lock<std::mutex> lock(td_mtx);
            if (stop) throw std::runtime_error("Thread pool's stopped.");
            tsk_set.en_queue([p_curr_task](){ (*p_curr_task)(); });
        }
        cond.notify_one();
        return res;
    }

    ~net_async_pool()
    {
        stop = true;
        cond.notify_all();
        for (auto i = 0ull; i < td_set.size(); ++i) if (td_set[i].joinable()) td_set[i].join();
    }

private:
    net_set<std::thread> td_set;
    net_queue<std::function<void()>> tsk_set;

    std::mutex td_mtx;
    std::condition_variable cond;

    net_async_digit<bool> stop = false;
};

ASYNC_END
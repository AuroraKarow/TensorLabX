ASYNC_BEGIN

template<typename r_arg, typename ... args> constexpr std::function<r_arg(args...)> function_capsulate(r_arg (*func)(args...)) { return static_cast<r_arg(*)(args...)>(func); }

template<typename f_arg, typename ... args> constexpr auto function_package(f_arg &&func, args &&...paras) { return std::make_shared<std::packaged_task<std::invoke_result_t<f_arg, args...>()>>(std::bind(std::forward<f_arg>(func), std::forward<args>(paras)...)); }

struct async_controller final {
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

struct async_concurrent final {
public:
    async_concurrent(uint64_t batch_size = NEUNET_ASYNC_CORE) :
        batch_cnt(batch_size) {}

    void set_batch_size(uint64_t batch_size) { batch_cnt = batch_size; }

    void batch_thread_attach() {
        if ((++ready_cnt) == batch_cnt) ctrl_main.thread_wake_one();
        if (ready_cnt) ctrl_batch.thread_sleep();
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

    std::atomic_uint64_t proc_cnt  = 0,
                         ready_cnt = 0;

    async_controller ctrl_batch,
                     ctrl_main;
};

// Multi-thread safe queue
template<typename arg> class net_queue final {
public:
    net_queue() {}
    
    uint64_t size() const {
        std::unique_lock<std::mutex> lck(td_mtx);
        return elem_ls.length;
    }

    // arg front() {
    //     std::shared_lock<std::shared_mutex> lck(td_mtx);
    //     return elem_ls[0];
    // }

    // arg rear() {
    //     std::shared_lock<std::shared_mutex> lck(td_mtx);
    //     return elem_ls[elem_ls.length - 1];
    // }

    template<typename...args> bool en_queue(args &&...paras) {
        std::unique_lock<std::mutex> lck(td_mtx);
        auto flag = elem_ls.emplace_back(std::forward<args>(paras)...);
        td_cond.notify_one();
        return flag;
    }

    arg de_queue() {
        std::unique_lock<std::mutex> lck(td_mtx);
        if (!elem_ls.length) td_cond.wait(lck);
        return elem_ls.erase(0);
    }

    ~net_queue () = default;

private:
    net_list<arg> elem_ls;
    mutable std::mutex td_mtx;
    std::condition_variable td_cond;
};

// Thread pool. This class should be used in the thread for contolling only.
class async_pool final {
public:
    async_pool(uint64_t thread_size = NEUNET_ASYNC_CORE) :
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

    uint64_t size() { return td_set.length; }

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

    ~async_pool()
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

    std::atomic_bool stop = false;
};

ASYNC_END
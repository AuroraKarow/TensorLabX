# `async_pool`

```c++
class async_pool final;
```

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
`async_pool`|構造函數，用給定的綫程數量初始化綫程池，如果沒有指定參數，它將被設定為默認的機器 CPU 綫程數<br>Constructor, initialize the thread pool with given threads amount, it would be set with default thread count of the machine CPU for no specified parameter
[`add_task`](add_task.md)|添加綫程任務<br>Add task to thread pool
`size`|獲取綫程數<br>Get thread amount

```c++
using std::cout;
using std::endl;

using std::atomic_uint64_t;

using neunet::async::async_pool;
using neunet::async::async_controller;
// 初始化一個數組並獲取長度
// Initialize an array and get length
int arr[] = {0, 2, 4, 6, 8},
    arr_n = sizeof(arr) / sizeof(int);
// 初始化綫程池與控制器
// Initialize thread pool and controller
async_pool       pool(arr_n);
async_controller ctrl;
// 計數器
// Counter
atomic_uint64_t  tsk_cnt = 0;
// 打印數組，不能同步訪問和修改數組，只能進行異步操作！
// Print array, array could not be accessed and altered synchronous, it should be only operated asynchronously
for (auto temp : arr) cout << temp << ' ';
cout << '\n';
// 添加任務到池子
// Add task to thread pool
for (auto i = 0; i < arr_n; ++i) pool.add_task([&tsk_cnt, &ctrl, &arr, arr_n, i]{
    // 每個數字擴大四倍
    // Quadrupling of each number
    arr[i] += arr[i];
    arr[i] *= 2;
    // 當最後一個任務完成，喚醒主綫程
    // While the last task finished, wakes up main thread
    if (++tsk_cnt == arr_n) ctrl.thread_wake_one();
});
// 部署完任務主綫程睡眠
// Main thread sleeps after tasks deployment
ctrl.thread_sleep();
// 打印操作后的數組
// Print operated array
for (auto temp : arr) cout << temp << ' ';
cout << endl;
```

[<< 返回 Back](../cover.md)

---

*更多關於異步控制，請參閲* [`async_controller`](../async_controller.md) *。*\
*Please refer to* [`async_controller`](../async_controller.md) *for more detail of asynchronous control*

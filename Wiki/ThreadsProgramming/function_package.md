# `function_package`

```c++
template<typename f_arg,
         typename ... args> constexpr auto
         function_package(f_arg &&func,
                          args  &&...paras)
```

封裝綫程任務函數與函數傳入參數。\
Encapsulate thread task fuction and function parameter input.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`func`|函數指針<br>|*輸入<br>input*
`paras`|綁定參數列<br>Parameter binding list|*輸入<br>input*

返回異步任務函數指針。\
Return asynchronous task function pointer

```c++
#include <iostream>
#include "async"
using namespace neunet::async;
using namespace std;

int add(int a, int b) { return a + b; }

int main(int argc, char **argv) {
    // 設定綁定的函數參數
    // Set function parameters for binding
    int  add_a = 10, 
         add_b = 20;
    // 打包綫程任務函數
    // Packing thread task function
    auto add_f = function_package(add, add_a, add_b);
    // 異步控製器
    // Asynchronous controller
    async_controller ctrl;
    // 任務函數返回
    // Task function return
    auto ret = add_f->get_future();
    // 子綫程
    // Child thread
    std::thread tsk([&ctrl, &ret, add_f]{
        // 解引用並執行綫程任務函數
        // Dereference and execute the thread task function
        (*add_f)();
        // 喚醒主綫程
        // Wake up main thread
        ctrl.thread_wake_one();
    });
    // 主綫程睡眠
    // Sleep the main thread
    ctrl.thread_sleep();
    // 打印綫程任務返回
    // Print thread task return
    cout << ret.get() << endl;
    // 加載子綫程
    // Load child thread
    if (tsk.joinable()) tsk.join();
    return 0;
}
```

[<< 返回 Back](cover.md)

---

*更多關於異步控制器，請參閲* [`async_controller`](async_controller.md) *。*\
*Please refer to* [`async_controller`](async_controller.md) *for more about the asynchronous controller.*

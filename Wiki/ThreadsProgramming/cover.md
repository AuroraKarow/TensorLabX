# 多綫程編程實驗 Multi-Thread programming experiment

```c++
#include "async"

using namespace neunet::async;
```

以下列表列出了可用的類。\
Available class(es) list below.

類型<br>Class or type|描述<br>Description
-|-
[`async_controller`](async_controller.md)|綫程異步控制器，控制器只能控制一對一、一對多以及多對一的綫程通信<br>Thread asynchronous controller, it could only contol the thread communication between one and one, one and multiple, multiple and one
[`net_queue`](net_queue.md)|綫程安全隊列<br>Thread safty queue
[`async_pool`](async_pool/cover.md)|綫程池<br>Threads pool

以下列表列出了可用的函數。\
Available function(s) list below.

函數<br>Function|描述<br>Description
-|-
[`function_capsulate`](function_capsulate.md)|重載函數封裝<br>Overloaded function encapsulation
[`function_package`](function_package.md)|異步函數打包<br>Asynchronous function packing

[<< 返回 Back](../../README.md)

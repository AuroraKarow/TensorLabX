# `async_controller`

```c++
struct async_controller final;
```

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
`thread_sleep`|控制器睡眠當前綫程，如果參數 `wait_ms` 為 `0` 綫程直到控制器函數 `thread_wake_one` 或 `thread_wake_one` 被調用不會醒來<br>Controller sleeps the current thread, thread would not wake up until the function `thread_wake_one` or `thread_wake_one` of the controller called if parameter `wait_ms` is `0` ms
`thread_wake_all`|控制器喚醒所有綫程<br>Controller wakes up all threads
`thread_wake_one`|控制器喚醒一個綫程<br>Controller wakes up one thread

[<< 返回 Back](cover.md)

---

*更多請參閲* [`async_pool`](async_pool/cover.md) *。*\
*Please refer to* [`async_pool`](async_pool/cover.md) *for more details.*

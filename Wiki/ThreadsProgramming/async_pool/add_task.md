# `add_task`

```c++
template<typename f_arg,
         typename ... args> auto
         add_task(f_arg &&func,
                  args  &&...paras);
```

指定函數指針與函數參數變量，添加綫程池任務。\
Specify function pointer and parameter varables, add task to thread pool

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`func`|函數指針<br>|*輸入<br>input*
`paras`|綁定參數列<br>Parameter binding list|*輸入<br>input*

返回任務函數的返回值，`std::future` 為返回值類汎型。\
Return the task function return, `std::function` is the return generic type.

[<< 返回 Back](cover.md)

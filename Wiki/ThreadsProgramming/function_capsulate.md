# `function_capsulate`

```c++
template<typename r_arg,
         typename ... args> constexpr std::function<r_arg(args...)>
         function_capsulate(r_arg (*func)(args...));
```

封裝包括重載在内的函數指針。\
Encapsulate the function pointer, including overloads.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`func`|函數指針，重載函數需要指定函數返回值與參數類型列<br>Function pointer, need to specify the return type and parameter type list of the function|*輸入<br>input*

返回函數封裝。\
Return function encapsulation.

```c++
#include <iostream>
#include "async"
// 重載函數
// Overload function
int add(int a, int b) { return a + b; }
double add(double a, double b) { return a + b; }

using namespace neunet::async;

int main(int argc, char **argv) {
    auto add_d = function_capsulate<double, double, double>(add);
    printf("3 + 4 = %lf", add_d(3, 4));
    return 0;
}
```

[<< 返回 Back](cover.md)

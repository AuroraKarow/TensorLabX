

#include <iostream>
#include "ThreadPool.hpp"
using namespace Tool;
int main()
{
    std::cout << "Hello World!\n";
    Tool::ThreadPool pool(2);
    int length = 4;
    int cnt = 12 / length;

    auto resf = std::vector<std::future<int>>();

    for (int i = 0; i < cnt; i++)
    {
        resf.emplace_back(pool.AddTask([i, length]
                                       { 
            printf_s("task %d begin work!\n", i);
            int sum = 0;
            int start = 12 / length * i;
            for (int k = start; k < start + length; k++)
            {
                sum += k;
            }
            printf_s("task %d finished work! res: %d\n", i, sum);
            return sum; }));
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));
    for (size_t i = 0; i < cnt; i++)
    {
        auto res = resf[i].get();
        std::cout << "res from main thread:" << res << std::endl;
    }
}

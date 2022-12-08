# `NEUNET_CHRONO_TIME_POINT`

獲取調用命令后瞬時時間點。\
Get instantial time point after calling this command.

```c++
using std::cout;
using std::endl;
using namespace neunet;
// 用区间[-1, 1]内的伪随机数初始化1000維方阵A, B
// Initialize 1000 dimensional phalanx A, B with pseudo random number in the interval [-1, 1]
vect A(1000, 1000, true, -1, 1),
     B(1000, 1000, true, -1, 1);
// 获取矩阵A和B之间乘法的时间长度
// Get the time duration of the multiplicaation between matrix A and B
auto time_pt = NEUNET_CHRONO_TIME_POINT;
auto C = A * B;
// 打印时间长度
// Print the duration
cout << (NEUNET_CHRONO_TIME_POINT - time_pt) << "ms" << endl;
```

[<< 返回 Back](cover.md)

---

*更多關於矩陣請參閲* [`net_matrix`](../../MatrixTensor/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../../MatrixTensor/net_matrix/cover.md) *for more about matrix*

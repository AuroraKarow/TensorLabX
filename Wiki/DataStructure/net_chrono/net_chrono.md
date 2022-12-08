# `net_chrono`

```c++
struct net_chrono final;
```

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`=`|計時器集合實例移動或複製賦值操作<br>Move or copy assignment operation for timer set instnce.

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
`net_chrono`|構造函數，初始化計時器集合初始緩衝區大小<br>Constructor, initialize memory buffer size of timer set.
`timer_begin`|開始計時，返回計時器令牌<br>Begin timing and return timer ID
`timer_end`|對應計時器令牌結束計時，計時結束成功返回`true`否則為`false`<br>Timer of corresponding ID ends timing, it would return `true` for ending timing successfully otherwise `false`
`duration`|獲取對應計時令牌耗時，如果計時失敗會返回-1<br>Get duration of corresponding timer ID, it would return -1 for timing failed.
`erase`|擦除對應計時器令牌的計時記錄<br>Erase target timer log of corresponding timer ID
`reset`|手動釋放計時器集合内存<br>Release timer set memory manually

```c++
using std::cout;
using std::endl;
using neunet::net_chrono;
// 當前綫程初始化計時器集合，緩衝大小默認
// Initialize a timer set of current thread, buffer length is default
net_chrono timer;
// 隨機初始化矩阵A、B、C
// Initialize matrix A, B, C randomly
vect A(1000, 2000, true, -1, 1),
     B(2000, 2000, true, -1, 1),
     C(2000, 3000, true, -1, 1);
// 对矩阵 A、B 和 A、C 之间的乘法进行计时，ID_AxB 和 ID_AxC 分别是这两个操作的计时器ID
// Timing the multiplicating between matrix A, B and A, C, ID_AxB and ID_AxC are timer ID of these two operations respectively
auto ID_AxB = timer.timer_begin();
auto D = A * B;
auto AxB_timer_status = timer.timer_end(ID_AxB);
auto ID_AxC = timer.timer_begin();
auto E = A * C;
auto AxC_timer_status = timer.timer_end(ID_AxC);
// 获取这两项操作的持续时间
// Get duration of these two operation
auto dur_AxB = timer.duration(ID_AxB),
     dur_AxC = timer.duration(ID_AxC);
```

[<< 返回 Back](cover.md)

---

*更多關於矩陣請參閲* [`net_matrix`](../../MatrixTensor/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../../MatrixTensor/net_matrix/cover.md) *for more about matrix*

# `insert`

```c++
template<typename ... args>
bool insert(uint64_t idx,
            args     &&...paras);
```

在目标索引对应的位置插入一个元素，索引不应大于集合长。\
Insert an element into the position corresponding to target index, index should not be greater than set length.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`idx`|目标索引<br>Target index|*輸入<br>input*
`paras`|插入的元素构造函數参数<br>Inserted element contructor parameters|*輸入<br>input*

插入元素成功返回`true`否則為`false`。\
Return `true` for inserting successfully otherwise `false`.

```c++
using std::cout;
using std::endl;
using neunet::net_set;
// 初始化一个矩阵集合
// Initialize a matrix set
net_set<vect> test = {
    {{0, 1},
     {2, 3}},

    {{1, 3, 5},
     {7, 9,11}},
     
    {{0, 2},
     {4, 6},
     {8,10}}
};
// 打印
// Print
cout << test << endl;
// 将矩阵 A (11, 19, 21) 插入到集合的第一位
// Insert matrix A (11, 19, 21) at first of the set
vect A = {{11, 19, 21}};
test.insert(0, A);
/* 插入矩陣
 * Insert matrix
 * 
 *  2 16 18
 * 32 14  7
 * -4 22 -1
 * 
 * 到表索引2的位置
 * to index 2 of the set
 */
// 插入矩陣
// Insert matrix
test.insert<vect>(2, {{ 2, 16, 18},
                      {32, 14,  7},
                      {-4, 22, -1}});
// 在集合的索引4处插入一个2行4列的空矩阵
// Insert a 2-line-4-column blank matrix at index 4 of the set
test.insert(4, 2, 4);
// 打印
// Print
cout << test << endl;
```

[<< 返回 Back](cover.md)

---

*更多關於類型* `vect` *請參閲* [`net_matrix`](../../MatrixCalculation/net_matrix/cover.md)\
*Please refer to* [`net_matrix`](../../MatrixCalculation/net_matrix/cover.md) *for more about type* `vect`

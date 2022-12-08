# `output_table`

```c++
void output_table(const net_set<net_set<string>> &output_strings, 
                  const string                   &file_path);
```

輸出2D字符串集合到目標本地路徑。\
Output 2D string set to target local path.

```c++
using namespace std;
using namespace neunet;
using namespace neunet::csvio;
// 創建一個2D字符串集合作爲表格
// Create a table as 2D string set
net_set<net_set<string>> tab_data = {
    {"name" , "age", "gender"},
    {"Harry", "16" , "male"  },
    {"Ray"  , "17" , "female"}
};
// 保存為 "src/test.csv"
// Save as "src/test.csv"
output_table(tab_data, "src/test.csv");
```

[<< 返回 Back](cover.md)

---

*更多關於動態集合請參閲* [`net_set`](../../net_set/cover.md)\
*Please refer to* [`net_set`](../../net_set/cover.md) *for more about dynamic set*

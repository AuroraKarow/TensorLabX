# `net_deciaml`

```c++
class net_decimal;
```

**屬性** Properties

變量<br>Varable|描述<br>Description
-|-
`calculate_accuracy`|計算精度，靜態屬性，直到下一次更改該屬性，計算保留的浮點位數均爲該值，默認值 `32`<br>Calculation accuracy, static property, the float point digit count of caulculation would be remained as this property value until next changing, default value `32`
`high_precision_mode`|高精度模式，靜態屬性，犧牲運算速度開啓更高精度模式，默認值 `false`<br>High precision mode, static property, enable higher precision mode at the expense of calculation speed, default value `false`
`modulo_mode`|模數模式，靜態屬性，如果這個屬性的值為 `true`，那麽運算符 `%` 將會計算模數<br>Modulo mode, static property, operator `%` would be get the modulus if the value of this property is `true`
`float_digit_count`|浮點位數引用，可以通過這個屬性獲取浮點位數或是指定小於當前值的浮點位數對浮點進行截斷<br>Reference of the float point digit count, it could get float point digit count or truncate the float point part of current value by specifying float point digit count
`integer_digit_count`|當前值得整數部分位數<br>Integer part digit count of the current value
`number_format`|當前 `net_decimal` 類型值的 `long double` 類型格式，可能會截斷<br>`long double` format of current `net_decimal` type of value, it might be truncated
`string_format`|當前 `net_decimal` 類型值的 `string` 類型格式<br>`string` format of current `net_decimal` type of value
`integer_part`|整數部分<br>Integer part of current value
`float_part`|浮點部分<br>Float point part of current value
`absolute`|絕對值<br>Absolute value of current value
`reciprocal`|倒數<br>Reciprocal value of current value

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`+` `+=` `++`|加<br>Addition
`-` `-=` `--`|減<br>Subtraction
`*` `*=`|乘<br>Multiplication
`/` `/=`|除<br>Division
`%` `%=`|余或模，取決於屬性 `modulo_mode` 的值<br>Remainder or modulus, it depends on the value of property `modulo_mode`
`~`|取反，浮點位為 `0`<br>Ones' complement, float point part should be `0`
`<<` `<<=`|左移，浮點位為 `0`<br>Left shift, float point part should be `0`
`>>` `>>=`|右移，浮點位為 `0`<br>Right shift, float point part should be `0`
`&` `&=`|與，浮點位為 `0`<br>And, float point part should be `0`
`\|` `\|=`|或，浮點位為 `0`<br>Or, float point part should be `0`
`^` `^=`|異或，浮點位為 `0`<br>Xor, float point part should be `0`
`<`|小于<br>Less than
`<=`|小于等于<br>Less than or equal to
`>`|大于<br>Greater than
`>=`|大于等于<br>Greater than or equal to
`==`|等于<br>Equal to
`!=`|不等于<br>Not equal to
`=`|賦值<br>Assignment

```c++
using decimal = neunet::net_decimal
// 初始化 net_decimal 的方法
// Methods of initializing net_decimal
decimal test_0 = "-0.216",
        test_1 = 9.02,
        test_2 = 12.7_dec;
// 標準函數支持
// Standard function supporting
auto res_0 = std::pow(test_0, 1/3.0),
     res_1 = std::exp(test_1),
     res_2 = std::log(test_2),
     res_3 = std::sin(res_0),
     res_4 = std::cos(res_1);
```

[<< 返回 Back](cover.md)

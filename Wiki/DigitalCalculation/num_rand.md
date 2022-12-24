# `num_rand`

## 重載 Overload $^1$

```c++
long double num_rand(long double fst_rng = 0,
                     long double snd_rng = 0,
                     uint64_t    acc = 8);
```

生成一個僞隨機數。\
Generate a pseudo random number.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`fst_rng`|僞隨機數範圍區間的第一個端點，如果該值與第二個端點相同，則會使用隨機數種子。<br>First endpoint of the pseudo number generation interval, it would use random number seed if this value equals second endpoint|*輸入<br>input*
`snd_rng`|僞隨機數範圍區間的第二個端點<br>Second endpoint of the pseudo number generation interval|*輸入<br>input*
`acc`|隨機數生成精度<br>Generation precision of the pseudo random number|*輸入<br>input*

返回僞隨機數。\
Return pseudo random number.

## 重載 Overload $^2$

```c++
callback_arg arg *num_rand(uint64_t amt,
                           arg      fst_rng = 0,
                           arg      snd_rng = 0,
                           bool     order   = true,
                           uint64_t acc     = 8);
```

生成隨機數數組。\
Generate pseudo random number array.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`amt`|隨機數個數，數組長度<br>Amount of pseudo random number, array length|*輸入<br>input*
`fst_rng`|僞隨機數範圍區間的第一個端點，如果該值與第二個端點相同，則會使用隨機數種子。<br>First endpoint of the pseudo number generation interval, it would use random number seed if this value equals second endpoint|*輸入<br>input*
`snd_rng`|僞隨機數範圍區間的第二個端點<br>Second endpoint of the pseudo number generation interval|*輸入<br>input*
`order`|指定是否返回有序數組<br>Specify wether returning ordered array|*輸入<br>input*
`acc`|隨機數生成精度<br>Generation precision of the pseudo random number|*輸入<br>input*

返回數組指針，需要手動釋放内存。\
Return array pointer which need to release memory manually.

[<< 返回 Back](cover.md)

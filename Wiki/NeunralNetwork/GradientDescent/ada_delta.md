# `ada_delta`

```c++
matrix_declare struct ada_delta final;
```

**字段** Fields

變量<br>Varable|描述<br>Description
-|-
`rho`|衰退率<br>Decay rate
`epsilon`|除 `0` 分母值<br>Denominator without `0`

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`=`|$AdaDelta$ 梯度下降實例移動或複製賦值操作<br>Move or copy assignment operation for $AdaDelta$ GD instance.
`==`|判定兩個 $AdaDelta$ 梯度下降實例是否相同<br>Determine if two $AdaDelta$ GD instances are same
`!=`|判定兩個 $AdaDelta$ 梯度下降實例是否不同<br>Determine if two $AdaDelta$ GD instances are different

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
`delta`|通過梯度獲取 $AdaDelta$ 的 $Delta$ 值，用於權重更新<br>Get $Delta$ value of $AdaDelta$ via gradient for weight updating
`reset`|手動釋放 $AdaDelta$ 結構分配的内存<br>Release the allocated memory of $AdaDelta$ structure manually

$$
W_{i}=W_{i-1}-Delta
$$

變量<br>Variable|描述<br>Description
-|-
$W_{i}$|下一次前向傳播的權重<br>Weight for next forward propagation
$W_{i-1}$|上一個權重<br>Previous weight
$Delta$|`delta` 函數返回值<br>Return of function `delta`

[<< 返回 Back](cover.md)

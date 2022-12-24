# `ada_nesterov`

```c++
matrix_declare struct ada_nesterov final;
```

**字段** Fields

變量<br>Varable|描述<br>Description
-|-
`rho`|衰退率<br>Decay rate

**操作符** Operators

符號<br>Operator|描述<br>Description
-|-
`=`|$Nadam$ 梯度下降實例移動或複製賦值操作<br>Move or copy assignment operation for $Nadam$ GD instance.
`==`|判定兩個 $Nadam$ 梯度下降實例是否相同<br>Determine if two $Nadam$ GD instances are same
`!=`|判定兩個 $Nadam$ 梯度下降實例是否不同<br>Determine if two $Nadam$ GD instances are different

**函數** Functions

函數名<br>Function name|描述<br>Description
-|-
`weight`|用當前權重 `curr_weight` 獲取前向傳播的 $Nadam$ 權重<br>Get $Nadam$ weight for forward propagation with current weight `curr_weight`
`momentum`|用當前梯度 `curr_grad` 和學習率 `learn_rate` 獲取動量值用以更新權重<br>Get momentum for weight updating with current gradient `curr_grad` and learn rate `learn_rate`
`reset`|手動釋放 $Nadam$ 結構分配的内存<br>Release the allocated memory of $Nadam$ structure manually

$$
W_{i}=W_{i-1}-momentum
$$

變量<br>Variable|描述<br>Description
-|-
$W_{i}$|下一次前向傳播的權重<br>Weight for next forward propagation
$W_{i-1}$|上一個權重<br>Previous weight
$momentum$|`momentum` 函數返回值<br>Return of function `momentum`

[<< 返回 Back](cover.md)

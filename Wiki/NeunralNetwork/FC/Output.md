# `Output`

```c++
callback_matrix neunet_vect Output(const neunet_vect &vecInput,
                                   const neunet_vect &vecWeight);
```

前向傳播，獲取當前網絡層輸入神經元信號的輸出。\
Forward propagation, get the neural signal output of current network layer input.

參數<br>Parameters|描述<br>Description|I/O
-|-|-
`vecInput`|輸入向量，輸入神經元信號<br>Input vecotr, input neural signal|*輸入<br>input*
`vecWeight`|權重<br>Weight|*輸入<br>input*

返回輸出向量。\
Return output vector.

[<< 返回 Back](cover.md)

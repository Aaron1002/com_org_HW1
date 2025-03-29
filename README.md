---
title: Com_org HW1

---

# Com_org HW1

## Array Bubble Sort

**Algorithm:** 比較後把比較大的放到最後，在 LOOP，對所有元素作相同任務

**The most difficult part:**
* 一開始做 Swap 的時候，不知道如何把 arr[i] & arr[j] 交換，後來想到用 "and" instruction (將資料和全 1 的 register 做 "and"，即可複製原本的資料)
=> Problem: 製作全 1 register
=> Sol: 在複習上課講義時看到 XOR 可達到我的期望目標，所以就用了 `xori x9, x0, -1`

## Array Search

Algorithm: Linear search

Records:
* 因為用的指令和第一題很像，邏輯也較簡單，所以製作 search 並沒有花很多時間，主要任務就是 Loop array，並比較是否和 target 相等
* 在製作時想到可能會有多個 target，所以有詢問過是否有規定要如何處理此類輸出，得到的結果是不會有多個一樣的 target，所以在開發時就沒有特別處理

## Linked List Merge Sort

困難點：
* **Node的移動:**
=> 因為不常用 struct，且寫高階程式時，不用考慮 padding，所以需要特別查找 C alignment 的底層操作。 發現在 64 bit 下，若用 int 會自動補 4 bytes，align to 8 bytes

* **指標的操作:**
=> 沒有特別注意到函數裡的 parameter 型態是 `Node**`，所以剛開始誤把 address 存到 `%[firstHalf]`，導致程式邏輯錯誤
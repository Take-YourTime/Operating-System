# Operating System 作業說明 (Homework Readme)

> [English](README.md) | [繁體中文]

這份 repo 包含了**作業系統**課程裡的四次程式作業，以下是各個作業的題目說明與實作方式介紹。

## HW1: Custom Simple Minix Shell Implementation

### 作業說明

實作一個可以執行基本指令與系統程式的自製 Shell。

### 實作方式

- 使用 C 語言撰寫 (`shell.c`)。
- 實作了內建指令 (Built-in commands) 如 `exit`、`cd`、`pwd`、`history`（顯示指定數量的歷史紀錄）、`help`，以及客製化指令如 `NSYSU` 和 `rabbit`。
- 支援執行 Minix3 系統中其他的預設指令。
- 能夠處理各種 Shell Operator，包含：背景執行 (`&`)、輸出與輸入重新導向 (`>`、`>>`、`<`) 以及管線 (`|`)。
- 核心邏輯透過 `fork()` 產生 Child Process，再利用 `exec` 相關函式執行外部指令；重新導向與管線則是透過操作 File Descriptor 以及 `pipe()` 來達成。

## HW2: Minix Kernel Modification & Process Scheduling

### 作業說明

修改 Minix 3 核心程式碼以改變 Process 優先級與排班機制，並監控行程資訊。

### 實作方式
 
- 透過修改 Minix 作業系統 Kernel 中的 `proc.c`、`proc.h`、`system.c` 以及 `clock.c` 檔案來完成。
- 實作分為三個部分：修改系統的排班演算法、設定 Process Priority，以及在 `clock.c` 中加入計時功能，讓系統每隔一段時間於螢幕輸出各個 Priority Level 內的 Process 數量。
- 將修改後的程式碼複製到 `/usr/src/minix/kernel` 目錄下，使用 `make clean`、`make kernel`、`make install` 重新編譯 Kernel，並重新啟動 (Reboot) 套用新核心。
  
### 備註

   此份作業未能完美達到要求，只能說 Minix 太難搞了。

## HW3: Shared Memory and Process Management

### 作業說明

延伸 HW1 的 Shell，加入 Shared Memory (共享記憶體) 以及進階的 Child Process (子行程) 管理功能。

### 實作方式

- 使用 C 語言撰寫 (`shell.c`)，保留 HW1 的基本功能。
- 執行 Shell 時可以傳入參數決定生成子行程的數量（預設為 1）。若輸入未定義的指令，會由這些子行程同時進行多次輸出。
- 實作了行程管理相關的函式（如 `Waitpid`、`ProcessFork`、`Spawn`、`Exit`）來管控子行程的狀態。
- 使用 `sys/shm.h` 與 `sys/ipc.h` 函式庫，透過 `shmget` 與 `shmat` 建立並連接 Shared Memory，讓 Parent Process 與 Child Processes 之間可以共享變數與資料。

## HW4: Disk Scheduling Algorithms

### 作業說明

實作並比較多種 Disk Scheduling Algorithms 在磁頭移動距離與延遲時間上的效能。

### 實作方式

- 使用 C++ 撰寫 (`DiskSchedulingAlgorithms.cpp`)，支援如 FCFS、SSTF、SCAN 等常見的排班演算法。
- 程式執行時會隨機生成一系列磁碟柱面 (Cylinder) 的 Requests。
- 接著提示使用者輸入磁頭的初始位置 (0 ~ 4999 之間)。
- 演算法邏輯中，利用 C++ 的 `vector` 儲存 Requests，並透過 `min_element` 等 STL 函式找出下一個目標 Cylinder。
- 程式會計算出各個演算法的「磁頭移動總路徑長度 (Total Head Movement)」以及「總延遲時間 (Latency)」。
- 可輸入多次初始位置進行測試，當輸入 `-1` 結束時，會自動計算並輸出所有測資的平均延遲時間。

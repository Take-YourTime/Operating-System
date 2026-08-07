# Programming Assignment 4

**Disk Scheduling Algorithms**

Student: B113040045許育菖

---

## 名詞解釋

- **cylinder**
    
    磁碟（disk）上的一個邏輯單位。
    
    磁碟驅動器由多個磁盤（disk platter）組成，每個磁盤由多個同心圓軌道（track）組成。這些同心圓軌道在所有磁盤上的垂直堆疊形成一個圓柱狀的邏輯單位，即所謂的「cylinder」。
    
- **餓死（starvation）**
    
    指某個process長時間甚指永遠無法獲得資源，導致其無法完成的情況。在磁碟排程中，「餓死」則指某些request因為算法的選擇策略長時間得不到服務。
    
- **磁頭**
    
    「磁碟讀寫頭」的簡寫。
    
- **等待時間（waiting time）**
    
    指的是一個requst從出現在系統中直到它被服務之間的時間。
    
- **尋道時間(seek time)**
    
    讀寫頭沿徑向移動，移到要讀取的扇區所在磁道的上方，這段時間稱為尋道時間(seek time)。
    

---

## Question

1. **What does each algorithm do? Describe its behavior.**
2. **What is the objective / goal of each algorithm?**
3. **What are the strengths and weaknesses of each disk-scheduling algorithm?**

### FCFS（First-Come, First-Served）

按照request到達的順序進行處理。

- **目標**：公平，按先後順序進行處理。
- **優點**：實現簡單，保證不會有requset被餓死（starvation）的情況發生。
- **缺點**：如果requset間距離較遠，會導致大量的磁頭移動，效率低下。此排程也是在實驗中平均延遲時間最久的演算法，甚至是接下來要介紹的幾個排程的十幾倍。

### SSTF（Shortest Seek Time First）

選擇距離當前磁頭位置最近的requset進行處理。

- **目標**：通過選擇最近的request來減少磁頭的移動距離，以減少尋道時間（seek time）。
- **優點**：相對於 FCFS，可以減少總尋道時間。
- **缺點**：可能會導致距離磁頭遠的請求被餓死。

### SCAN（電梯算法）

磁頭在磁碟兩端之間來回移動，並在移動過程中處理所有遇到的request。當磁頭到達一端後，會反向移動。

- **目標**：提供更均衡的等待時間（waiting time）。
- **優點**：相較於 FCFS 和 SSTF，減少了延遲時間的變化幅度，並使等待時間更均衡。
- **缺點**：在磁頭改變方向時可能導致較長的等待時間，因為會有一大段是已經處理過的request。

### C-SCAN（Circular SCAN）

與SCAN類似，但磁頭始終以同一方向上移動，並處理所有遇到的request。當到達終點時，直接返回起點，不處理返回途中遇到的request。

- **目標**：確保更均衡的等待時間。
- **優點**：相較於 SCAN，提供更均衡的等待時間。
- **缺點**：返回起點的過程可能會浪費一些時間。

### LOOK

類似於 SCAN，但磁頭只移動到當前方向上的最遠請求位置，而不是磁碟的邊界。

- **目標**：減少不必要的磁頭移動。
- **優點**：相較於 SCAN，減少了不必要的磁頭移動。
- **缺點**：實現複雜度增加。

### C-LOOK（Circular LOOK）

C-SCAN和LOOK的結合體，原理與C-SACAN大致相同，但磁頭只移動到當前方向上的最遠請求位置，然後快速返回到第一個requset的位置。

- **目標**：進一步減少不必要的磁頭移動。
- **優點**：相較於 C-SCAN，進一步減少不必要的磁頭移動。
- **缺點**：實現複雜度增加。

### Optimal

計算所有request的最佳處理順序，最小化總磁頭移動距離。

我設計的Optimal演算法，是以趕進LOOK演算法為主。在LOOK演算法中，每次都是從起點開始往固定一個方向進行掃描，直到碰到邊界才反向。反向時，會使磁頭移動很長一段距離。

因此，如果我們在最一開始時，對磁頭所在位置做**二分法**，如果靠近前端，則先往前端掃描；如果靠近末端，則先往末端掃描。如此一來，便可減少無頭在反向時的移動距離。

- **目標**：最小化總尋道時間。
- **優點**：提供理論上的最小尋道時間。
- **缺點**：需要預先知道所有請求，實時系統中難以實現。

| **算法** | **優點** | **缺點** |
| --- | --- | --- |
| **FCFS** | 簡單，不會有請求被餓死 | 效率低下，總磁頭移動距離大 |
| **SSTF** | 縮短總尋道時間 | 距離磁頭遠的請求可能會被餓死 |
| **SCAN** | 減少等待時間差異 | 磁頭改變方向時可能有較長等待時間 |
| **C-SCAN** | 提供均衡的等待時間 | 返回起始端的過程可能浪費時間 |
| **LOOK** | 減少不必要的磁頭移動 | 實現複雜度增加 |
| **C-LOOK** | 進一步減少不必要的磁頭移動 | 實現複雜度增加 |
| **Optimal** | 最小化總尋道時間 | 需要預知所有請求，實時系統中不可行 |
1. **What is the average latency of each algorithm (assume 1ms for every 100 cylinders)?**
2. **What is the total head movement required for each algorithm?**

先計算出每個算法的總辭頭移動路徑長，然後再把它除以100，即可得到總延遲時間。最後再除以case數量，就可以得到平均延遲時間（單位：ms），見右表格。

**每個test的詳細結果請見out.txt。**

| FCFS | 17420.8ms |
| --- | --- |
| SSTF | 71.0078ms |
| SCAN | 72.1578ms |
| C-SCAN | 127.678ms |
| LOOK | 72.0578ms |
| C-LOOK | 94.2078ms |
| Optimal | 56.76ms |

至於磁頭的移動距離，我這邊舉例兩個test的結果，剩餘的資料一樣在out.txt中。

**initial position: 1000**
FCFS: Total Head Movement = 1741898 cylinders, Latency = 17419 ms
SSTF: Total Head Movement = 9006 cylinders, Latency = 90.06 ms
SCAN: Total Head Movement = 8994 cylinders, Latency = 89.94 ms
C-SCAN: Total Head Movement = 13991 cylinders, Latency = 139.91 ms
LOOK: Total Head Movement = 8982 cylinders, Latency = 89.82 ms
C-LOOK: Total Head Movement = 9975 cylinders, Latency = 99.75 ms
Optimal: Total Head Movement = 5985 cylinders, Latency = 59.85 ms

**initial position: 4900**
FCFS: Total Head Movement = 1742512 cylinders, Latency = 17425.1 ms
SSTF: Total Head Movement = 5082 cylinders, Latency = 50.82 ms
SCAN: Total Head Movement = 5094 cylinders, Latency = 50.94 ms
C-SCAN: Total Head Movement = 10091 cylinders, Latency = 100.91 ms
LOOK: Total Head Movement = 5082 cylinders, Latency = 50.82 ms
C-LOOK: Total Head Movement = 9964 cylinders, Latency = 99.64 ms
Optimal: Total Head Movement = 5082 cylinders, Latency = 50.82 ms

1. **Think about each algorithm, when would you use one algorithm versus another? List 2
applications where each algorthm would perfom best.**

### FCFS

- **使用場景**：簡單系統，公平性比效率更重要。
- **應用**：
    - 日誌系統，必須按請求順序記錄。
    - 教育工具，用於展示磁碟排程概念。

### SSTF

- **使用場景**：尋道時間優化至關重要，且請求飢餓現象不明顯的系統。
- **應用**：
    - 實時系統，需要快速讀寫操作。
    - 請求均勻分佈且預測性高的系統。

### SCAN

- **使用場景**：需要均衡性能和公平性的系統。
- **應用**：
    - 數據庫系統，需要訪問大量數據。
    - 負載適中且請求模式多樣的系統。

### C-SCAN

- **使用場景**：大規模系統，需要均衡的等待時間。
- **應用**：
    - 處理大量數據的高性能服務器。
    - 高可用性要求的系統。

### LOOK

- **使用場景**：已知請求模式，最小化磁頭移動至關重要。
- **應用**：
    - 多媒體系統，流式傳輸數據，效率至關重要。
    - 科學計算環境，大量順序數據訪問。

### C-LOOK

- **使用場景**：類似於 LOOK，但需要週期性均勻性更重要的環境。
- **應用**：
    - 金融系統，處理大量交易。
    - 網絡存儲系統，多個並發訪問。

### Optimal

- **使用場景**：假設場景或模擬，需要最小化總尋道時間。
- **應用**：
    - 模擬磁盤性能的研究環境。
    - 批處理作業的離線處理，已知所有請求。
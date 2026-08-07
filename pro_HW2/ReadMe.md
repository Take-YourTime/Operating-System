# Programming Assignment 2

**Priority Scheduling**

**Student: B113040045 許育菖**

---

### **Basic lottery scheduling**

修改的檔案有：**system.c、proc.h**、**proc.c**

- **system.c**

首先先新增標頭檔 `#include <stdlib.h>`  

然後在函式`system_init()` 開頭的地方設置亂數種子

※注意此處不能使用`srand(time(NULL))` ，不然 OS reboot 時會卡住

```c
srandom( get_realtime() );
```

- **proc.h**

在struct proc中宣告一個變數用於紀錄每個 process 擁有的 tickets

```c
int p_tickets; // new: process擁有的彩票數量
```

- **proc.c**

新增標頭檔`#include <stdlib.h>` 

在函式`proc_init()` 中，設定每個 process 初始擁有的tickets數量為 5

```c
rp->p_tickets = 5;
```

修改函式`pick_proc()` ，將每個 process 的票數相加起來，然後使用`random()` 來隨機決定執行的 prcoess

```c
static struct proc * pick_proc(void)
{

/* Decide who to run now.  A new process is selected an returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
	register struct proc *rp;	/* process to run */
	struct proc **rdy_head;
	struct proc *chosen = NULL;  // 最終選中的process

	int q;	/* iterate over queues */
	int total_tickets = 0;  // 總票數
  int lottery, current_ticket = 0;

 /* Check each of the scheduling queues for ready processes. The number of
	* queues is defined in proc.h, and priorities are set in the task table.
	* If there are no processes ready to run, return NULL.
	*/
	rdy_head = get_cpulocal_var(run_q_head);
	for (q = 0; q < NR_SCHED_QUEUES; q++) {	
		for (rp = rdy_head[q]; rp != NULL; rp = rp->p_nextready) {
            if (proc_is_runnable(rp)) {
                total_tickets += rp->p_tickets;
            }
        }
	}

	// 如果没有就绪的process或彩票總數為0，return NULL
  if (total_tickets == 0)
	  return NULL;

	lottery = random() % total_tickets;

	// 根據彩票结果選擇process
    for (q = 0; q < NR_SCHED_QUEUES; q++) {
        for (rp = rdy_head[q]; rp != NULL; rp = rp->p_nextready) {
            if (proc_is_runnable(rp)) {
                current_ticket += rp->p_tickets;
                if (current_ticket > lottery) {
                    chosen = rp;
                    if (priv(rp)->s_flags & BILLABLE)
                        get_cpulocal_var(bill_ptr) = rp; // bill for system time
                    return chosen;
                }
            }
        }
    }

	return NULL;
}
```

---

### Lottery scheduling with dynamic priorities

延續Basic lottery scheduling中所修改的檔案，並在**prco.c**中的函式`switch_to_user()`新增程式碼。

當 process 因為剩餘時間（p_cpu_time_left = 0）而中斷時，使當前 process 的 tickets 數量增加 1 ，最多增加到 10。

而當 process 執行完成，且還有剩餘時間（p_cpu_time_left > 0），使當前 process 的 tickets 數量減少 1 ，最少減少到 1。

```c
if (!proc_is_runnable(p)) {
	if (p->p_cpu_time_left > 0 && p->p_tickets < 10)
		p->p_tickets += 1;
	else if(p->p_cpu_time_left <= 0 && p->p_tickets > 1)
		p->p_tickets -= 1;

	goto not_runnable_pick_new;
}
```

line: 357 ~ 364

line: 379 ~ 386

詳細程式碼：

[`switch_to_user()`](https://app.notion.com/p/switch_to_user-d0e3fe4fdb6d40e7b5e64e23abfa77bf?pvs=21)

---

### Profile your scheduler

在clock.c中宣告全域變數

```c
static int interrupt_count = 0; // 發生interrupt的次數
static int priority[11] = {0}; // 1~10紀錄每個優先級擁有的程序數量，0不會用到
const int INTERRUPT_COUNT_MAX = 100; // interrupt_count = INTERRUPT_COUNT_MAX時，output
```

並修改clock.c中的`timer_int_handler()`函式

在該函式一開頭的地方加上如下程式碼

```c
int q;	/* iterate over queues */
struct proc **rdy_head;
register struct proc *rp;	/* process to run */

//	紀錄interrupt發生當下，每個優先級擁有的process數量
rdy_head = get_cpulocal_var(run_q_head);
for (q = 0; q < NR_SCHED_QUEUES; q++)
	if(rdy_head[q])
		for (rp = rdy_head[q]; rp != NULL; rp = rp->p_nextready)
			priority[ (rp->p_tickets) ]++;

//	紀錄發生了多少次interrupt
interrupt_count++;
if(interrupt_count % INTERRUPT_COUNT_MAX == 0)
{
	printf("INTERRUPT_COUNT: %d ~ %d\n", interrupt_count - 100, interrupt_count);
	int i;
	for(i = 0; i <= 10; i++)
	{
		printf("\ttickets %d: %d\n", i, priority[i]);
		priority[i] = 0; // reset
	}
}
```

這段程式碼是用來走訪每個process，並把它當前的tickets數紀錄起來

每當interrupt發生500次時，會把這段時間內所記錄到的tickets數output在螢幕上

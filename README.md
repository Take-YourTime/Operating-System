# Operating System

> [English] | [繁體中文](README_zh-TW.md)

This repository contains the four programming assignments from the **Operating System** course. Below are the descriptions and implementation details for each assignment.

## HW1: Custom Simple Minix Shell Implementation

### Description

Implement a custom Shell capable of executing basic commands and system programs.

### Implementation Details

- Written in C (`shell.c`).
- Implemented built-in commands such as `exit`, `cd`, `pwd`, `history` (display a specified number of history records), `help`, as well as customized commands like `NSYSU` and `rabbit`.
- Supports executing other default commands in the Minix3 system.
- Capable of handling various Shell Operators, including: background execution (`&`), input/output redirection (`>`, `>>`, `<`), and pipeline (`|`).
- The core logic uses `fork()` to create Child Processes, and utilizes `exec` family functions to execute external commands; redirection and pipelining are achieved by manipulating File Descriptors and `pipe()`.

## HW2: Minix Kernel Modification & Process Scheduling

### Description

Modify the Minix 3 kernel source code to alter the Process priority and scheduling mechanism, and monitor process information.

### Implementation Details
 
- Achieved by modifying the `proc.c`, `proc.h`, `system.c`, and `clock.c` files in the Minix operating system kernel.
- The implementation is divided into three parts: modifying the system's scheduling algorithm, setting Process Priorities, and adding a timer function in `clock.c` to output the number of processes in each Priority Level periodically.
- Copy the modified code to the `/usr/src/minix/kernel` directory, recompile the kernel using `make clean`, `make kernel`, and `make install`, and then Reboot to apply the new kernel.
  
### WATCHOUT

This assignment was not perfectly completed to the requirements. Minix is just too tricky.

## HW3: Shared Memory and Process Management

### Description

Extend the Shell from HW1 by adding Shared Memory and advanced Child Process management features.

### Implementation Details

- Written in C (`shell.c`), retaining the basic features from HW1.
- When executing the Shell, arguments can be passed to determine the number of child processes to create (default is 1). If an undefined command is entered, it will be output multiple times simultaneously by these child processes.
- Implemented process management functions (such as `Waitpid`, `ProcessFork`, `Spawn`, `Exit`) to control the status of child processes.
- Utilized `sys/shm.h` and `sys/ipc.h` libraries to create and attach Shared Memory using `shmget` and `shmat`, allowing variables and data to be shared between the Parent Process and Child Processes.

## HW4: Disk Scheduling Algorithms

### Description

Implement and compare the performance of various Disk Scheduling Algorithms regarding head movement distance and latency.

### Implementation Details

- Written in C++ (`DiskSchedulingAlgorithms.cpp`), supporting common scheduling algorithms such as FCFS, SSTF, and SCAN.
- During execution, the program randomly generates a series of disk Cylinder Requests.
- It then prompts the user to input the initial position of the disk head (between 0 and 4999).
- In the algorithm logic, C++'s `vector` is used to store Requests, and STL functions like `min_element` are used to find the next target Cylinder.
- The program calculates the "Total Head Movement" and "Latency" for each algorithm.
- You can input multiple initial positions for testing. When `-1` is entered to terminate, it will automatically calculate and output the average latency of all test cases.

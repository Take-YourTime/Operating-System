# Operating-System

O.S. course data in NSYSU (National Sun Yat-sen University).

This repository contains four programming assignments from the Operating System course. Each assignment focuses on a different core OS concept implemented on Minix3 or as a standalone C program.

---

## pro_HW1 — Custom Shell (Basic)

**Files:** `shell.c`, `testfile.txt`, `Programming Assignment 1.pdf`, `ReadMe.txt`

A Unix-like shell implemented in C, designed to run on Minix3. The shell supports the following built-in commands:

| Command | Description |
|---------|-------------|
| `exit` | Terminate the shell |
| `cd` | Change the current directory |
| `pwd` | Print the current working directory |
| `history [n]` | Display the last *n* commands used (default: all) |
| `help` | Display the list of supported commands |
| `NSYSU` | Print an introduction to NSYSU |
| `rabbit` | Display an ASCII-art rabbit |

In addition to built-in commands, the shell supports standard Minix3 commands and the following shell operators:

- `&` — Run a command in the background
- `>` — Redirect stdout to a file (overwrite)
- `>>` — Redirect stdout to a file (append)
- `|` — Pipe the output of one command into another

---

## pro_HW2 — Kernel Process Scheduling (Minix3)

**Files:** `proc.c`, `proc.h`, `system.c`, `clock.c` (inside sub-folders `1/` and `2/`), `Programming Assignment 2.pdf`, `DesignDocument.pdf`, `ReadMe.txt`

This assignment involves modifying the Minix3 kernel to experiment with process scheduling and priority management. There are three sub-problems:

- **Problem 1 (`1/`):** Modify the kernel scheduler (`proc.c`, `proc.h`, `system.c`) to observe process priorities. After rebooting, use `top` to examine system performance.
- **Problem 2 (`2/`):** Apply an alternative `proc.c` with different scheduling behaviour, rebuild the kernel, and compare performance with Problem 1.
- **Problem 3:** Combines the kernel patches from either sub-problem with a modified `clock.c` that periodically prints the number of processes at each priority level to the screen. The system is accessed via SSH remote connection.

To apply any sub-problem, copy the relevant kernel source files to `/usr/src/minix/kernel`, rebuild and reinstall the kernel, then reboot and select option 2 at startup.

---

## pro_HW3 — Custom Shell with Shared Memory

**Files:** `shell.c`, `Programming Assignment 3 v3.pdf`, `DesignDocument.pdf`, `ReadMe.txt`

An extended version of the HW1 shell that adds **multi-process** and **shared memory** support. All original HW1 built-in commands and shell operators are retained.

New behaviour:
- When the shell is started with a numeric argument (e.g., `./shell 3`), it spawns that many child processes.
- Any unrecognised command is written to **shared memory** and repeated across all child processes, demonstrating inter-process communication via shared memory.

**Build & run:**
```bash
clang shell.c -o shell
./shell 3   # spawn 3 child processes
```

---

## pro_HW4 — Disk Scheduling Algorithms

**Files:** `DiskSchedulingAlgorithms.cpp`, `out.txt`, `Programming Assignment 4 (mod) v2.pdf`, `DesignDocument.pdf`, `ReadMe.txt`

A C++ (C++17) simulation of classic disk scheduling algorithms on a disk with 5,000 cylinders and 1,000 random I/O requests. The program:

1. Displays all randomly generated request positions (cylinder number and location).
2. Prompts the user to enter the initial head position (0–4999).
3. Outputs the **total head movement** (number of cylinders traversed) and **average latency** (ms) for each scheduling algorithm.
4. Accepts `-1` as input to terminate; upon exit, prints the average latency across all test cases.

Scheduling algorithms implemented:

| Algorithm | Description |
|-----------|-------------|
| **FCFS** | First-Come, First-Served — processes requests in arrival order |
| **SSTF** | Shortest Seek Time First — services the nearest cylinder request first |
| **SCAN** | Moves the head in one direction, servicing requests, then reverses at the disk end |
| **C-SCAN** | Like SCAN, but the head returns to the start after reaching the far end (circular) |
| **LOOK** | Like SCAN, but reverses at the furthest pending request instead of the disk end |
| **C-LOOK** | Like C-SCAN, but jumps back to the nearest pending request instead of the disk start |

Sample output is provided in `out.txt`.

> **Note:** Compile and run with Visual Studio or a compatible C++17 compiler. Avoid DEV-C++.

**Build example (Visual Studio command line):**
```
cl /std:c++17 DiskSchedulingAlgorithms.cpp
```

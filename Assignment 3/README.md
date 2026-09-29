# Assignment 3 — CPU Scheduling

## Aim

Write a menu-driven program to simulate the behavior of the following CPU scheduling algorithms:

1. FCFS (First Come First Serve)
2. SJF (Shortest Job First) — Preemptive
3. Priority Scheduling — Non-Preemptive
4. Round Robin — Preemptive

---

## Problem Statement

Implement a menu-driven C++ program that accepts process details and simulates different CPU scheduling algorithms.

The program calculates and displays:

* Completion Time (CT)
* Turnaround Time (TAT)
* Waiting Time (WT)
* Response Time (RT)
* Average Waiting Time
* Average Turnaround Time
* Average Response Time
* Gantt Chart

For Priority Scheduling, a smaller priority number represents a higher priority.

For Round Robin Scheduling, the program accepts a user-defined time quantum.

---

## Algorithms Implemented

### 1. FCFS — First Come First Serve

FCFS schedules processes according to their arrival time.

* Non-preemptive
* The process that arrives first gets executed first.
* Once a process starts execution, it runs until completion.

### 2. SJF — Preemptive

The preemptive version of SJF is also known as **Shortest Remaining Time First (SRTF)**.

* Preemptive
* The process with the shortest remaining burst time is selected.
* A running process can be preempted when a new process with a shorter remaining time arrives.

### 3. Priority Scheduling — Non-Preemptive

Processes are scheduled according to their priority.

* Non-preemptive
* Smaller priority number indicates higher priority.
* Once a process starts execution, it continues until completion.

### 4. Round Robin — Preemptive

Round Robin schedules processes using a fixed time quantum.

* Preemptive
* Each process gets CPU time for at most one time quantum.
* If a process does not finish within the quantum, it is placed at the end of the ready queue.

---

## Input

For every process, the program accepts:

* Arrival Time
* Burst Time

Priority is requested only when **Priority Scheduling** is selected.

Time Quantum is requested only when **Round Robin** is selected.

---

## Output

For each selected scheduling algorithm, the program displays a table containing:

| Field           | Description                         |
| --------------- | ----------------------------------- |
| PID             | Process ID                          |
| Arrival Time    | Time at which the process arrives   |
| Burst Time      | CPU execution time required         |
| Completion Time | Time at which the process completes |
| Turnaround Time | Completion Time − Arrival Time      |
| Waiting Time    | Turnaround Time − Burst Time        |
| Response Time   | First CPU Start Time − Arrival Time |

The program also displays the average:

* Waiting Time
* Turnaround Time
* Response Time

along with the corresponding Gantt Chart.

---

## Project Structure

```text
Assignment 3/
│
├── cpu_scheduling.cpp
└── README.md
```

---

## Compilation and Execution

### Windows

Open Command Prompt or PowerShell in the Assignment 3 directory.

```cmd
g++ cpu_scheduling.cpp -o cpu_scheduling
```

Run the program:

```cmd
cpu_scheduling
```

Alternatively:

```cmd
g++ cpu_scheduling.cpp -o cpu_scheduling && cpu_scheduling
```

---

## Sample Input

```text
Enter number of processes: 4

Process P1:
Arrival Time : 0
Burst Time   : 5

Process P2:
Arrival Time : 1
Burst Time   : 3

Process P3:
Arrival Time : 2
Burst Time   : 8

Process P4:
Arrival Time : 3
Burst Time   : 6
```

### Priority Scheduling

```text
Priority for P1: 2
Priority for P2: 1
Priority for P3: 4
Priority for P4: 3
```

Here, a smaller priority number represents a higher priority.

### Round Robin

```text
Enter Time Quantum: 2
```

---

## Sample Results

### FCFS

```text
Average Waiting Time    : 5.75
Average Turnaround Time : 11.25
Average Response Time   : 5.75
```

### SJF (Preemptive)

```text
Average Waiting Time    : 5.00
Average Turnaround Time : 10.50
Average Response Time   : 4.25
```

### Priority (Non-Preemptive)

```text
Average Waiting Time    : 5.25
Average Turnaround Time : 10.75
Average Response Time   : 5.25
```

### Round Robin

```text
Average Waiting Time    : 9.75
Average Turnaround Time : 15.25
Average Response Time   : 2.00
```

---

## Menu

The program provides the following menu:

```text
--------------- MENU ----------------
1. FCFS
2. SJF (Preemptive)
3. Priority (Non-Preemptive)
4. Round Robin (Preemptive)
5. Exit
-------------------------------------
```

The same set of processes can be used to compare the behavior and performance of all four scheduling algorithms.

---

## Key Concepts

* CPU Scheduling
* Process Scheduling
* Arrival Time
* Burst Time
* Completion Time
* Turnaround Time
* Waiting Time
* Response Time
* Preemptive Scheduling
* Non-Preemptive Scheduling
* Ready Queue
* Time Quantum
* Gantt Chart

---

## Technologies Used

* **Language:** C++
* **Compiler:** g++
* **Platform:** Windows
* **Concept:** Operating Systems — CPU Scheduling

---

## Learning Outcome

This assignment demonstrates the implementation and comparison of commonly used CPU scheduling algorithms. It provides practical understanding of process scheduling, preemption, ready queues, scheduling metrics, and Gantt chart representation.

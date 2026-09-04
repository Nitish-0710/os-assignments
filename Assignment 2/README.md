# Operating Systems – Assignment 2

## Process Control

This assignment demonstrates fundamental **process control mechanisms in Linux using C programming**. It covers process creation, process states, process hierarchy, process synchronization, and program execution using system calls such as `fork()`, `wait()`, and `exec()`.

---

## Aim

To study and implement process control mechanisms in Linux using C programming, including process creation using `fork()`, zombie and orphan processes, multi-level process trees, and program execution using `exec()`.

---

## Objectives

* To understand process creation using the `fork()` system call.
* To understand parent-child process relationships.
* To demonstrate zombie and orphan processes.
* To generate a multi-level process tree.
* To understand program execution using `exec()`.
* To combine `fork()` and `exec()` for process creation and execution.
* To understand process synchronization using `wait()`.
* To implement process-control operations using a menu-driven C program.

---

## Concepts Covered

### 1. Process Creation using `fork()`

The `fork()` system call creates a new child process from an existing parent process.

```c
pid_t pid = fork();
```

The return value of `fork()` is used to distinguish between the parent and child processes.

---

### 2. Zombie Process

A **zombie process** is a child process that has terminated but still has an entry in the process table because its parent has not yet collected its exit status using `wait()`.

The zombie state can be observed using:

```bash
ps -el
```

A process in state `Z` represents a zombie process.

---

### 3. Orphan Process

An **orphan process** is a child process whose parent terminates before the child.

The parent process ID can be obtained using:

```c
getppid()
```

After the original parent terminates, the child is adopted by another system process and its PPID changes.

---

### 4. Multi-Level Process Tree

The assignment generates a multi-level process hierarchy using multiple `fork()` calls.

```text
                 Root Process
                /            \
           Child 1          Child 2
              |                |
       Grandchild 1      Grandchild 2
```

This demonstrates multiple levels of parent-child process relationships.

---

### 5. Program Execution using `exec()`

The `exec()` family of system calls replaces the currently executing process with another program.

This assignment uses:

```c
execl("/bin/ls", "ls", "-l", NULL);
```

to execute the Linux `ls -l` command.

---

### 6. `fork()` + `exec()`

The assignment demonstrates the commonly used combination of `fork()` and `exec()`.

```text
Parent Process
       |
     fork()
       |
   +---+---+
   |       |
Parent    Child
            |
          exec()
            |
       New Program
```

The parent creates a child using `fork()`, and the child executes another program using `exec()`.

---

## Menu-Driven Program

The main program provides the following menu:

```text
========================================
       PROCESS CONTROL MENU
========================================
1. Process Creation using fork()
2. Zombie Process
3. Orphan Process
4. Multi-Level Process Tree
5. Program Execution using exec()
6. fork() + exec()
7. Exit
========================================
Enter your choice:
```

---

## Programs

| File                | Description                                                  |
| ------------------- | ------------------------------------------------------------ |
| `process_control.c` | Menu-driven implementation of all process-control operations |
| `fork.c`            | Demonstrates process creation using `fork()`                 |
| `zombie.c`          | Demonstrates creation of a zombie process                    |
| `orphan.c`          | Demonstrates creation of an orphan process                   |
| `process_tree.c`    | Generates a multi-level process tree                         |
| `exec_demo.c`       | Demonstrates program execution using `exec()`                |
| `fork_exec.c`       | Demonstrates the combination of `fork()` and `exec()`        |

---

## Compilation and Execution

### Menu-Driven Program

Compile:

```bash
gcc process_control.c -o bin/process_control
```

Run:

```bash
./bin/process_control
```

### Individual Programs

```bash
gcc fork.c -o bin/fork
gcc zombie.c -o bin/zombie
gcc orphan.c -o bin/orphan
gcc process_tree.c -o bin/process_tree
gcc exec_demo.c -o bin/exec_demo
gcc fork_exec.c -o bin/fork_exec
```

---

## Process Synchronization

The `wait()` system call is used for synchronization between parent and child processes.

```c
wait(NULL);
```

It allows the parent process to wait for the termination of its child process and helps maintain proper process execution order.

---

## Process Hierarchy

The multi-level process tree demonstrates the following hierarchy:

```text
                    Root
                  /      \
              Child 1   Child 2
                |          |
          Grandchild 1  Grandchild 2
```

Each process displays its PID and PPID to verify the process relationships.

---

## Process Monitoring

The following Linux commands can be used to observe processes:

```bash
ps -el
```

```bash
pstree -p
```

For example, a zombie process can be identified by its process state:

```text
Z
```

where `Z` indicates a zombie process.

---

## Verification

The following operations were successfully implemented and tested:

* Process creation using `fork()`
* Zombie process creation and verification
* Orphan process creation and PPID change
* Multi-level process tree generation
* Program execution using `exec()`
* Combined `fork()` and `exec()`
* Parent-child synchronization using `wait()`
* Menu-driven process control

---

## Conclusion

This assignment provides practical understanding of **process management and control in Linux**. It demonstrates how processes are created using `fork()`, synchronized using `wait()`, and replaced or executed using `exec()`.

The implementation also demonstrates important process states such as **zombie and orphan processes**, along with the creation of a **multi-level process hierarchy**.

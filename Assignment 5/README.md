# Assignment 5 — Inter-Process Communication (IPC)

## 1. Objective

To implement and demonstrate different Inter-Process Communication (IPC) mechanisms provided by Linux using C and POSIX/Linux system calls.

## 2. Problem Statement

Develop a menu-driven C program to demonstrate the following IPC mechanisms:

1. Anonymous pipe communication.
2. Bidirectional communication using two pipes.
3. File transfer using pipes.
4. Named pipes (FIFO).
5. Client-server communication using FIFO.
6. FIFO-based chat application.
7. Producer-consumer using message queues.
8. Message exchange using System V message queues.
9. Sharing data between processes using shared memory.
10. Performing arithmetic operations using shared memory.
11. Copying a file using shared memory.

## 3. Tools and Technologies

- Operating System: Ubuntu on WSL 2
- Programming Language: C
- Compiler: GCC
- IPC mechanisms: Anonymous pipes, named pipes, System V message queues, and shared memory

## 4. Directory Structure

```text
Assignment 5/
├── ipc.c
├── README.md
└── test.txt
```

The `test.txt` file serves as the source file for file-transfer and shared-memory file-copy demonstrations.

## 5. Theory

### Anonymous Pipes

An anonymous pipe provides a unidirectional communication channel between related processes. The `pipe()` system call creates a pair of file descriptors: one for reading and one for writing.

The `fork()` system call creates a child process that inherits the pipe descriptors. The processes communicate using `read()` and `write()`.

### Bidirectional Communication Using Two Pipes

A single anonymous pipe is unidirectional. Two pipes can be used to support communication in both directions between a parent and child process.

One pipe carries data from the parent to the child, while the second carries the response from the child to the parent.

### File Transfer Using Pipes

A pipe can transfer file contents between processes. The source process reads data from a file and writes it into a pipe. The receiving process reads from the pipe and writes the received data into a destination file.

### Named Pipes (FIFO)

A named pipe, or FIFO, is a special filesystem object that allows processes to communicate through a named path. Unlike anonymous pipes, a FIFO can be accessed by independently created processes that have permission to open it.

The `mkfifo()` system call creates a FIFO, while `open()`, `read()`, and `write()` perform communication.

### Client-Server Communication Using FIFO

A client and server can exchange messages through two FIFOs. One FIFO carries the client request, and the other carries the server response.

The demonstration creates the FIFOs, starts a child process to act as the server, exchanges a message, and removes the FIFO objects afterward.

### FIFO-Based Chat Application

The FIFO-based chat demonstration allows a client to send a message to a server and receive a response through named pipes.

The client and server operate as separate processes, demonstrating message exchange through FIFO communication.

### System V Message Queues

System V message queues allow processes to exchange messages through a kernel-managed queue.

Important system calls include:

- `msgget()` — creates or accesses a message queue.
- `msgsnd()` — sends a message to the queue.
- `msgrcv()` — receives a message from the queue.
- `msgctl()` — controls or removes the queue.

### Producer-Consumer Using Message Queues

The producer sends an item to a message queue. The consumer receives the item from the queue and displays it.

The queue provides a communication mechanism between the producer and consumer processes.

### Shared Memory

Shared memory allows multiple processes to access the same memory region. It is useful for transferring data without repeatedly copying the data through a pipe or message queue.

The System V shared-memory system calls used in the program include:

- `shmget()` — creates or obtains a shared-memory segment.
- `shmat()` — attaches the segment to a process.
- `shmdt()` — detaches the segment.
- `shmctl()` — controls or removes the segment.

### Arithmetic Operations Using Shared Memory

The client places two integer operands and an arithmetic operator in shared memory. The server process reads the operands, performs the requested calculation, and stores the result in the shared region.

A pipe is used to coordinate the client and server processes.

### Shared-Memory File Copy

The source process reads data from a file into a shared-memory buffer. The receiving process reads the buffer and writes its contents into the destination file.

Pipes are used to coordinate data availability and buffer consumption.

## 6. Algorithm

1. Start the program and display the IPC menu.
2. Read the user's choice.
3. Execute the selected IPC demonstration.
4. Create the necessary pipes, FIFOs, message queues, or shared-memory segments.
5. Create child processes when required.
6. Exchange data using the selected IPC mechanism.
7. Synchronize processes and collect their results.
8. Close file descriptors and release IPC resources.
9. Return to the menu after the operation.
10. Terminate when the user selects Exit.

## 7. Compilation and Execution

Run these commands from the `Assignment 5` directory in WSL Ubuntu.

### Compile

```bash
gcc ipc.c -o bin/ipc
```

### Execute

```bash
./bin/ipc
```

The executable is generated in the `bin/` directory.

## 8. Test Cases and Verified Results

The menu-driven program was executed in WSL Ubuntu. All eleven menu options were exercised during the recorded test session.

### Test 1: Anonymous Pipe Communication

Input:

```text
1
```

Verified output:

```text
Parent sent: Hello from the parent process!
Child received: Hello from the parent process!
```

### Test 2: Bidirectional Communication Using Two Pipes

Input:

```text
2
```

Verified output:

```text
Parent sent: Hello child!
Child received: Hello child!
Parent received: Hello parent!
```

### Test 3: File Transfer Using Pipes

Input:

```text
3
test.txt
pipe_copy.txt
```

Verified output:

```text
File transfer through the pipe completed.
Destination: pipe_copy.txt
```

Verification command:

```bash
diff -s test.txt pipe_copy.txt
```

Verified result:

```text
Files test.txt and pipe_copy.txt are identical
```

### Test 4: Named Pipes (FIFO)

Input:

```text
4
```

Verified output:

```text
Writer sent: Message sent through a named FIFO.
Reader received: Message sent through a named FIFO.
```

### Test 5: Client-Server Communication Using FIFO

Input:

```text
5
Hello FIFO server
```

Verified output:

```text
Client received: Server processed: Hello FIFO server
```

### Test 6: FIFO-Based Chat Application

Input:

```text
6
Hello from client
```

Verified output:

```text
Server received: Hello from client
Server: Acknowledged: Hello from client
```

### Test 7: Producer-Consumer Using Message Queues

Input:

```text
7
```

Verified output:

```text
Producer sent: Item produced by the producer.
Consumer received: Item produced by the producer.
```

### Test 8: System V Message Queue Communication

Input:

```text
8
```

Verified output:

```text
Parent sent: Request from parent process.
Child received: Request from parent process.
Parent received: Reply from child process.
```

### Test 9: Shared Data Communication

Input:

```text
9
```

Verified output:

```text
Parent wrote: Data written by parent into shared memory.
Child read from shared memory: Data written by parent into shared memory.
```

### Test 10: Arithmetic Operations Using Shared Memory

Input:

```text
10
20
10
+
```

Verified output:

```text
Client placed operands in shared memory.
Server completed the arithmetic operation.
Result: 20 + 10 = 30
```

### Test 11: Shared-Memory File Copy

Input:

```text
11
test.txt
shared_copy.txt
```

Verified output:

```text
Shared-memory file copy completed.
Destination: shared_copy.txt
```

Verification command:

```bash
diff -s test.txt shared_copy.txt
```

Verified result:

```text
Files test.txt and shared_copy.txt are identical
```

### Test 12: Exit

Input:

```text
0
```

Verified output:

```text
Exiting Assignment 5.
```

## 9. Observations

- Anonymous pipes support communication between related processes.
- Two pipes can provide bidirectional communication.
- Pipes can transfer file contents between processes.
- FIFOs provide named communication channels between processes.
- System V message queues allow processes to exchange typed messages.
- Shared memory enables processes to access a common memory region.
- Process synchronization is necessary to coordinate shared-memory access.
- IPC resources should be released after use to avoid unnecessary resource consumption.

## 10. Conclusion

The assignment demonstrates Inter-Process Communication in C using Linux system calls. Anonymous pipes, bidirectional pipes, file transfer, FIFOs, System V message queues, and shared memory were implemented through a menu-driven program.

The recorded execution covered all eleven menu options, and the generated file copies were verified against the source file using `diff`.

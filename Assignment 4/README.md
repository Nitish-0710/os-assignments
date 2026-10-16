# Assignment 4 — Multithreaded Programming Using Pthreads

## 1. Objective

To implement multithreaded programming in C using POSIX threads (pthreads) and demonstrate parallel execution through array summation, matrix addition, matrix multiplication, and file processing.

## 2. Problem Statement

Develop a menu-driven C program that uses multiple threads to:

1. Calculate the sum of array elements in parallel.
2. Add two matrices using multiple threads.
3. Multiply two matrices using multiple threads.
4. Count characters, words, and lines in a file using separate threads.

## 3. Tools and Technologies

- Operating System: Ubuntu on WSL 2
- Programming Language: C
- Compiler: GCC
- Threading Library: POSIX Threads (pthreads)

## 4. Directory Structure

    Assignment 4/
    ├── multithreading.c
    ├── README.md
    └── test.txt

The file `test.txt` may be created locally for file-processing tests.

## 5. Theory

### POSIX Threads

POSIX threads provide an interface for creating and managing multiple threads within a process. Threads share the process address space but can execute different functions concurrently.

Important functions:

- `pthread_create()` creates a new thread.
- `pthread_join()` waits for a thread to finish.

### Parallel Sum of Array Elements

The array is divided into approximately equal portions. Each thread calculates the sum of its assigned portion. The main thread waits for the worker threads and combines their partial sums.

### Matrix Addition

Each thread processes a range of rows. Corresponding elements of the two input matrices are added to produce the result matrix.

### Matrix Multiplication

Each thread calculates a range of rows in the result matrix. Each result element is obtained by multiplying a row of the first matrix by a column of the second matrix and summing the products.

The number of columns in the first matrix must equal the number of rows in the second matrix.

### Parallel File Processing

Three independent threads open and scan the same file:

- Character-count thread: counts bytes read from the file.
- Word-count thread: counts whitespace-separated words.
- Line-count thread: counts lines, including a final non-empty line without a terminating newline.

The main thread joins the workers and displays their results.

## 6. Algorithm

1. Start the program and display the menu.
2. Read the user's choice.
3. For array summation, distribute array elements among the requested threads and combine the partial sums.
4. For matrix addition, distribute rows among threads and add corresponding elements.
5. For matrix multiplication, validate matrix dimensions and distribute result rows among threads.
6. For file processing, create separate threads for character, word, and line counting.
7. Join the worker threads and display the results.
8. Return to the menu after each operation.
9. Terminate when the user selects Exit.

## 7. Compilation and Execution

Run these commands from the `Assignment 4` directory in WSL Ubuntu.

Compile:

    gcc multithreading.c -o multithreading

Execute:

    ./multithreading

## 8. Test Cases and Verified Results

### Test 1: Parallel Sum of Array Elements

Input:

- Array size: 6
- Array elements: 1, 2, 3, 4, 5, 6
- Number of threads: 3

Verified partial sums:

| Thread | Array range (end exclusive) | Partial sum |
| ------ | --------------------------- | ----------: |
| 1      | [0, 2)                      |           3 |
| 2      | [2, 4)                      |           7 |
| 3      | [4, 6)                      |          11 |

Total sum: **21**

### Test 2: Matrix Addition

Matrix A:

```text
    1  2
    3  4
```

Matrix B:

```text
    5  6
    7  8
```

Number of threads: `2`

Verified result:

```text
    6   8
    10  12
```

### Test 3: Matrix Multiplication

Matrix A:

```text
    1  2
    3  4
```

Matrix B:

```text
    5  6
    7  8
```

Number of threads: 2

Verified result:

```text
    19  22
    43  50
```

### Test 4: Parallel File Processing

Test file content consists of two lines:

    Hello world
    Pthreads are useful

Verified results:

- Lines: 2
- Words: 5
- Bytes: 32

## 9. Observations

- Multiple threads can process different portions of an array or matrix.
- `pthread_join()` ensures that the main thread waits for worker threads before collecting results.
- Matrix multiplication requires compatible matrix dimensions.
- Separate file-processing threads can calculate different statistics from the same file concurrently.
- The menu allows multiple operations during one program run.

## 10. Conclusion

The assignment demonstrates POSIX multithreading in C through a menu-driven program. Parallel array summation, matrix addition, matrix multiplication, and concurrent file-statistics calculation were implemented and tested in WSL Ubuntu using GCC and pthreads.

The results demonstrate thread creation, workload distribution, synchronization, and aggregation of computed results.

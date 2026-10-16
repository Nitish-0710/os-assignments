# Assignment 6 — Deadlock Handling Using Banker's Algorithm

## 1. Objective

To implement and simulate Banker's Algorithm in C to demonstrate deadlock avoidance using the Safety Algorithm and Resource Request Algorithm.

## 2. Problem Statement

Develop a menu-driven C program to implement the following algorithms:

1. Safety Algorithm to determine whether the system is in a safe state.
2. Resource Request Algorithm to determine whether a process's resource request can be granted without making the system unsafe.

The program should calculate the Need matrix, identify a safe sequence when one exists, validate resource requests, and reject requests that exceed the process's remaining need, exceed currently available resources, or result in an unsafe state.

## 3. Tools and Technologies

- Operating System: Ubuntu on WSL 2
- Programming Language: C
- Compiler: GCC
- Standard: C11
- Algorithm: Banker's Algorithm
- Maximum supported processes: 20
- Maximum supported resource types: 20

## 4. Directory Structure

```text
Assignment 6/
├── bankers_algorithm.c
└── README.md
```

## 5. Theory

### Banker's Algorithm

Banker's Algorithm is a deadlock-avoidance algorithm that determines whether resource allocations can leave a system in a safe state.

A safe state is one in which the system can find an order of process completion such that every process can obtain its remaining required resources, finish execution, and release its allocated resources.

### Data Structures

The program uses the following data structures:

- **Allocation Matrix:** Represents the resources currently allocated to each process.
- **Maximum Matrix:** Represents the maximum resource requirement declared by each process.
- **Need Matrix:** Represents the remaining resources required by each process.
- **Available Vector:** Represents the number of currently available instances of each resource type.
- **Work Vector:** A temporary vector used by the Safety Algorithm to simulate resource availability.
- **Finish Array:** Tracks whether each process can finish during the safety check.
- **Safe Sequence:** Stores the order in which processes can complete safely.

### Need Matrix

The Need matrix is calculated using:

`Need[i][j] = Maximum[i][j] - Allocation[i][j]`

Here, `i` represents a process and `j` represents a resource type.

### Safety Algorithm

The Safety Algorithm determines whether the current resource-allocation state is safe.

1. Initialize `Work` with the `Available` vector.
2. Mark every process as unfinished.
3. Find an unfinished process whose Need is less than or equal to Work for every resource type.
4. If such a process exists, simulate its completion and release its allocated resources by adding its Allocation row to Work.
5. Mark the process as finished and append it to the safe sequence.
6. Repeat until every process finishes or no further process can finish.
7. If all processes finish, the system is safe. Otherwise, the system is unsafe.

The comparison is performed element by element:

`Need[i][j] <= Work[j]`

for every resource type `j`.

### Resource Request Algorithm

The Resource Request Algorithm checks whether a process's resource request can be granted safely.

For a requesting process `Pi`, the program performs the following checks:

1. Verify that the request does not exceed the process's remaining Need.
2. Verify that the request does not exceed the currently Available resources.
3. Temporarily allocate the requested resources and update the Available vector.
4. Execute the Safety Algorithm on the tentative state.
5. If the resulting state is safe, grant the request.
6. If the resulting state is unsafe, restore the original state and reject the request.

The request must satisfy:

`Request[i][j] <= Need[i][j]`

and

`Request[i][j] <= Available[j]`

for every resource type `j`.

## 6. Algorithm

### Safety Algorithm

1. Start the program and select the Safety Algorithm.
2. Read the number of processes and resource types.
3. Read the Allocation matrix, Maximum matrix, and Available vector.
4. Calculate the Need matrix.
5. Initialize Work using Available and mark all processes unfinished.
6. Find an unfinished process whose Need can be satisfied by Work.
7. Simulate its completion and release its allocated resources.
8. Record the process in the safe sequence.
9. Repeat until all processes finish or no eligible process remains.
10. Display the safe sequence if the system is safe; otherwise, report an unsafe state.

### Resource Request Algorithm

1. Select the Resource Request Algorithm.
2. Read the system's resource-allocation data.
3. Read the requesting process number and its request vector.
4. Reject the request if it exceeds the process's remaining Need.
5. Reject the request if it exceeds Available resources.
6. Save the original state and tentatively allocate the requested resources.
7. Execute the Safety Algorithm on the tentative state.
8. If the state is safe, retain the allocation and grant the request.
9. If the state is unsafe, restore the original state and deny the request.
10. Return to the main menu.

## 7. Compilation and Execution

Run the following commands from the `Assignment 6` directory in WSL Ubuntu.

### Compile

```bash
gcc bankers_algorithm.c -o bankers_algorithm
```

### Execute

```bash
./bankers_algorithm
```

### Menu Options

```text
========================================
       BANKER'S ALGORITHM SIMULATOR
========================================
1. Safety Algorithm
2. Resource Request Algorithm
0. Exit
Enter your choice:
```

## 8. Test Cases and Results

### Test 1: Safety Algorithm — Safe State

**Input**

Number of processes: `5`

Number of resource types: `3`

Allocation Matrix:

```text
0 1 0
2 0 0
3 0 2
2 1 1
0 0 2
```

Maximum Matrix:

```text
7 5 3
3 2 2
9 0 2
2 2 2
4 3 3
```

Available Vector:

```text
3 3 2
```

**Verified Need Matrix**

```text
P0: 7 4 3
P1: 1 2 2
P2: 6 0 0
P3: 0 1 1
P4: 4 3 1
```

**Verified Output**

```text
System is in a SAFE state.
Safe sequence: P1 -> P3 -> P4 -> P0 -> P2
```

**Observation**

All five processes can finish in the generated safe sequence. The system is therefore in a safe state.

### Test 2: Resource Request Algorithm — Request Granted

**Input**

Use the same Allocation matrix, Maximum matrix, and Available vector as Test 1.

Requesting process: `P1`

Request vector:

```text
1 0 2
```

**Verified Output**

```text
Request tentatively allocated to P1.

System is in a SAFE state.
Safe sequence: P1 -> P3 -> P4 -> P0 -> P2
Request GRANTED. The system remains safe.
Updated Available Vector: 2 3 0
```

**Observation**

The request does not exceed P1's remaining Need or the Available resources. The tentative allocation preserves a safe state, so the request is granted.

### Test 3: Resource Request Algorithm — Insufficient Available Resources

**Input**

Use the same initial system state as Test 1.

Requesting process: `P0`

Request vector:

```text
0 4 0
```

**Expected Output**

```text
Request cannot be granted now: insufficient available resources.
```

**Observation**

The request asks for four instances of R1, while only three instances are available. The request must be rejected without changing the system state.

## 9. Observations

- The Need matrix is calculated by subtracting Allocation from Maximum.
- The Safety Algorithm identifies whether all processes can complete in a safe order.
- A safe sequence demonstrates a possible order of process completion without entering an unsafe state.
- The Resource Request Algorithm validates requests against both remaining Need and Available resources.
- A request that exceeds the process's remaining Need is rejected.
- A request that exceeds currently Available resources cannot be granted immediately.
- Tentative allocations must be checked using the Safety Algorithm before being committed.
- An unsafe tentative allocation must be rolled back to preserve the original state.

## 10. Conclusion

Banker's Algorithm was implemented in C as a menu-driven program to demonstrate deadlock avoidance.

The Safety Algorithm was tested using a five-process, three-resource system and successfully generated the safe sequence `P1 -> P3 -> P4 -> P0 -> P2`.

The Resource Request Algorithm was tested with a request from P1 for resources `1 0 2`. The request was granted because the resulting state remained safe, and the Available vector was updated to `2 3 0`.

An additional test case demonstrates the rejection of a request that exceeds the currently available resources. The implementation illustrates how Banker's Algorithm evaluates system safety before granting resource requests.

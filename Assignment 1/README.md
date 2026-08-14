# Assignment 1 – Shell Programming and AWK

## Objective

To understand and implement basic **Shell Programming** and **AWK programming** concepts by creating a simple student database system.

The program provides a menu-driven interface to perform common database operations such as creating, viewing, inserting, deleting, modifying, and displaying the result of a particular student.

---

## Problem Statement

Implement a student database using **Shell and AWK programming** to perform the following operations:

1. Create Database
2. View Database
3. Insert a Record
4. Delete a Record
5. Modify a Record
6. Display Result of a Particular Student
7. Exit

---

## Technologies Used

* Bash Shell
* AWK
* Linux / Ubuntu / WSL
* Text File as Database

---

## Database Format

The student database is stored in a text file named:

```text
student.txt
```

Each record contains three fields:

```text
Roll_Number Name Marks
```

Example:

```text
101 Rahul 85
102 Sneha 92
103 Amit 78
104 Neha 88
```

The fields are separated using spaces.

---

## Features

### 1. Create Database

Creates the student database file.

The database is initialized as an empty file.

### 2. View Database

Displays all student records stored in the database.

Example:

```text
Roll   Name   Marks
--------------------
101    Rahul   85
102    Sneha   92
103    Amit    78
104    Neha    88
```

### 3. Insert Record

Allows the user to enter:

* Roll Number
* Student Name
* Marks

The record is appended to the database.

Example:

```text
Enter Roll Number: 105
Enter Name: Karan
Enter Marks: 90
```

The record is added as:

```text
105 Karan 90
```

### 4. Delete Record

Deletes a student record using the student's roll number.

AWK is used to filter out the selected record.

Example:

```text
Enter Roll Number to Delete: 102
```

The record having roll number `102` is removed.

### 5. Modify Record

Allows the user to modify the name and marks of an existing student.

Example:

```text
Enter Roll Number to Modify: 103
Enter New Name: Amit
Enter New Marks: 82
```

The corresponding record is updated.

### 6. Display Result

Displays the details and result of a particular student.

The program considers:

```text
Marks >= 40 → Pass
Marks < 40  → Fail
```

Example:

```text
Roll Number : 101
Name         : Rahul
Marks        : 85
Result       : Pass
```

### 7. Exit

Terminates the program.

---

## AWK Concepts Used

The assignment demonstrates several basic AWK concepts.

### Field Access

```bash
$1
$2
$3
```

These represent:

* `$1` → Roll Number
* `$2` → Name
* `$3` → Marks

### Filtering Records

```bash
awk '$3 > 80 {print $2,$3}' student.txt
```

This displays students whose marks are greater than 80.

### Calculating Average

```bash
awk '{sum += $3} END {print sum/NR}' student.txt
```

Here:

* `sum` stores the total marks.
* `$3` represents marks.
* `NR` represents the number of records.

### Passing Variables to AWK

```bash
awk -v r="$roll" '$1 == r {print}' student.txt
```

The `-v` option passes a Shell variable to AWK.

---

## How to Run

### 1. Clone the Repository

```bash
git clone https://github.com/Nitish-0710/os-assignments.git
````

### 2. Navigate to Assignment 1

```bash
cd os-assignments
cd "Assignment 1"
```

### 3. Give Execute Permission

```bash
chmod +x student_database.sh
```

### 4. Run the Program

```bash
./student_database.sh
```

Alternatively:

```bash
bash student_database.sh
```

---

## Sample Menu

```text
----------------------------
      Student Database
----------------------------
1. Create Database
2. View Database
3. Insert Record
4. Delete Record
5. Modify Record
6. Display Result
7. Exit
----------------------------
Enter your choice:
```

---

## Sample Workflow

### Create Database

```text
Enter your choice: 1

Database Created Successfully.
```

### Insert Records

```text
Enter your choice: 3
Enter Roll Number: 101
Enter Name: Rahul
Enter Marks: 85

Record Inserted Successfully.
```

```text
Enter your choice: 3
Enter Roll Number: 102
Enter Name: Sneha
Enter Marks: 92

Record Inserted Successfully.
```

### View Database

```text
Enter your choice: 2

Roll   Name   Marks
--------------------
101    Rahul   85
102    Sneha   92
```

### Display Result

```text
Enter your choice: 6
Enter Roll Number: 101

Roll Number : 101
Name         : Rahul
Marks        : 85
Result       : Pass
```

### Delete Record

```text
Enter your choice: 4
Enter Roll Number to Delete: 102

Record Deleted Successfully.
```

### Modify Record

```text
Enter your choice: 5
Enter Roll Number to Modify: 101
Enter New Name: Rahul
Enter New Marks: 90

Record Modified Successfully.
```

---

## Learning Outcomes

After completing this assignment, the following concepts are understood:

* Basic Bash Shell scripting
* Variables and user input
* `if` conditions
* `case` statements
* Loops using `while`
* File handling in Shell
* Redirection operators
* AWK field processing
* AWK filtering and conditional statements
* Passing Shell variables to AWK
* Basic text-file-based database operations
* Combining Shell scripting with AWK

---

## Files

| File                  | Description                                          |
| --------------------- | ---------------------------------------------------- |
| `student_database.sh` | Main Shell program implementing the student database |
| `student.txt`         | Text file used to store student records              |

---

## Conclusion

This assignment demonstrates how **Shell scripting and AWK** can be combined to implement a simple text-based student database. It provides practical understanding of file handling, loops, conditional statements, command-line input, and AWK-based record processing.

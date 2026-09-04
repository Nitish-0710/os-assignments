#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void process_creation() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed.\n");
        return;
    }

    if (pid == 0) {
        printf("\nChild Process\n");
        printf("Child PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());
    } else {
        printf("\nParent Process\n");
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);
        wait(NULL);
    }
}

void zombie_process() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed.\n");
        return;
    }

    if (pid == 0) {
        printf("\nChild Process\n");
        printf("Child PID: %d\n", getpid());
        printf("Child exiting...\n");
        exit(0);
    } else {
        printf("\nParent Process\n");
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);
        printf("Parent sleeping for 20 seconds...\n");
        printf("Use another terminal to run: ps -el | grep process_control\n");

        sleep(20);
        wait(NULL);

        printf("Parent exiting...\n");
    }
}

void orphan_process() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed.\n");
        return;
    }

    if (pid == 0) {
        printf("\nChild Process\n");
        printf("Child PID: %d\n", getpid());
        printf("Initial Parent PID: %d\n", getppid());

        sleep(5);

        printf("\nAfter Parent Termination:\n");
        printf("Child PID: %d\n", getpid());
        printf("New Parent PID: %d\n", getppid());

        sleep(3);
    } else {
        printf("\nParent Process\n");
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);

        sleep(2);

        printf("Parent exiting...\n");
        exit(0);
    }
}

void process_tree() {
    pid_t child1, child2;
    pid_t grandchild1, grandchild2;

    printf("\nRoot Process: PID = %d, PPID = %d\n",
           getpid(), getppid());

    child1 = fork();

    if (child1 < 0) {
        printf("Fork failed.\n");
        return;
    }

    if (child1 == 0) {
        printf("Level 1 - Child 1: PID = %d, PPID = %d\n",
               getpid(), getppid());

        grandchild1 = fork();

        if (grandchild1 == 0) {
            printf("Level 2 - Grandchild 1: PID = %d, PPID = %d\n",
                   getpid(), getppid());
            sleep(5);
            exit(0);
        }

        sleep(5);
        exit(0);
    }

    child2 = fork();

    if (child2 < 0) {
        printf("Fork failed.\n");
        return;
    }

    if (child2 == 0) {
        printf("Level 1 - Child 2: PID = %d, PPID = %d\n",
               getpid(), getppid());

        grandchild2 = fork();

        if (grandchild2 == 0) {
            printf("Level 2 - Grandchild 2: PID = %d, PPID = %d\n",
                   getpid(), getppid());
            sleep(5);
            exit(0);
        }

        sleep(5);
        exit(0);
    }

    wait(NULL);
    wait(NULL);

    printf("Root Process exiting: PID = %d\n", getpid());
}

void exec_process() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed.\n");
        return;
    }

    if (pid == 0) {
        printf("\nChild executing 'ls' using exec()...\n");

        execl("/bin/ls", "ls", "-l", NULL);

        printf("exec() failed.\n");
        exit(1);
    } else {
        wait(NULL);
        printf("Program execution using exec() completed.\n");
    }
}

void fork_exec_process() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed.\n");
        return;
    }

    if (pid == 0) {
        printf("\nChild Process\n");
        printf("Child PID: %d\n", getpid());
        printf("Child executing 'ls' using exec()...\n");

        execl("/bin/ls", "ls", "-l", NULL);

        printf("exec() failed.\n");
        exit(1);
    } else {
        printf("\nParent Process\n");
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);
        printf("Parent waiting for child...\n");

        wait(NULL);

        printf("Child execution completed.\n");
        printf("Parent Process exiting.\n");
    }
}

int main() {
    int choice;

    while (1) {
        printf("\n========================================\n");
        printf("       PROCESS CONTROL MENU\n");
        printf("========================================\n");
        printf("1. Process Creation using fork()\n");
        printf("2. Zombie Process\n");
        printf("3. Orphan Process\n");
        printf("4. Multi-Level Process Tree\n");
        printf("5. Program Execution using exec()\n");
        printf("6. fork() + exec()\n");
        printf("7. Exit\n");
        printf("========================================\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                process_creation();
                break;

            case 2:
                zombie_process();
                break;

            case 3:
                orphan_process();
                break;

            case 4:
                process_tree();
                break;

            case 5:
                exec_process();
                break;

            case 6:
                fork_exec_process();
                break;

            case 7:
                printf("Exiting program...\n");
                exit(0);

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

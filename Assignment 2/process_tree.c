#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main() {
    pid_t child1, child2, grandchild1, grandchild2;

    printf("Root Process: PID = %d, PPID = %d\n",
           getpid(), getppid());

    child1 = fork();

    if (child1 < 0) {
        printf("Fork failed.\n");
        return 1;
    }

    if (child1 == 0) {
        printf("Level 1 - Child 1: PID = %d, PPID = %d\n",
               getpid(), getppid());

        grandchild1 = fork();

        if (grandchild1 == 0) {
            printf("Level 2 - Grandchild 1: PID = %d, PPID = %d\n",
                   getpid(), getppid());
            exit(0);
        }

        sleep(2);
        exit(0);
    }

    child2 = fork();

    if (child2 < 0) {
        printf("Fork failed.\n");
        return 1;
    }

    if (child2 == 0) {
        printf("Level 1 - Child 2: PID = %d, PPID = %d\n",
               getpid(), getppid());

        grandchild2 = fork();

        if (grandchild2 == 0) {
            printf("Level 2 - Grandchild 2: PID = %d, PPID = %d\n",
                   getpid(), getppid());
            exit(0);
        }

        sleep(2);
        exit(0);
    }

    sleep(3);

    printf("Root Process exiting: PID = %d\n", getpid());

    return 0;
}

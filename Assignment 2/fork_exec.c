#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("Parent Process: PID = %d\n", getpid());

    pid = fork();

    if (pid < 0) {
        printf("Fork failed.\n");
        return 1;
    }

    if (pid == 0) {
        printf("Child Process: PID = %d\n", getpid());
        printf("Child executing 'ls' using exec()...\n");

        execl("/bin/ls", "ls", "-l", NULL);

        printf("exec() failed.\n");
    }
    else {
        printf("Parent waiting for child...\n");

        wait(NULL);

        printf("Child execution completed.\n");
        printf("Parent Process exiting.\n");
    }

    return 0;
}

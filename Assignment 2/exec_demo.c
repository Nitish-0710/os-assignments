#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Before exec(): PID = %d\n", getpid());

    printf("Executing 'ls' command...\n");

    execl("/bin/ls", "ls", "-l", NULL);

    printf("exec() failed.\n");

    return 0;
}

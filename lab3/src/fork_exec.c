#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        execvp("./sequential_min_max", argv);
        exit(EXIT_SUCCESS);
    } 
    else {
       printf("Waiting for child process to finish executing.\n");
       wait(NULL);
    }

    return 0;
}

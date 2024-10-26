#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Дочерний процесс (PID: %d) завершает работу.\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        printf("Родительский процесс (PID: %d) создал дочерний процесс (PID: %d).\n", getpid(), pid);
        printf("Родительский процесс будет спать 10 секунд, чтобы создать зомби-процесс.\n");
        sleep(10);
        printf("Родительский процесс завершает работу.\n");
    }

    return 0;
}
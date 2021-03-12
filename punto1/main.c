#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void) {

    pid_t nuevo;
    for (int num = 0; num < 2; num++) {
        nuevo = fork();
        if (nuevo == 0)
            break;
    }

    nuevo =  fork();
    nuevo =  fork();
    printf("Soy el proceso %d y mi padre es %d\n", getpid(), getppid());
    return 0;
}

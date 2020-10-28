//#define _POSIX_C_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <errno.h>
#include <signal.h>

void handler (int signo) {
    if (signo == SIGUSR1)
        printf ("I caught it\n");
}

int main () {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigaction(SIGUSR1, &sa, NULL);
    pid_t pid = fork();

    if (pid == 0) {
        sleep(1);
        printf ("I'm a child\n");
        exit(EXIT_SUCCESS);
    }

    kill (pid, SIGUSR1);
    wait(NULL);
    return 0;
}



#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if  (pid < 0) {
        printf ("Cant do fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf ("Hello, I'm a child.\nMy pid is [%d]\n", getpid());
        kill(getpid(), SIGKILL);
        printf ("Whaaaaat?\n");
        exit(EXIT_SUCCESS);
    }
    printf ("I'm parent.\nMy pid is [%d]\n", getpid());
    int wstatus;
    sleep(2);
    pid_t wpid = waitpid (pid, &wstatus, 0);
    if (WIFEXITED(wstatus)) {
        printf ("Child was exit normally with [%d]\n", WEXITSTATUS(wstatus));
    }
    if (WIFSIGNALED(wstatus)) {
        printf ("Child was terminated by signal [%d]\n", WTERMSIG(wstatus));
    }
    return 0;
}

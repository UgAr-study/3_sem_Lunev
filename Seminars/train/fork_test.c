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
#include <sys/msg.h>

int main() {
    printf ("I'm parent.\nMy pid is [%d]\n", getpid());
    printf ("parent euid is %d\n", geteuid());
    int msgid = msgget (IPC_PRIVATE, 0666 | O_CREAT);
    pid_t pid = fork();

    if  (pid < 0) {
        printf ("Cant do fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        int msdctl_ret_val;
        sleep(2);
        if ((msdctl_ret_val = msgctl(msgid, IPC_RMID, NULL)) < 0) {
            printf ("\nmsgctl returned %d\n", msdctl_ret_val);
            perror("msgctl()");
        }

        printf ("\nmsgctl returned %d\n", msdctl_ret_val);

        printf ("Hello, I'm a child.\nMy pid is [%d]\nMy euid is %d\n", getpid(), geteuid());
        kill(getpid(), SIGKILL);
        //printf ("Whaaaaat?\n");
        exit(EXIT_SUCCESS);
    }
    //printf ("I'm parent.\nMy pid is [%d]\n", getpid());
    exit(EXIT_FAILURE);
    int wstatus;
    //sleep(2);
    pid_t wpid = waitpid (pid, &wstatus, 0);
    if (WIFEXITED(wstatus)) {
        printf ("Child [%d] was exit normally with [%d]\n", wpid, WEXITSTATUS(wstatus));
    }
    if (WIFSIGNALED(wstatus)) {
        printf ("Child [%d] was terminated by signal [%d]\n", wpid, WTERMSIG(wstatus));
    }
    return 0;
}

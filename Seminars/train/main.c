#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/wait.h>

int main() {
    pid_t ppid_bef_fork = getpid();

    printf ("I'm parent.\nMy pid is [%d]\n", ppid_bef_fork);

    pid_t pid = fork();

    if  (pid < 0) {
        printf ("Cant do fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {

        printf ("I'm a child.\nMy pid is [%d]\n", getpid());
        //exit(EXIT_SUCCESS);
        sleep(2);
        if (prctl(PR_SET_PDEATHSIG, SIGKILL) < 0) {
            printf ("Can't do prctl\n");
            exit(EXIT_FAILURE);
        }

        printf ("\nbefore checking ppid\n");

        if (getppid() != ppid_bef_fork) {
            printf ("My parent has been changed\n");
            printf ("My new parent pid is [%d]\n", getppid());
            kill(getpid(), SIGKILL);
            //exit(EXIT_FAILURE);
        }
        printf ("before sleep\n");

        printf ("after sleep\n");
        exit(EXIT_SUCCESS);
    }
    //printf ("parent is going to sleep\n");
    //sleep(1);
    //getchar();
    printf ("\nparent is returning\n");
    return 0;
}

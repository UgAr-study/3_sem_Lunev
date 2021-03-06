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
#include <string.h>

//int i = 0;

struct Bits {
    short bits[8 * sizeof(char)];
};

struct Bits ToBits(char c);
void PrintBit (struct Bits bits);
char ToChar (struct Bits bit);
int SetParentDeath (pid_t ppid_bef_fork);
void SetBitsEmpty (struct Bits* bit);

void child_handler (int signo);
void parent_handler (int signo);
void wait_child (int signo);

int count = 0;
struct Bits bit_char;

int main () {

    SetBitsEmpty(&bit_char);
    printf ("Parent: my pid is %d\n", getpid());
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);

    sigprocmask(SIG_BLOCK, &mask, NULL);

    struct sigaction ch_sa, p_sa, w_ch;

    p_sa.sa_handler = parent_handler;
    w_ch.sa_handler = wait_child;

    pid_t pid = fork();

    if (pid == 0) {
        prctl(PR_SET_PDEATHSIG, SIGKILL);
        printf ("Child: my pid is %d\n", getpid());

        char TestText[] = "Hello";

        ch_sa.sa_handler = child_handler;
        sigaction (SIGUSR1, &ch_sa, NULL);

        sigset_t ch_mask;
        sigfillset(&ch_mask);
        sigdelset(&ch_mask, SIGUSR1);
        sigdelset(&ch_mask, SIGINT);

        for (int j = 0; j < strlen(TestText); ++j) {
            printf ("Child: j = %d\n", j);
            fflush(stdout);

            if (j == 2)
                kill(getpid(), SIGKILL);

            struct Bits bit_ = ToBits(TestText[j]);

            for (int r = 0; r < 8 * sizeof(char); ++r) {

                if (bit_.bits[r] == 0)
                    kill(getppid(), SIGUSR1);
                else if (bit_.bits[r] == 1)
                    kill(getppid(), SIGUSR2);
                sigsuspend(&ch_mask);
            }
        }

        exit(EXIT_SUCCESS);
    }

    sigaction(SIGCHLD, &w_ch, NULL);
    sigaction(SIGUSR1, &p_sa, NULL);
    sigaction(SIGUSR2, &p_sa, NULL);

    sigset_t p_mask;
    sigfillset(&p_mask);
    sigdelset(&p_mask, SIGUSR1);
    sigdelset(&p_mask, SIGUSR2);
    sigdelset(&p_mask, SIGCHLD);
    sigdelset(&p_mask, SIGINT);

    int r = 0;
    while (1) {
        if (count == 9 * sizeof(char))
            break;

        sigsuspend(&p_mask);

        if (count == 8 * sizeof(char)) {
            printf ("Parent: %c\n", ToChar(bit_char));
            SetBitsEmpty(&bit_char);
            count = 0;
        }
        //printf ("on %d time: i = %d\n",r, i);
        //fflush(stdout);
        ++r;
        kill(pid, SIGUSR1);
    }

    return 0;
}

void child_handler (int signo) {
    if (signo == SIGUSR1) {
        printf("Child: got SIGUSR1\n");
        fflush(stdout);
    }
}

void parent_handler (int signo) {
    if (signo == SIGUSR1) {
        printf ("Parent: SU1\n");
        bit_char.bits[count] = 0;
        count++;
        return;
    }
    if (signo == SIGUSR2) {
        printf ("Parent: SU2\n");
        bit_char.bits[count] = 1;
        count++;
        return;
    }
    else
        printf ("Unknown signal for child's handler\n");
}

void wait_child (int signo) {
    if (signo == SIGCHLD) {
        printf ("Parent: child changed his status\n");
        fflush(stdout);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}

void SetBitsEmpty (struct Bits* bit) {
    for (int i = 0; i < 8 * sizeof(char); ++i)
        bit->bits[i] = 0;
}

struct Bits ToBits(char c) {
    struct Bits res;
    for (int i = 0; i < 8 * sizeof(char); ++i) {
        if ((c & (1 << i)) != 0) {
            res.bits[i] = 1;
        } else
            res.bits[i] = 0;
    }
    return res;
}

char ToChar (struct Bits bit) {
    char c = 0;
    for (int i = 0; i < 8 * sizeof(char); ++i)
        c = c | (bit.bits[i] << i);
    return c;
}

void PrintBit (struct Bits c) {
    for (int i = 0; i < 8 * sizeof(char); ++i)
        printf ("%d", c.bits[i]);
    printf ("\n");
}

int SetParentDeath (pid_t ppid_bef_fork) {

    if (prctl(PR_SET_PDEATHSIG, SIGKILL) < 0) {
        printf ("Can't do prctl\n");
        return -1;
    }

    if (getppid() != ppid_bef_fork) {
        printf ("My parent has been changed\n");
        printf ("My new parent pid is [%d]\nI'm finish", getppid());
        return -1;
    }

    return 0;
}




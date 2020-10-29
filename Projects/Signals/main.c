//#include <bits/sigaction.h>
#include "header.h"

//TODO SIGUSR1 = 0, SIGUSR2 = 1

int count = 0;
struct Bits bit_char;

void child_handler (int signo);
void parent_handler (int signo);
void wait_child (int signo);

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
        char filepath[] = "../test.txt";
        FILE *file = fopen (filepath, "r");

        ch_sa.sa_handler = child_handler;
        sigaction (SIGUSR1, &ch_sa, NULL);

        sigset_t ch_mask;
        sigfillset(&ch_mask);
        sigdelset(&ch_mask, SIGUSR1);
        sigdelset(&ch_mask, SIGINT);

        char ch;
        int j = 0;
        while ((ch = fgetc(file)) != EOF) {
            printf ("Child: j = %d\n", j);
            fflush(stdout);

            struct Bits bit_ = ToBits(ch);

            for (int r = 0; r < 8 * sizeof(char); ++r) {

                if (bit_.bits[r] == 0)
                    kill(getppid(), SIGUSR1);
                else if (bit_.bits[r] == 1)
                    kill(getppid(), SIGUSR2);
                sigsuspend(&ch_mask);
            }
            ++j;
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

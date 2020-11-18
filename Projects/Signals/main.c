#include "header.h"

//TODO SIGUSR1 = 0, SIGUSR2 = 1

volatile int count = 0;
struct Bits bit_char;
volatile int is_child_dead = 0;

void child_handler (int signo);
void parent_handler (int signo);
void wait_child (int signo);

int main () {

    SetBitsEmpty(&bit_char);
    printf ("Parent: my pid is %d\n", getpid());
    sigset_t mask;

    if (sigemptyset(&mask) < 0) {
        perror("sigemptyset()");
        exit(EXIT_FAILURE);
    }

    if (sigaddset(&mask, SIGUSR1) < 0) {
        perror("sigaddset()");
        exit (EXIT_FAILURE);
    }

    if (sigaddset(&mask, SIGUSR2) < 0) {
        perror ("sigaddset()");
        exit (EXIT_FAILURE);
    }

    if (sigaddset(&mask, SIGCHLD) < 0) {
        perror ("sigaddtesst()");
        exit (EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror ("sigprocmask()");
        exit(EXIT_FAILURE);
    }

    struct sigaction ch_sa, p_sa, w_ch;

    p_sa.sa_flags = 0;
    p_sa.sa_handler = parent_handler;

    w_ch.sa_handler = wait_child;
    w_ch.sa_flags = SA_NOCLDWAIT;

    pid_t ppid_before_fork = getpid();

    pid_t pid = fork();

    if (pid < 0) {
        printf ("Can't do fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        SetParentDeath(ppid_before_fork);

        printf ("Child: my pid is %d\n", getpid());

        char filepath[] = "../test.txt";
        FILE *file = fopen (filepath, "r");

        ch_sa.sa_flags = 0;
        ch_sa.sa_handler = child_handler;
        sigaction (SIGUSR1, &ch_sa, NULL);

        sigset_t ch_mask;

        if (sigfillset(&ch_mask) < 0) {
            perror ("sigfillset()");
            exit(EXIT_FAILURE);
        }

        if (sigdelset(&ch_mask, SIGUSR1) < 0) {
            perror ("sigdelset()");
            exit(EXIT_FAILURE);
        }

        if (sigdelset(&ch_mask, SIGINT) < 0){
            perror ("sigdelset()");
            exit(EXIT_FAILURE);
        }

        char ch;
        int j = 0;
        while ((ch = fgetc(file)) != EOF) {
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

    if (sigaction(SIGCHLD, &w_ch, NULL) < 0) {
        perror ("sidaction()");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR1, &p_sa, NULL) < 0) {
        perror ("sigaction()");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &p_sa, NULL) < 0) {
        perror ("sigaction()");
        exit(EXIT_FAILURE);
    }

    sigset_t p_mask;

    if (sigfillset(&p_mask) < 0) {
        perror("sigfillset()");
        exit(EXIT_FAILURE);
    }

    if (sigdelset(&p_mask, SIGUSR1) < 0) {
        perror ("sigdelset()");
        exit(EXIT_FAILURE);
    }

    if (sigdelset(&p_mask, SIGUSR2) < 0) {
        perror ("sigdelset()");
        exit (EXIT_FAILURE);
    }
    sigdelset(&p_mask, SIGCHLD);
    sigdelset(&p_mask, SIGINT);

    int r = 0;
    while (1) {
        if (count == 9 * sizeof(char))
            break;

        sigsuspend(&p_mask);

        if (is_child_dead) {
            printf ("Child died\n");
            exit(EXIT_SUCCESS);
        }

        if (count == 8 * sizeof(char)) {
            printf ("%c", ToChar(bit_char));
            fflush(stdout);
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
        fflush(stdout);
    }
}

void parent_handler (int signo) {
    if (signo == SIGUSR1) {
        bit_char.bits[count] = 0;
        count++;
        return;
    }
    if (signo == SIGUSR2) {
        bit_char.bits[count] = 1;
        count++;
        return;
    }
    else
        printf ("Unknown signal for child's handler\n");
}

void wait_child (int signo) {
    if (signo == SIGCHLD) {
        is_child_dead = 1;
        printf ("Parent: child changed his status\n");
        fflush(stdout);
    }
}

#include "header.h"

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

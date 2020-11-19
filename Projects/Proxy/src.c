#include "header.h"

void free_all(struct Channel *channels, size_t size) {
    for (int i = 0; i < size; ++i) {
        char *buff = NULL;
        if ((buff = channels[i].buffer)) {
            free (buff);
        }
    }
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

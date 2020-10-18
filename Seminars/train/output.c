#include "../../Projects/First_Pipe/fifo_header.h"
#include <sys/wait.h>

int main() {
////
    pid_t pid = fork ();

    if (pid < 0) {
        printf ("Don't know\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf ("Hello\n");
        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        printf ("Good Bye\n");
        wait(NULL);
    }

    return 0;
}

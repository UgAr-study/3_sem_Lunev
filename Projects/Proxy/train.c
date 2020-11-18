#include "header.h"

int main () {

    struct Channel channels[2] = {0};

    for (int i = 0; i < 2; ++i) {
        int fds_to[2] = {0};
        int fds_from[2] = {0};

        pipe(fds_to);
        pipe(fds_from);

        pid_t pid = fork();

        if (pid == 0) {
            //child
            int fd_from = 0, fd_to = 0;

            if (i == 0) {
                close (fds_to[0]);
                close (fds_to[1]);
                close (fds_from[0]);

                char *file = "test.txt";
                fd_from = open (file, O_RDONLY);

                if (fd_from <= 0) {
                    perror("open: ");
                    exit(EXIT_FAILURE);
                }

                fd_to = fds_from[1];
            }

            if (i == 1) {
                close (fds_from[0]);
                close (fds_to[1]);

                fd_from = fds_to[0];
                fd_to = fds_from[1];
            }

            printf ("child[%d]: splice from %d to %d\n", i, fd_from, fd_to);
            splice(fd_from, NULL, fd_to, NULL, 20, SPLICE_F_MOVE);

            printf("child finished\n");
            exit(EXIT_SUCCESS);

        } else {
            if (i == 0) {
                close (fds_to[0]);
                close (fds_to[1]);
                close (fds_from[1]);

                channels[i].fd_from = fds_from[0];
                channels[i].buffer = (char *) calloc(50, sizeof(char));
            }

            if (i == 1) {
                close (fds_to[0]);
                close (fds_from[1]);

                channels[i - 1].fd_to = fds_to[1];
                channels[i].fd_from = fds_from[0];
                channels[i].fd_to = STDOUT_FILENO;
                channels[i].buffer = (char *) calloc(50, sizeof(char));
            }
        }
    }

    read(channels[0].fd_from, channels[0].buffer, 20);
    write(channels[0].fd_to, channels[0].buffer, 20);
    read(channels[1].fd_from, channels[1].buffer, 20);
    write(channels[1].fd_to, channels[1].buffer, 20);


    free (channels[0].buffer);
    free (channels[1].buffer);

    wait(NULL);
    wait(NULL);
    return 0;
}
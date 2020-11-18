#include "header.h"

int main() {

    struct Channel *channels = (struct Channel*) calloc (N, sizeof (struct Channel));

    for (int i = 0; i < N; ++i) {

        int fds_to[2] = {0};
        int fds_from[2] = {0};

        pipe(fds_to);
        pipe(fds_from);

        if (i == 0) {
            close(fds_to[0]);
            close (fds_to[1]);

            channels[i].fd_from = fds_from[0];
            channels[i].buffer = (char*) calloc ((size_t)pow(3, N - i) * 1024, sizeof(char));

        } else if (i == N - 1) {
            channels[i].fd_from = fds_from[0];
            channels[i].fd_to = STDOUT_FILENO;
            channels[i].buffer = (char*) calloc ((size_t)pow(3, N - i) * 1024, sizeof(char));

        } else {
            channels[i - 1].fd_to = fds_to[1];
            channels[i].fd_from = fds_from[0];
            channels[i].buffer = (char*) calloc ((size_t)pow(3, N - i) * 1024, sizeof(char));

        }

        pid_t pid = fork();

        if (pid < 0) {
            printf ("parent can't do fork\n");
            free_all(channels, i + 1);
            perror ("fork: ");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {

            int fd_from = 0, fd_to = 0;
            close (fds_to[1]);
            close (fds_from[0]);

            if (i == 0) {
                char *text = "Hello!";

                fd_from = open (text, O_RDONLY);
                fd_to = fds_from[1];

            } else {
                fd_from = fds_to[0];
                fd_to = fds_from[1];
            }

            while(1) {

                if (i == 100/*some condition*/) {
                    break;
                }

                splice(fd_from, NULL, fd_to, NULL, PAGE, SPLICE_F_MOVE);
                //TODO: wait for message from fd_from
                //TODO: send this message to fd_to
            }

        } else {

            if (i == 0) {
                close (fds_from[1]);
                continue;
            }

            close (fds_from[1]);
            close (fds_to[0]);
        }
    }
    return 0;
}

#include "header.h"


int stop = 1;

int ch_deaths1 = 0;

void child_handler1(int s) {
    if (s == SIGCHLD) {

        int state = 0;
        wait(&state);

        if (WIFEXITED(state)) {

            if (WEXITSTATUS(state) != EXIT_SUCCESS) {
                printf ("One of the children was killed\n");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }

            ch_deaths1++;
            //if (ch_alive == 0) stop = 0;

            printf("One of the children successfully finished\n");
            fflush(stdout);
        }
    }
}

int isempty (const char *buf, int len) {
    for (int i = 0; i < len; ++i) {
        if (buf[i] != '\0') {
            return 0;
        }
    }
    return 1;
}

void clear (char* buf, int len) {
    for (int i = 0; i < len; ++i)
        buf[i] = '\0';
}

int main () {


    printf ("POLLWRNORM = %d\n", POLLWRNORM);
    printf ("POLLRDNORM = %d\n", POLLRDNORM);
    printf ("POLLIN     = %d\n", POLLIN);
    printf ("POLLOUT    = %d\n", POLLOUT);
    printf ("POLLERR    = %d\n", POLLERR);
    printf ("POLLNVAL   = %d\n", POLLNVAL);
    printf ("POLLHUP    = %d\n\n", POLLHUP);

    fflush(stdout);

    //ch_alive = 3;

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = child_handler1;

    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        printf ("Can't do signal handler\n");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    struct Channel channels[3] = {0};

    pid_t ppid_before_fork = getpid();

    for (int i = 0; i < 3; ++i) {
        int fds_to[2] = {0};
        int fds_from[2] = {0};

        pipe(fds_to);
        pipe(fds_from);

        pid_t pid = fork();

        if (pid == 0) {
            //child

            for (int j = 0; j < i; ++j) {
                close (channels[j].fd_to);
                close (channels[j].fd_from);
            }


            if (SetParentDeath(ppid_before_fork) < 0) {
                printf ("child: parent died\n");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }

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
            } else {
                close (fds_from[0]);
                close (fds_to[1]);

                fd_from = fds_to[0];
                fd_to = fds_from[1];
            }

           while(1) {
                printf("child[%d]: splice from %d to %d\n", i, fd_from, fd_to);
                fflush(stdout);

                int srv = splice(fd_from, NULL, fd_to, NULL, 20, SPLICE_F_MOVE);
                if (srv <= 0) break;

                printf("child[%d]: %d bytes spliced\n", i, srv);
                fflush(stdout);
            }

            close(fd_to);
            close(fd_from);

            printf("child[%d] finished\n", i);
            fflush(stdout);

            exit(EXIT_SUCCESS);

        } else {
            if (i == 0) {
                close (fds_to[0]);
                close (fds_to[1]);
                close (fds_from[1]);

                channels[i].fd_from = fds_from[0];

            } else if (i == 2) {
                close (fds_to[0]);
                close (fds_from[1]);

                channels[i - 1].fd_to = fds_to[1];
                channels[i].fd_from = fds_from[0];
                channels[i].fd_to = STDOUT_FILENO;

            } else {
                close (fds_to[0]);
                close (fds_from[1]);

                channels[i - 1].fd_to = fds_to[1];
                channels[i].fd_from = fds_from[0];
            }
        }
    }

    for (int i = 0; i < 3; ++i) {
        channels[i].buffer = (char *) calloc(50, sizeof(char));
    }

    struct pollfd reading[3], writing[3];

    for (int i = 0; i < 3; ++i) {
        reading[i].fd = channels[i].fd_from;
        reading[i].events = POLLRDNORM;

        writing[i].fd = channels[i].fd_to;
        writing[i].events = POLLWRNORM;
    }

    for (int i = 0; i < 3; ++i) {
        int res;
        res = fcntl(channels[i].fd_from, F_SETFL, O_NONBLOCK);

        if (res < 0) {
            perror("fcntl read: ");
            exit(EXIT_FAILURE);
        }

        res = fcntl(channels[i].fd_to, F_SETFL, O_NONBLOCK);

        if (res < 0) {
            perror("fcntl write: ");
            exit(EXIT_FAILURE);
        }
    }

    int r = 0;
    while (1) {


        int ready_read = poll(reading, 3, 0);
        printf ("\nPoll for reading = %d\n\n", ready_read);
        fflush(stdout);

        if (ready_read > 0) {
            int close_fds = 0;
            for (int i = 0; i < 3; ++i) {

                printf ("reading[%i].revents = %d\n", i, reading[i].revents);
                fflush(stdout);

                if (reading[i].revents & POLLNVAL)
                    close_fds++;

                if (reading[i].revents & POLLRDNORM) {
                    printf ("\nreading from channels[%d]...\n\n", i);
                    fflush(stdout);
                    if (read(channels[i].fd_from, channels[i].buffer, 20) >= 0) {
                        printf("buffer[%d] is after reading:\"%s\"\n", i, channels[i].buffer);
                        fflush(stdout);
                    }
                }
                if (reading[i].revents & POLLHUP) {
                    close (channels[i].fd_from);
                }
            }
            if (close_fds == 3) {
                break;
            }

        }

        int ready_write = poll(writing, 3, 0);

        printf ("\nPoll for writing = %d\n\n", ready_write);
        fflush(stdout);

        if (ready_write > 0) {
            for (int i = 0; i < 3; ++i) {

                printf ("writing[%i].revents = %d\n", i, writing[i].revents);
                fflush(stdout);

                if (writing[i].revents & POLLERR) {
                    close (channels[i].fd_to);
                    continue;
                }

                if (writing[i].revents & POLLWRNORM) {

                    printf ("\n%d children died; biffer state is %d\n\n", ch_deaths1, isempty(channels[i].buffer, 50));
                    fflush(stdout);

                    if (isempty(channels[i].buffer, 50) && ch_deaths1 == i + 1) {
                        if (i != 3 - 1) {
                            close(channels[i].fd_to);
                            //sleep(1);
                            printf ("channels[%d].fd_to closed\n", i);
                            fflush(stdout);
                        }
                    }

                    if (isempty(channels[i].buffer, 50)) continue;

                    printf("buffer[%d] before writing is :\"%s\"\n", i, channels[i].buffer);
                    fflush(stdout);

                    if (write(channels[i].fd_to, channels[i].buffer, 20) >= 0) {
                        clear(channels[i].buffer, 50);

                        //sleep(1);

                    }
                }

            }
        }
    }

    for (int i = 0; i < 3; ++i) {
        free (channels[i].buffer);
        wait(NULL);
    }

    //sleep(5);
    return 0;
}
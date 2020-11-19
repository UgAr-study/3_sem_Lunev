#include "header.h"

int child_deaths = 0;

int main() {

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = child_handler;

    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        printf ("Can't do signal handler\n");
        exit(EXIT_FAILURE);
    }

    printf ("Parent pid is %d\n", getpid());

    struct Channel *channels = (struct Channel*) calloc (N, sizeof (struct Channel));

    for (int i = 0; i < N; ++i) {

        int fds_to[2] = {0};
        int fds_from[2] = {0};

        pipe(fds_to);
        pipe(fds_from);

        printf ("\n\n>>>>Pipe[%d]::fds_to[0]   = %d\n"
                ">>>>Pipe[%d]::fds_to[1]   = %d\n"
                ">>>>Pipe[%d]::fds_from[0] = %d\n"
                ">>>>Pipe[%d]::fds_from[1] = %d\n\n\n",
                i, fds_to[0], i, fds_to[1], i, fds_from[0], i, fds_from[1]);

        pid_t ppid_before_fork = getpid();
        pid_t pid = fork();

        if (pid < 0) {
            printf ("parent can't do fork\n");
            free_all(channels, i + 1);
            perror ("fork: ");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {

            for (int j = 0; j < i; ++j) {
                close (channels[j].fd_from);
                close (channels[j].fd_to);
            }


            if (SetParentDeath(ppid_before_fork) < 0) {
                printf ("parent died\n");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }

            int fd_from = 0, fd_to = 0;

            if (i == 0) {
                close(fds_to[0]);
                close(fds_to[1]);
                close(fds_from[0]);

                char *filepath = "test.txt";
                fd_from = open (filepath, O_RDONLY);

                printf ("\nfile descriptor is %d\n", fd_from);
                fflush(stdout);

                if (fd_from <= 0) {
                    printf ("Can't open a file\n");
                    fflush(stdout);
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

                printf ("\nChild[%d]: splice from %d to %d\n\n", i, fd_from, fd_to);
                fflush(stdout);

                int spl_ret_val;
                if ((spl_ret_val = splice(fd_from, NULL, fd_to, NULL, PAGE, SPLICE_F_MOVE)) == 0) {
                    close (fd_to);
                    close (fd_from);

                    printf ("child №%d finished\n", i);
                    fflush(stdout);
                    exit(EXIT_SUCCESS);
                }

                if (spl_ret_val < 0) {
                    printf ("child[%d]: splice\n", i);
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                }
            }

        } else {

            if (i == 0) {
                close (fds_to[0]);
                close (fds_to[1]);
                close (fds_from[1]);

                channels[i].fd_from = fds_from[0];
                channels[i].fd_to = -1; //?

            } else if (i == N - 1) {
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

            printf ("pid[%d] = %d\n", i, pid);
        }
    }

    for (int i = 0; i < N; ++i) {
        channels[i].size = (size_t)pow(3, N - i) * 1024;
        channels[i].buffer = (char*) calloc (channels[i].size, sizeof(char));
        channels[i].offset_for_read = channels[i].buffer;
        channels[i].offset_for_write = channels[i].buffer;
        channels[i].empty = channels[i].size;
        channels[i].full = 0;
    }

    struct pollfd *poll_read_fds = (struct pollfd*) calloc (N, sizeof (struct pollfd));
    struct pollfd *poll_write_fds = (struct pollfd*) calloc (N, sizeof (struct pollfd));

    for (int i = 0; i < N; ++i) {
        poll_write_fds[i].fd = channels[i].fd_to;
        poll_write_fds[i].events = POLLWRNORM;

        poll_read_fds[i].fd = channels[i].fd_from;
        poll_read_fds[i].events = POLLRDNORM;
    }

    for (int i = 0; i < N; ++i) {
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

    while (1) {

       int ready_to_read = poll (poll_read_fds, N, 0);

       if (ready_to_read > 0) {

           int closed_read_fds = 0;
           for (int i = 0; i < N; ++i) {

               printf ("poll_read_fds[%i].revents = %d\n", i, poll_read_fds[i].revents);
               fflush(stdout);

               if (poll_read_fds[i].revents & POLLNVAL) {
                   closed_read_fds++;
                   continue; //?
               }

               if (poll_read_fds[i].revents & POLLRDNORM) {
                   //read from pipe to buffer (check if buffer is available: empty != 0 or offset != 0)
                   // don't forget to update empty

                   printf("Parent: read from channels[%d]\n", i);

                   if (channels[i].empty != 0) {

                       int read_ret_val = 0;

                       if (channels[i].offset_for_write - channels[i].buffer <= channels[i].size - channels[i].empty) {
                           read_ret_val = read(channels[i].fd_from, channels[i].offset_for_write, channels[i].empty);

                       } else {
                           size_t n = (size_t) (channels[i].buffer + channels[i].size - channels[i].offset_for_write);
                           read_ret_val = read(channels[i].fd_from, channels[i].offset_for_write, n);
                       }

                       if (read_ret_val < 0) {
                           perror("Parent: read: ");
                           free_all(channels, N);
                           exit(EXIT_FAILURE);
                       }

                       channels[i].empty -= read_ret_val;
                       channels[i].offset_for_write += read_ret_val;
                       channels[i].full += read_ret_val;

                       assert (channels[i].empty + channels[i].full == channels[i].size);

                       if (channels[i].offset_for_write - channels[i].buffer == channels[i].size) {
                           //we wrote up to end;
                           channels[i].offset_for_write = channels[i].buffer;
                       }

                       printf("buffer[%d] is after reading:\"%s\"\n", i, channels[i].buffer);
                       fflush(stdout);
                   }
               }

               if (poll_read_fds[i].revents & POLLHUP) {
                   close (channels[i].fd_from);
               }
           }

           if (closed_read_fds == N) {
               //check the last buffer?
               break;
           }

        }

        int ready_to_write = poll (poll_write_fds, N, 0);

        if (ready_to_write > 0) {

            for (int i = 0; i < N; ++i) {

                printf ("poll_write_fds[%i].revents = %d\n", i, poll_write_fds[i].revents);
                fflush(stdout);

                if (poll_write_fds[i].revents & POLLERR) {
                    close (channels[i].fd_to);
                    continue;
                }

                if (poll_write_fds[i].revents & POLLWRNORM) {
                    // i != 0
                    //write from buffer to pipe (check if buffer is not empty: empty != size)
                    // don't forget to update offset (by write() returned value)
                    // if offset == size -> offset = 0

                    printf ("\n%d children died; biffer state is %zu\n\n", child_deaths, channels[i].full);
                    fflush(stdout);

                    if (child_deaths == i + 1 && channels[i].full == 0) {
                        if (i != N - 1) {
                            close(channels[i].fd_to);
                            printf("channels[%d].fd_to closed\n", i);
                            fflush(stdout);
                        }
                    }

                    if (channels[i].full != 0) {

                        printf("buffer[%d] before writing is :\"%s\"\n", i, channels[i].buffer);
                        fflush(stdout);

                        int write_ret_val = 0;

                        if (channels[i].offset_for_read - channels[i].buffer <= channels[i].size - channels[i].full) {
                            write_ret_val = write(channels[i].fd_to, channels[i].offset_for_read, channels[i].full);
                        } else {
                            size_t n = (size_t) (channels[i].buffer + channels[i].size - channels[i].offset_for_read);
                            write_ret_val = write(channels[i].fd_to, channels[i].offset_for_read, n);
                        }

                        if (write_ret_val < 0) {
                            perror("Parent: write: ");
                            free_all(channels, N);
                            exit(EXIT_FAILURE);
                        }

                        channels[i].full -= write_ret_val;
                        channels[i].offset_for_read += write_ret_val;
                        channels[i].empty += write_ret_val;

                        assert (channels[i].empty + channels[i].full == channels[i].size);

                        if (channels[i].offset_for_read - channels[i].buffer == channels[i].size) {
                            //we read up to end;
                            channels[i].offset_for_read = channels[i].buffer;
                        }
                    }
                }
            }
        }
    }

    return 0;
}


void child_handler(int s) {
    if (s == SIGCHLD) {
        int state = 0;
        wait(&state);
        if (WIFEXITED(state)) {
            if (WEXITSTATUS(state) != EXIT_SUCCESS) {
                printf ("One of the children was killed\n");
                exit(EXIT_FAILURE);
            }
            printf("One of the children successfully finished\n");
            child_deaths++;
        }
    }
}
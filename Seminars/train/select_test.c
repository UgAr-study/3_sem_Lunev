#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>

int main() {
    int fd;
    const char* filepath = "/home/artem/ProgProjects/ClionProjects/Lunev/Seminars/train/fifo";
    //mkfifo(filepath, 0666);
    if (mkfifo(filepath, 0666) < 0 && errno != EEXIST) {
        printf("Ooops! input.c\n");
        exit(EXIT_FAILURE);
    }

    int fds[2];
    pipe(fds);
    close (fds[1]);
    fcntl (fds[0], F_SETFL, O_RDONLY | O_NONBLOCK);

    fd_set r1;
    FD_ZERO(&r1);
    FD_SET(fds[0], &r1);

    struct timeval tv1;
    tv1.tv_sec = 1;
    tv1.tv_usec = 0;

    int res_sel = select(fds[0] + 1,&r1, NULL, NULL, NULL);
    printf ("res_sel = %d\n", res_sel);

    char pipe_contents[10];
    int bytes_read = read(fds[0], pipe_contents, 10);
    printf("Read from pipe returned %d\n", bytes_read);
/*
    fd = open (filepath, O_RDONLY | O_NONBLOCK);
    printf ("input fd = %d\n", fd);
    fd_set r, w;
    FD_ZERO(&r);
    FD_ZERO(&w);
    FD_SET(fd, &r);
    FD_SET(fd, &w);

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    //sleep(1);

    char buf[10];
    int bytes_read = read(fd, buf, 10);
    printf("Read returned %d\n", bytes_read);

    //int sel_ret_valR = select(fd + 1, &r, NULL, NULL, NULL);
    //int sel_ret_valW = select(fd + 1, NULL, &w, NULL, &tv);

    //printf ("selectR is %d\tr.bits = %ld\n", sel_ret_valR, r.fds_bits[0]);
    //printf ("selectW is %d\tw.bits = %ld\n", sel_ret_valW, w.fds_bits[0]);
    */
}


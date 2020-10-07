#include "../../Projects/First_Pipe/fifo_header.h"

int main() {
    int fd;
    const char* filepath = "/home/artem/ProgProjects/ClionProjects/Lunev/Seminars/train/fifo";
    mkfifo(filepath, 0666);
    if (mkfifo(filepath, 0666) < 0 && errno != EEXIST)
        printf ("Ooops! input.c\n");
    fd = open (filepath, O_RDWR);
    printf ("input fd = %d\n", fd);
    fd_set r, w;
    FD_ZERO(&r);
    FD_ZERO(&w);
    FD_SET(fd, &r);
    FD_SET(fd, &w);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    //sleep(1);
    int sel_ret_valR = select(fd + 1,&r, NULL, NULL, &tv);
    int sel_ret_valW = select(fd + 1,NULL, &w, NULL, &tv);

    printf ("selectR is %d\tr.bits = %ld\n", sel_ret_valR, r.fds_bits[0]);
    printf ("selectW is %d\tw.bits = %ld\n", sel_ret_valW, w.fds_bits[0]);
}
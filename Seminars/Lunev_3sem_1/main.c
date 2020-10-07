#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int fd[2], res, text_file;
    res = pipe(fd);

    if (res < 0) {
        printf ("Error\n");
        exit (-1);
    }

    res = fork();
    if (res < 0){
        printf ("Error of creating fork\n");
        exit (-1);
    } else if (res > 0) {
        close (fd[0]);

        text_file = open ("test.txt", O_RDONLY);


        if (text_file < 0) {
            printf ("Error of open file: %d\n", text_file);
            exit (-1);
        }

        struct stat st;
        fstat(text_file, &st);

        off_t size_of_file = st.st_size, res_read;
        while (res_read = read (text_file, text, size_of_file);)
        char *text = (char*) calloc (size_of_file, sizeof(char));

        off_t

        if (res_read != size_of_file) {
            printf ("Error of reading file\n");
            exit(-1);
        }

        char *test = "Hello, world!";
        size_t len = strlen(test) + 1;
        off_t res_write = write (fd[1], text, len);

        if (res_write != len) {
            printf ("Error of write in pipe\n");
            exit (-1);
        }

        close(fd[1]);
        printf ("Parent exit\n");
    } else {
        close (fd[1]);

        char c;
        while (read (fd[0], &c, sizeof(char)) > 0)
            printf ("%c", c);

        close (fd[0]);

        printf ("\nChild exit\n");
    }
    return 0;
}

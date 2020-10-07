#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

    execvp(argv[1], &argv[1]);
    printf("Hello, world");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* foo (void* arg) {
    int* pi = (int*) arg;
    for (int i = 0; i < 10000; ++i) {
        (*pi)++;
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int a = 0;
    long n = strtol (argv[1], NULL, 0);
    pthread_t* parr = (pthread_t*) calloc (n, sizeof(pthread_t));
    for (long i = 0; i < n; ++i) {
        pthread_create(&parr[i], NULL, &foo, &a);
        printf ("%d\n", a);
    }
    for (long i = 0; i < n; ++i) {
        pthread_join(parr[i], NULL);
    }
    return 0;
}

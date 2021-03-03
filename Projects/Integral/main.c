#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "true.c"

double foo (double x) {
    return x * x;
}

//void *(*start_routine) (void *);
struct thread_info {
    double begin;
    double end;
    int n_steps;
    double (*foo) (double x);
};

void* start_routine (void* arg) {

    struct thread_info* params = arg;
    printf ("do from [%f] to [%f]\n", params->begin, params->end);
    double* res = (double *) calloc (1, sizeof(double));
    *res = TrueIntegral(params->begin, params->end, params->n_steps, params->foo);
    return res;
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        perror("Not all args\n");
        return 0;
    }

    const double begin = 0.0;
    const double end   = 500.0;
    const int n_steps  = 500000000;

    int n_threads = (int) strtol (argv[1], NULL, 10);
    pthread_t* threads = (pthread_t*) calloc (n_threads, sizeof (pthread_t));

    int check = 0;
    struct thread_info* data = (struct thread_info*) malloc (n_threads * sizeof (struct thread_info));


    const double step = (end - begin) / (double) n_threads;
    double th_begin = begin;
    double th_end   = begin + step;
    const int th_n_steps = n_steps / n_threads;

    for (int i = 0; i < n_threads - 1; ++i) {

        data[i].begin   = th_begin;
        data[i].end     = th_end;
        data[i].n_steps = th_n_steps;
        data[i].foo     = foo;

        th_begin = th_end;
        th_end += step;
    }

    data[n_threads - 1].begin   = th_begin;
    data[n_threads - 1].end     = end;
    data[n_threads - 1].n_steps = n_steps - (n_threads - 1) * th_n_steps;
    data[n_threads - 1].foo     = foo;

    clock_t time_begin = clock();

    for (int i = 0; i < n_threads; ++i) {

        check = pthread_create (&threads[i], NULL, start_routine, (void*) &data[i]);
        if (check != 0) {
            perror ("pthread_create");
            exit(1);
        }
    }

    void* part_res;
    double res = 0;
    for (int i = 0; i < n_threads; ++i) {

        check = pthread_join(threads[i], &part_res);
        if (check != 0) {
            perror("pthread_join");
            exit(1);
        }

        res += *((double*)part_res);
    }

    clock_t time_end = clock();
    double time_spent = (time_end - time_begin) / CLOCKS_PER_SEC;
    printf("res = %f\nexecuted time = %f\n", res, time_spent);
    return 0;
}

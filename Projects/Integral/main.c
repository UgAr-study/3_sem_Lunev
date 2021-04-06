#define _GNU_SOURCE
#include <sched.h>
#include "cpu_info.h"
#include "calc.h"

double foo (double x) {
    return x * x;
}

static const double begin = 0.0;
static const double end   = 200.0;

void* start_routine (void* arg) {

    struct thread_info* params = arg;
    params->res = CalcIntegral(params->begin, params->end, foo);
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        fprintf(stderr, "Not all args\n");
        return 0;
    }

    // number of threads we need to calculate into
    int n_threads = (int) strtol (argv[1], NULL, 10);

    // number of cpus we have on this system
    int n_cpus = get_nprocs();
    int n_thread_create = n_threads > n_cpus ? n_threads : n_cpus;

    printf ("n_threads = %d\nn_thread_create = %d\n", n_threads, n_thread_create);
    struct cpu_info cpuInfo = get_mycpu_info();

    struct thread_info init = {
            .begin = begin,
            .end = end,
    };

    pthread_t* threads = (pthread_t*) malloc (n_thread_create * sizeof (pthread_t));
    pthread_attr_t* attrs = (pthread_attr_t*) calloc(n_thread_create, sizeof(pthread_attr_t));
    struct thread_info** infosp = build_cache_aligned_thread_info(n_thread_create);

    if (threads == NULL || attrs == NULL || infosp == NULL) {
        fprintf(stderr, "allocation memory error\n");
        exit(EXIT_FAILURE);
    }

    if (n_threads <= n_thread_create)
        set_attrs(cpuInfo, attrs, n_thread_create);

    fill_thread_info(infosp, n_thread_create, init, n_threads);

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpuInfo.cpus[0].processors[0], &cpu_set);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set);

    int ret = -1;
    for (int i = 0; i < n_thread_create; ++i) {

        if (i == 32980) {
            printf("Nooooooooo\n");
        }

        ret = pthread_create (&threads[i], &attrs[i], start_routine, (void*) infosp[i]);
        if (ret != 0) {
            perror ("pthread_create");
            free (cpuInfo.cpus);
            free(infosp[0]);
            free (infosp);
            free (threads);
            free (attrs);
            exit(1);
        }
    }

    double res = 0;
    for (int i = 0; i < n_threads; ++i) {

        ret = pthread_join(threads[i], NULL);
        if (ret != 0) {
            perror("pthread_join");
            free (cpuInfo.cpus);
            free(infosp[0]);
            free (infosp);
            free (threads);
            free (attrs);
            exit(1);
        }
        res += infosp[i]->res;
    }

    printf("res = %f\n", res);

    for (int i = 0; i < cpuInfo.n_cpu; ++i)
        free (cpuInfo.cpus[i].processors);

    for (int i = 0; i < n_thread_create; ++i)
        pthread_attr_destroy(&attrs[i]);

    free (cpuInfo.cpus);
    free(infosp[0]);
    free (infosp);
    free (threads);
    free (attrs);
    return 0;
}
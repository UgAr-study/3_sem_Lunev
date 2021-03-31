#define _GNU_SOURCE
#include <sched.h>
#include "cpu_info.h"
#include "calc.h"

double foo (double x) {
    return x * x;
}

void* start_routine (void* arg) {

    struct thread_info* params = arg;
    params->res = CalcIntegral(params->begin, params->end, params->n_steps, params->foo);
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        printf("This system has %d processors configured and "
               "%d processors available. size of cache line is %ld "
               "size of page is %ld\n",
               get_nprocs_conf(), get_nprocs(), sysconf (_SC_LEVEL1_DCACHE_LINESIZE), sysconf (_SC_PAGESIZE));
        printf("ncpus returned %d\n", ncpus());
        fprintf(stderr, "Not all args\n");
        return 0;
    }

    const double begin = 0.0;
    const double end   = 500.0;
    const size_t n_steps  = 500000000;

    // number threads we need to create except our main thread
    int n_threads = (int) strtol (argv[1], NULL, 10);
    struct cpu_info cpuInfo = get_mycpu_info();

    if (n_threads == 1) {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(cpuInfo.cpus[0].processors[0], &cpu_set);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set);

        printf("res = %f\n", CalcIntegral(begin, end, n_steps, foo));
        return 0;
    }

    const double step = (end - begin) / (n_threads);
    const double my_begin = begin;
    const double my_end = my_begin + step;
    const size_t my_n_steps = n_steps / (n_threads);

    struct thread_info init = {
            .begin = my_end,
            .end = end,
            .n_steps = n_steps - my_n_steps,
            .foo = foo
    };

    pthread_t* threads = (pthread_t*) malloc ((n_threads - 1) * sizeof (pthread_t));
    pthread_attr_t* attrs = (pthread_attr_t*) malloc(sizeof(pthread_attr_t) * (n_threads - 1));

    set_attrs(cpuInfo, attrs, n_threads - 1);

    struct thread_info** infosp = build_cache_aligned_thread_info(n_threads - 1);

    fill_thread_info(infosp, n_threads - 1, init);

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpuInfo.cpus[0].processors[0], &cpu_set);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set);

    for (int i = 0; i < n_threads - 1; ++i) {

        int check = pthread_create (&threads[i], &attrs[i], start_routine, (void*) infosp[i]);
        if (check != 0) {
            perror ("pthread_create");
            exit(1);
        }
    }

    double res = CalcIntegral(my_begin, my_end, my_n_steps, foo);
    for (int i = 0; i < n_threads - 1; ++i) {

        int check = pthread_join(threads[i], NULL);
        if (check != 0) {
            perror("pthread_join");
            exit(1);
        }
    }

    for (int i = 0; i < n_threads - 1; ++i)
        res += infosp[i]->res;

    printf("res = %f\n", res);

    for (int i = 0; i < cpuInfo.n_cpu; ++i)
        free (cpuInfo.cpus[i].processors);

    free (cpuInfo.cpus);
    free (infosp);
    free (threads);
    free (attrs);
    return 0;
}

/*
 * cpu_info = get_cpu_info;
 * set_attrs(cpu_info, pthread_arrt* attrs, size_t size); //size = n_threads
 * struct thread_info** infos = get_aligned_chunks(size_t n_threads);
 */


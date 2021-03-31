#define _GNU_SOURCE
#include <sched.h>
#include "cpu_info.h"
#include "calc.h"

double foo (double x) {
    return x * x;
}

//void *(*start_routine) (void *);

static const double begin = 0.0;
static const double end   = 500.0;

void* start_routine (void* arg) {

    struct thread_info* params = arg;
    params->res = CalcIntegral(params->begin, params->end, foo);
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

    // number of threads we need to calculate into
    int n_threads = (int) strtol (argv[1], NULL, 10);

    // number of cpus we have on this system
    int n_cpus = get_nprocs();
    int n_thread_create = n_threads > n_cpus ? n_threads : n_cpus;

    struct cpu_info cpuInfo = get_mycpu_info();

    struct thread_info init = {
            .begin = begin,
            .end = end,
    };

    pthread_t* threads = (pthread_t*) malloc (n_thread_create * sizeof (pthread_t));
    pthread_attr_t* attrs = (pthread_attr_t*) malloc(sizeof(pthread_attr_t) * n_thread_create);

    set_attrs(cpuInfo, attrs, n_thread_create);

    struct thread_info** infosp = build_cache_aligned_thread_info(n_thread_create);

    fill_thread_info(infosp, n_thread_create, init, n_threads);

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpuInfo.cpus[0].processors[0], &cpu_set);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set);

    for (int i = 0; i < n_thread_create; ++i) {

        int check = pthread_create (&threads[i], &attrs[i], start_routine, (void*) infosp[i]);
        if (check != 0) {
            perror ("pthread_create");
            exit(1);
        }
    }

    double res = 0;
    for (int i = 0; i < n_threads; ++i) {

        int check = pthread_join(threads[i], NULL);
        if (check != 0) {
            perror("pthread_join");
            exit(1);
        }
        res += infosp[i]->res;
    }

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

/*
 * если больше, то создаем столько сколько есть и в функцию заполнения инфы передаем количество сколько надо создать (size of arr)
 * и сколько всего есть, и разность заполняем первым промежутком.
 */
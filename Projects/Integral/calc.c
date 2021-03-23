//
// Created by artem on 01.03.2021.
//

#include "calc.h"

double CalcIntegral (double begin, double end, size_t n_steps, double (*foo) (double x)) {

    if (end < begin) {
        double tmp = end;
        end = begin;
        begin = tmp;
    }

    //debug info
    printf ("do from [%f] to [%f]\n", begin, end);

    const double step = (end - begin) / n_steps;

    double res = 0;
    double x2 = begin + step;
    double f1 = foo (begin);
    double f2 = foo (x2);

    for (int i = 0; i < n_steps; ++i) {

        res += step * (f1 + f2) / 2;
        f1 = f2;
        x2 += step;
        f2 = foo (x2);
    }

    return res;
}


void set_attrs (struct cpu_info cpuInfo, pthread_attr_t* attrs, size_t size) {
    //size = n_threads & size of attrs
    //i = 1, cause the 0th cpu is under our main thread
    int i = 0;
    for (; i < size && i + 1 < cpuInfo.real_threads_size; ++i) {
        cpu_set_t cpuSet;
        CPU_ZERO(&cpuSet);
        CPU_SET(cpuInfo.real_threads[i + 1], &cpuSet);
        pthread_attr_setaffinity_np(&attrs[i], 1, &cpuSet);
    }

    int j = 0;
    for (; i + j < size && j < cpuInfo.hyper_threads_size; ++j) {
        cpu_set_t cpuSet;
        CPU_ZERO(&cpuSet);
        CPU_SET(cpuInfo.hyper_threads[j], &cpuSet);
        pthread_attr_setaffinity_np(&attrs[i + j], 1, &cpuSet);
    }

    i += j;

    size_t n_threads = cpuInfo.hyper_threads_size + cpuInfo.real_threads_size;
    int tmp = i;

    while (i < size) {
        cpu_set_t cpuSet;
        CPU_ZERO(&cpuSet);
        CPU_SET((i - tmp) % n_threads, &cpuSet);
        pthread_attr_setaffinity_np(&attrs[i], 1, &cpuSet);
        ++i;
    }
}

struct thread_info** build_cache_aligned_thread_info (size_t n_threads) {

    long cache_line_size = sysconf (_SC_LEVEL1_DCACHE_LINESIZE);
    long page_size = sysconf (_SC_PAGESIZE);
    size_t one_info_size = (sizeof (struct thread_info) / cache_line_size + 1) * cache_line_size;

    char* aligned_mem = (char*) memalign(page_size, n_threads * one_info_size);
    if (aligned_mem == NULL) {
        perror("aligned allocation failed");
        exit(EXIT_FAILURE);
    }

    struct thread_info** infosp = (struct thread_info**) malloc(sizeof(struct thread_info*) * n_threads);

    for (int i = 0; i < n_threads; ++i)
        infosp[i] = (struct thread_info *) (aligned_mem + i * one_info_size);

    return infosp;
}

void fill_info (struct thread_info** infosp, size_t size, struct thread_info init) {

    const double interval = (init.end - init.begin) / (double) size;
    double th_begin = init.begin;
    double th_end   = init.begin + interval;
    const size_t th_n_steps = init.n_steps / size;

    for (int i = 0; i < size - 1; ++i) {

        infosp[i]->begin   = th_begin;
        infosp[i]->end     = th_end;
        infosp[i]->n_steps = th_n_steps;
        infosp[i]->foo     = init.foo;

        th_begin = th_end;
        th_end += interval;
    }

    infosp[size - 1]->begin   = th_begin;
    infosp[size - 1]->end     = init.end;
    infosp[size - 1]->n_steps = init.n_steps - (size - 1) * th_n_steps;
    infosp[size - 1]->foo     = init.foo;
}

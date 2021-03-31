//
// Created by artem on 01.03.2021.
//

#include "calc.h"

const double step = 1e-7;

double CalcIntegral (double begin, double end, double (*foo) (double x)) {

    if (end < begin) {
        double tmp = end;
        end = begin;
        begin = tmp;
    }

    //debug info
    //printf ("do from [%f] to [%f]\n", begin, end);

    double res = 0;
    double x = begin;
    double f1 = foo (x);
    double f2 = foo (x + step);

    while (x < end) {
        res += step * (f1 + f2) / 2;
        x += step;
        f1 = f2;
        f2 = foo (x);
    }

    //printf ("begin [%f] finished\n", begin);
    return res;
}


void set_attrs (struct cpu_info cpu_info, pthread_attr_t* attrs, size_t size) {
    // we begin from 1 because 0 cpu 0 proc is under our main thread
    for (int i = 1; i <= size; ++i) {
        size_t num_cpu = i % cpu_info.n_cpu;
        size_t num_proc = (i / cpu_info.n_cpu) % cpu_info.cpus[num_cpu].n_proc;
        cpu_set_t  cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(cpu_info.cpus[num_cpu].processors[num_proc], &cpu_set);
        pthread_attr_setaffinity_np(&attrs[i - 1], sizeof(cpu_set_t), &cpu_set);
    }
}

struct thread_info** build_cache_aligned_thread_info (size_t n_threads) {

    long cache_line_size = sysconf (_SC_LEVEL1_DCACHE_LINESIZE);
    long page_size = sysconf (_SC_PAGESIZE);
    size_t one_info_size = (sizeof (struct thread_info) / cache_line_size + 1) * cache_line_size;

    char* aligned_mem = (char*) memalign(page_size, n_threads * one_info_size);
    if (aligned_mem == NULL) {
        return NULL;
    }

    struct thread_info** infosp = (struct thread_info**) malloc(sizeof(struct thread_info*) * n_threads);
    if (infosp == NULL) {
        free (aligned_mem);
        return NULL;
    }

    for (int i = 0; i < n_threads; ++i)
        infosp[i] = (struct thread_info *) (aligned_mem + i * one_info_size);

    return infosp;
}

void fill_thread_info (struct thread_info** infosp, size_t info_size,
                       struct thread_info init, size_t n_threads) {

    const double interval = (init.end - init.begin) / (double) n_threads;

    double th_begin = init.begin;
    double th_end   = init.begin + interval;

    const double c_begin = th_begin;
    const double c_end = th_end;

    for (int i = 0; i < n_threads - 1; ++i) {

        infosp[i]->begin   = th_begin;
        infosp[i]->end     = th_end;
        th_begin = th_end;
        th_end += interval;
    }

    infosp[n_threads - 1]->begin   = th_begin;
    infosp[n_threads - 1]->end     = init.end;

    //for threads, which must work for no reason
    for (int i = n_threads; i < info_size; ++i) {
        infosp[i]->begin = c_begin;
        infosp[i]->end = c_end;
    }
}
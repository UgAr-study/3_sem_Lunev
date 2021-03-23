//
// Created by artem on 23.03.2021.
//
#pragma once

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <malloc.h>
#include <unistd.h>
#include "cpu_info.h"

struct thread_info {
    double begin;
    double end;
    size_t n_steps;
    double (*foo) (double x);
    double res;
};

double CalcIntegral (double begin, double end, size_t n_steps, double (*foo) (double x));
void set_attrs (struct cpu_info cpuInfo, pthread_attr_t* attrs, size_t size);
struct thread_info** build_cache_aligned_thread_info (size_t n_threads);
void fill_info (struct thread_info** infosp, size_t size, struct thread_info init);

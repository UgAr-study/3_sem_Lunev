//
// Created by artem on 17.03.2021.
//
#pragma once

#include <stdio.h>
#include <stdlib.h>


struct cpu_info {
    int* real_threads;
    int* hyper_threads;
    size_t real_threads_size;
    size_t hyper_threads_size;
};

struct cpu_info get_mycpu_info ();

int ncpus(void);
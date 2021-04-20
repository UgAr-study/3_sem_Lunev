//
// Created by artem on 23.03.2021.
//
#pragma once


#include <stdio.h>
#include <stdlib.h>
#include "pthread_manager.h"

enum errors {E_SUCCESS, E_BADARGS, E_MEMLACK, E_THREAD};

double CalcIntegral (double begin, double end, double (*foo) (double x));
int Integrate (const int n_threads, const double begin, const double end, double (*foo) (double), double* result);

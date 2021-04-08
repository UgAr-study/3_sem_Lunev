#define _GNU_SOURCE
#include <sched.h>
#include "pthread_manager.h"
#include "calc.h"

double foo (double x) {
    return x * x;
}

static const double begin = 0.0;
static const double end   = 200.0;

int main(int argc, char* argv[]) {

    if (argc == 1) {
        fprintf(stderr, "Not all args\n");
        return 0;
    }

    int n_threads = (int) strtol (argv[1], NULL, 10);

    double res = 0;

    if (Integrate(n_threads, begin, end, foo, &res) != E_SUCCESS) {
        fprintf (stderr, "Error occurred! Check logs\n");
    } else {
        printf("res = %f\n", res);
    }

    return 0;
}
#include <stdio.h>

#include "test.c"

void foo () {
    int *a = (int*) CALLOC (6, 1);
    if (a == NULL)
        printf ("a is NULL\n");
    else {
        printf("a is not NULL\n");
        free(a);
    }

}

void test1() {
    foo();
}

void test2() {
    foo();
}


int main (int argc, char *argv[]) {

    test1();
    test2();
    return 0;

}
//
// Created by artem on 08.02.2021.
//
#include <assert.h>
#include "tester.h"

struct Pair arr_1[10] = {{13, 1},
                         {8, 1},
                         {17, 1},
                         {1, 1},
                         {25, 1},
                         {11, 1},
                         {15, 1},
                         {6, 1},
                         {22, 1},
                         {27, 1}};

struct Array testInput () {

    int n = 0;
    scanf ("%d", &n);

    struct Pair* a = (struct Pair*) calloc (n, sizeof(struct Pair));

    for (int i = 0; i < n; ++i) {
        scanf("%d %d", &a[i].key, &a[i].value);
    }

    struct Array res;
    res.size = n;
    res.arr = a;
    return res;
}

void test_all () {
    test_calloc();
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    test_7();
    test_8();
    test_9();
    test_10();
    test_11();
    test_12();
    test_13();
    test_14();
    test_15();
    test_16();
    test_17();
    test_18();
    test_19();
    my_test();
    test_20();
    test_21();
}


void sumAll (struct Node* el, void *data) {
    *((int*)data) += getValue(el, NULL);
}

void test_1 () {
    struct Pair arr[6] =
            {{4, 0},
             {3, 0},
             {7, 0},
             {5, 0},
             {10, 0},
             {12, 0}};

    struct Map* map = createMap(arr, 6);
    assert (addItem(map, arr[1]) == SUCCESS);
    assert (printMap(map) == SUCCESS);
    assert (deleteMap(map) == SUCCESS);
}

void test_2 () {
    struct Array data = testInput();
    struct Map* map = createMap(data.arr, data.size);
    assert(printMap(map) == SUCCESS);
    assert(deleteMap(map) == SUCCESS);
    free(data.arr);
}

void test_3() {

    struct Map* map = createMap(NULL, 1);

    assert(deleteItem(map, 1) == INVALID_ARG);
    assert(deleteMap(map) == SUCCESS);
}

void test_4 () {
    struct Pair arr = {2, 1};

    struct Map* map = createMap(&arr, 1);
    struct Node* node = findItem(map, arr.key);
    assert (getValue(node, NULL) == 1);
    assert (deleteMap(map) == SUCCESS);
}

void test_5 () {
    struct Pair p = {0, 0};

    assert (addItem(NULL, p) == INVALID_ARG);

    struct Pair arr = {2, 3};

    struct Map* map = createMap(&arr, 1);

    assert (!isEmpty(map));

    int sum = 0;
    foreach(map, sumAll, &sum);

    assert (sum == 3);

    assert (deleteItem(map, arr.key) == SUCCESS);
    assert (isEmpty(map));
    assert (deleteMap(map) == SUCCESS);
}

void test_6 () {

    assert (printMap(NULL) == INVALID_ARG);
    int err = SUCCESS;
    getValue(NULL, &err);
    assert (err == INVALID_ARG);

    assert (isEmpty(NULL) == INVALID_ARG);
    assert (setValue(NULL, 1) == INVALID_ARG);
    assert (clearMap(NULL) == INVALID_ARG);
    assert (foreach(NULL, NULL, NULL) == INVALID_ARG);
    assert (deleteMap(NULL) == INVALID_ARG);
    assert (deleteItem(NULL, 1) == INVALID_ARG);

    struct Map* map = createMap(NULL, 1);
    assert (map);

    assert (deleteItem(map, 1) == INVALID_ARG);
    assert (deleteMap(map) == SUCCESS);
}

void test_7 () {

    struct Pair arr[6] =
            {{4, 1},
             {3, 1},
             {7, 1},
             {5, 1},
             {10, 1},
             {12, 1}};


    struct Map* map = createMap(arr, 6);

    assert (map);
    assert (deleteItem(map, arr[0].key) == SUCCESS);
    assert (findItem(map, arr[0].key) == NULL);

    assert (deleteItem(map, 20) == SUCCESS);
    assert (printMap(map) == SUCCESS);
    assert (deleteMap(map) == SUCCESS);
}

void test_8 () {

    struct Map* map = createMap(arr_1, 10);

    assert (map);
    assert (deleteItem(map, arr_1[7].key) == SUCCESS);
    assert (findItem(map, arr_1[7].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_9 () {

    struct Map* map = createMap(arr_1, 10);

    assert (map);
    assert (deleteItem(map, arr_1[3].key) == SUCCESS);
    assert (findItem(map, arr_1[3].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_10 () {

    struct Map* map = createMap(arr_1, 10);

    assert (map);
    assert (deleteItem(map, arr_1[2].key) == SUCCESS);
    assert (findItem(map, arr_1[2].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_11 () {

    struct Map* map = createMap(arr_1, 10);

    assert (map);
    assert (deleteItem(map, arr_1[4].key) == SUCCESS);
    assert (findItem(map, arr_1[4].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_12 () {

    struct Pair arr[8] =
            {{7, 1},
             {3, 1},
             {18, 1},
             {22, 1},
             {10, 1},
             {11, 1},
             {8, 1},
             {26, 1}};

    struct Map* map = createMap(arr, 8);

    assert (map);
    assert (deleteItem(map, arr[2].key) == SUCCESS);
    assert (findItem(map, arr[2].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_13 () {

    struct Pair arr[8] =
                 {{1, 1},
                  {2, 1},
                  {4, 1},
                  {5, 1},
                  {8, 1},
                  {7, 3},
                  {9, 1},
                  {10, 1}};

    struct Map* map = createMap(arr, 8);
    assert (map);

    assert (deleteItem(map, arr[7].key) == SUCCESS);
    assert (findItem(map, arr[7].key) == NULL);

    assert (deleteItem(map, arr[5].key) == SUCCESS);
    assert (findItem(map, arr[5].key) == NULL);

    assert (addItem(map, arr[5]) == SUCCESS);
    assert (getValue(findItem(map, arr[5].key), NULL) == 3);

    assert (deleteItem(map, arr[1].key) == SUCCESS);
    assert (findItem(map, arr[1].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_14 () {

    struct Map* map = createMap(arr_1, 10);

    assert (map);
    assert (deleteItem(map, arr_1[0].key) == SUCCESS);
    assert (findItem(map, arr_1[0].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_15 () {

    struct Map* map = createMap(arr_1, 10);

    assert (map);
    assert (deleteItem(map, arr_1[1].key) == SUCCESS);
    assert (findItem(map, arr_1[1].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_16 () {

    struct Pair arr[8] =
            {{7, 1},
             {3, 1},
             {18, 1},
             {22, 1},
             {10, 1},
             {11, 1},
             {8, 1},
             {26, 1}};

    struct Map* map = createMap(arr, 8);

    assert (map);

    assert (deleteItem(map, arr[1].key) == SUCCESS);
    assert (findItem(map, arr[1].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_17 () {

    struct Map* map = createMap(arr_1, 10);

    assert (map);
    assert (deleteItem(map, arr_1[5].key) == SUCCESS);
    assert (findItem(map, arr_1[5].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_18 () {

    struct Pair arr[2] =
            {{3, 1},
             {2, 1}};

    struct Map* map = createMap(arr, 2);

    assert (map);
    assert (deleteItem(map, arr[0].key) == SUCCESS);
    assert (findItem(map, arr[0].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_19 () {

    struct Pair arr[4] =
            {{5, 1},
             {2, 1},
             {8, 1},
             {1, 1}};

    struct Map* map = createMap (arr, 4);
    assert (deleteItem(map, arr[3].key) == SUCCESS);
    assert (findItem(map, arr[3].key) == NULL);

    assert (deleteItem(map, arr[0].key) == SUCCESS);
    assert (findItem(map, arr[0].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void test_calloc () {

    struct Pair arr[2] =
            {{5, 1},
             {2, 1}};

    struct Map* map = createMap(NULL, 0);
    assert (!map);

    map = createMap(arr, 1);
    assert (!map);

    map = createMap(arr, 2);

    assert (map);

    assert (deleteItem(map, arr[1].key) == SUCCESS);
    assert (findItem(map, arr[1].key) == NULL);
    assert (deleteMap(map) == SUCCESS);
}

void my_test() {
    struct Pair arr[2] =
            {{5, 1},
             {2, 1}};

    struct Map* map = createMap(arr, 2);
    assert (map);

    struct Node* n = findItem(map, arr[1].key);
    assert (n);
    assert (setValue(n, 15) == SUCCESS);
    assert (getValue(findItem(map, arr[1].key), NULL) == 15);

    assert (deleteItem(map, arr[1].key) == SUCCESS);
    assert (findItem(map, arr[1].key) == NULL);
    assert (clearMap(map) == SUCCESS);
    assert (isEmpty(map));
    assert (deleteMap(map) == SUCCESS);
}

void test_20() {

    struct Pair arr[4] =
            {{5, 1},
             {1, 1},
             {8, 1},
             {7, 1}};

    struct Map* map = createMap(arr, 4);

    deleteItem(map, arr[1].key);

    deleteMap(map);
}

void test_21() {
    struct Pair arr[8] =
            {{10, 1},
             {5, 1},
             {16, 1},
             {20, 1},
             {2, 1},
             {8, 1},
             {12, 1},
             {13, 1}};

    struct Map* map = createMap(arr, 8);

    deleteItem(map, arr[0].key);

    deleteMap(map);
}
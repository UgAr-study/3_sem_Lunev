//
// Created by artem on 08.02.2021.
//
#include "tester.h"

struct Array testInput () {

    int n = 0;
    scanf ("%d", &n);

    int *a = (int*) calloc (n, sizeof(int));

    for (int i = 0; i < n; ++i) {
        scanf("%d", &a[i]);
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
}


int sumAll (struct Node* el, void *data) {
    *((int*)data) += *getData(el);
    return 0;
}

void test_1 () {
    int arr[6] = {4, 3, 7, 5, 10, 12};

    struct Array data;
    data.size = 6;
    data.arr = arr;

    struct Map* map = createMap(&data);
    addItem(map, &arr[1]);
    printMap(map);
    deleteMap(map);
}

void test_2 () {
    struct Array data = testInput();
    struct Map* map = createMap(&data);
    printMap(map);
    deleteMap(map);
    free(data.arr);
}

void test_3() {
    struct Array data;
    data.size = 1;
    data.arr = NULL;
    struct Map* map = createMap(&data);
    deleteItem(map, NULL);
    deleteItem(map, (int*)&data.size);
    deleteMap(map);
}

void test_4 () {
    int arr[1] = {2};
    struct Array data;
    data.size = 1;
    data.arr = arr;
    struct Map* map = createMap(&data);
    struct Node* node = findItem(map, &arr[0]);
    if (node == NULL)
        fprintf(stderr, "Test №4: fail\n");
    deleteMap(map);
}

void test_5 () {
    addItem(NULL, NULL);

    int arr[1] = {2};

    struct Array data;
    data.size = 1;
    data.arr = arr;

    struct Map* map = createMap(&data);

    if (isEmpty(map))
        fprintf(stderr, "Test №5: fail\n");

    addItem(map, NULL);

    int sum = 0;
    foreach(map, sumAll, &sum);

    if (sum != 2)
        fprintf(stderr, "Test №5: fail\n");

    deleteItem(map, &arr[0]);
    deleteMap(map);
}

void test_6 () {
    int a = 0;
    printMap(NULL);
    printError(NULL);
    clearError(NULL);
    getData(NULL);
    isEmpty(NULL);
    foreach(NULL, sumAll, NULL);
    foreach(NULL, NULL, NULL);
    deleteMap(NULL);
    deleteItem(NULL, NULL);
    struct Map* map = createMap(NULL);
    deleteItem(map, &a);
    deleteMap(map);
}

void test_7 () {

    int arr[6] = {4, 3, 7, 5, 10, 12};

    int a = 20;

    struct Array data;
    data.size = 6;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[0]);
    deleteItem(map, &a);
    printMap(map);
    deleteMap(map);
}

void test_8 () {

    int arr[10] = {13, 8, 17, 1, 25, 11, 15, 6, 22, 27};

    struct Array data;
    data.size = 10;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[7]);
    deleteMap(map);
}

void test_9 () {

    int arr[10] = {13, 8, 17, 1, 25, 11, 15, 6, 22, 27};

    struct Array data;
    data.size = 10;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[3]);
    deleteMap(map);

}

void test_10 () {

    int arr[10] = {13, 8, 17, 1, 25, 11, 15, 6, 22, 27};

    struct Array data;
    data.size = 10;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[2]);
    deleteMap(map);
}

void test_11 () {

    int arr[10] = {13, 8, 17, 1, 25, 11, 15, 6, 22, 27};

    struct Array data;
    data.size = 10;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[4]);
    deleteMap(map);
}

void test_12 () {

    int arr[8] = {7, 3, 18, 22, 10, 11, 8, 26};

    struct Array data;
    data.size = 8;
    data.arr = arr;

    struct Map* map = createMap(&data);

    deleteItem(map, &arr[2]);
    deleteMap(map);
}

void test_13 () {

    int arr[8] = {1, 2, 4, 5, 8, 7, 9, 10};

    struct Array data;
    data.size = 8;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[7]);
    deleteItem(map, &arr[5]);
    addItem(map, &arr[5]);

    deleteItem(map, &arr[1]);
    deleteMap(map);
}

void test_14 () {

    int arr[10] = {13, 8, 17, 1, 25, 11, 15, 6, 22, 27};

    struct Array data;
    data.size = 10;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[0]);
    deleteMap(map);
}

void test_15 () {

    int arr[10] = {13, 8, 17, 1, 25, 11, 15, 6, 22, 27};

    struct Array data;
    data.size = 10;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[1]);
    deleteMap(map);
}

void test_16 () {

    int arr[8] = {7, 3, 18, 22, 10, 11, 8, 26};

    struct Array data;
    data.size = 8;
    data.arr = arr;

    struct Map* map = createMap(&data);

    deleteItem(map, &arr[1]);
    deleteMap(map);
}

void test_17 () {

    int arr[10] = {13, 8, 17, 1, 25, 11, 15, 6, 22, 27};

    struct Array data;
    data.size = 10;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[5]);
    deleteMap(map);
}

void test_18 () {

    int arr[2] = {3, 2};

    struct Array data;
    data.size = 2;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[0]);
    printError(map);
    deleteMap(map);
}

void test_19 () {

    int arr[4] = {5, 2, 8, 1};

    struct Array data;
    data.size = 4;
    data.arr = arr;

    struct Map* map = createMap(&data);
    deleteItem(map, &arr[3]);
    deleteItem(map, &arr[0]);
    deleteMap(map);
}

void test_calloc () {

    int arr[2] = {5, 2};

    struct Array data;
    data.size = 1;
    data.arr = arr;

    struct Map* map = createMap(NULL);
    printf ("\nmap is %p\n", map);

    map = createMap(&data);
    printf ("\nmap is %p\n", map);
    printError(map);
    clearError(map);

    data.size = 2;
    map = createMap(&data);

    deleteItem(map, &arr[1]);
    printError(map);
    clearError(map);
    deleteMap(map);
}

void my_test() {
    int arr[2] = {5, 2};

    struct Array data;
    data.size = 2;
    data.arr = arr;

    struct Map* map = createMap(&data);

    deleteItem(map, &arr[1]);
    deleteMap(map);
}
//
// Created by artem on 07.02.2021.
//

#pragma once

#include <stdlib.h>
#include <stdio.h>



//TODO:
// вместо findItem сделать получение по ключу.
// сделать нормальный map, а не эту непонятную хуйню.
// сделать влаг листа (для фиктивного листа при удалении)
// или же посмотреть нормальный алгоритм удаления,
// учитывающий, что листы это не объъекты
// сделать тест на удаление единственной ноды.
// Изменить конструктор (не нужна там структура Array).

struct Node;
struct Map;

struct Array {
    int* arr;
    size_t size;
};

struct Pair{
    int key;
    int value;
};

struct Map* createMap (struct Array* data);

int foreach (struct Map* map, int (*foo)(struct Node* el, void* data), void* data);
struct Node* findItem (struct Map* map, int* item);
int* getData (struct Node* node);
void deleteItem (struct Map* map, int *item);
void addItem (struct Map* map, int* item);
void printError (struct Map* map);
void printMap (struct Map* map);
void deleteMap (struct Map* map);
int isEmpty (struct Map* map);
void clearError (struct Map* map);
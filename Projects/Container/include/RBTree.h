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

enum error_t {SUCCESS = 0, LACK_OF_MEMORY, INVALID_ARG};

struct Pair{
    int key;
    int value;
};

struct Map* createMap (struct Pair* data, size_t size);

int foreach (struct Map* map, void (*foo)(struct Node* el, void* data), void* data);
struct Node* findItem (struct Map* map, int key);
int getValue (struct Node* node, int* error);
int setValue (struct Node* node, int value);
int deleteItem (struct Map* map, int key);

/*
 * return SUCCESS if item was added successfully, LACK_OF_MEMORY if calloc returned null
 */
int addItem (struct Map* map, struct Pair item);
int printMap (struct Map* map);
int deleteMap (struct Map* map);
int clearMap (struct Map* map);
int isEmpty (struct Map* map);

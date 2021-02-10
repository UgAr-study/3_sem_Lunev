//
// Created by artem on 07.02.2021.
//

#pragma once

#include <stdlib.h>
#include <stdio.h>

enum error_t {LACK_OF_MEMORY = 1, NOT_THE_ONLY_CHILD};
enum color_t {BLACK, RED};

struct Node;
struct Map;

struct Array {
    int* arr;
    size_t size;
};

struct Map createMap (struct Array data);

int foreach (struct Map map, int (*foo)(struct Node el, void* data), void* data);
struct Node* findItem (struct Map map, int* item);
void deleteItem (struct Map* map, int *item, int* error);
void addItem (struct Map* map, int* item, int* error);
void printMap (struct Map map);
void deleteMap (struct Map map);
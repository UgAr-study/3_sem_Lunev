//
// Created by artem on 07.02.2021.
//

#pragma once

#include <stdlib.h>
#include <stdio.h>


struct Node;
struct Map;

struct Array {
    int* arr;
    size_t size;
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
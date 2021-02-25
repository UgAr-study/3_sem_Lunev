//
// Created by artem on 07.02.2021.
//

#pragma once

#include <stdlib.h>
#include <stdio.h>

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

int addItem (struct Map* map, struct Pair item);
int printMap (struct Map* map);
int deleteMap (struct Map* map);
int clearMap (struct Map* map);
int isEmpty (struct Map* map);

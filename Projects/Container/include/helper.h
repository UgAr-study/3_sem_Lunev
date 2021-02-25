//
// Created by artem on 25.02.2021.
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#ifdef TEST
#define CALLOC(x, y) calloc_h(x, y)
#else
#define CALLOC(x, y) calloc(x, y)
#endif

void* calloc_h(size_t nnum, size_t size);

struct Node* findTop (struct Node* node);
struct Node* findParent (struct Node* tree, int key);

int isInTree (struct Map* map, int key);
struct Node* deleteNode (struct Node* node);
void insert (struct Node* parent, struct Node* node);

void foreach_h (struct Node* tree, void (*foo)(struct Node* el, void* data), void* data);
void deleteTree (struct Node* tree);
void printTreeWithIndents (struct Node* tree, int indents);
//
// Created by artem on 25.02.2021.
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "RBTree.h"


#ifdef TEST
#define CALLOC(x, y) calloc_h(x, y)
#else
#define CALLOC(x, y) calloc(x, y)
#endif

void* calloc_h(size_t nnum, size_t size);

struct Node* getTreeRoot (struct Map* map);
void setTreeRoot (struct Map* map, struct Node* root);

struct Map* createMap_h ();
struct Node* createNode_h (int key, int value, struct Node* parent, struct Node* left, struct Node* right);
int getKey_h (struct Node* node);
int getValue_h (struct Node* node);
struct Node* getLeftChild (struct Node* node);
struct Node* getRightChild (struct Node* node);
void setValue_h (struct Node* node, int value);

struct Node* findTop (struct Node* node);
struct Node* findParent (struct Node* tree, int key);

int isInTree (struct Map* map, int key);
struct Node* deleteNode (struct Node* node);
void insert (struct Node* parent, struct Node* node);

void foreach_h (struct Node* tree, void (*foo)(struct Node* el, void* data), void* data);
void deleteTree (struct Node* tree);
void printTreeWithIndents (struct Node* tree, int indents);
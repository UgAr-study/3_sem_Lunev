//
// Created by artem on 07.02.2021.
//

#pragma once

#include <stdlib.h>
#include <stdio.h>

enum error_t {LACK_OF_MEMORY, NOT_THE_ONLY_CHILD};
enum color_t {BLACK, RED};

struct Node {
    struct Node *parent, *left, *right;
    enum color_t color;
    int* data;
    unsigned key;
};

int foreach (struct Node* tree, int (*foo)(struct Node el, void* data), void* data);
struct Node* findItem (struct Node* node, int* item);
struct Node* deleteItem (struct Node* tree, int *item);
struct Node* addItem (struct Node* tree, int* item);
void printTree (struct Node* tree);
void deleteTree (struct Node* tree);
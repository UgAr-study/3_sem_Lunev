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
    int data;
};

struct Node* findTop (struct Node* node);
struct Node* findParent (struct Node* tree, int item);
struct Node* findGrandparent(struct Node *node);
struct Node* findUncle(struct Node *node);
struct Node* findBrother (struct Node* node);

void leftRotation(struct Node *node);
void rightRotation(struct Node *node);

void insert (struct Node* parent, struct Node* node);
void insert_case1(struct Node* node);
void insert_case2(struct Node* node);
void insert_case3(struct Node* node);
void insert_case4(struct Node* node);
void insert_case5(struct Node* node);

void replaceWithChild (struct Node* node, struct Node* child);

int deleteTheOnlyChild(struct Node* node);
void delete_case1 (struct Node* node);
void delete_case2 (struct Node* node);
void delete_case3 (struct Node* node);
void delete_case4 (struct Node* node);
void delete_case5 (struct Node* node);
void delete_case6 (struct Node* node);

struct Node* addItem (struct Node* tree, int item);
void printTreeWithIndents (struct Node* tree, int indents);
void printTree (struct Node* tree);
void deleteTree (struct Node* tree);
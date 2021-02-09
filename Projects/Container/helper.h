//
// Created by artem on 10.02.2021.
//
#pragma once


struct Node* findTop (struct Node* node);
struct Node* findParent (struct Node* tree, unsigned key);
struct Node* findGrandparent(struct Node *node);
struct Node* findUncle(struct Node *node);
struct Node* findBrother (struct Node* node);


int isInTree (struct Node* node, int* item);
void leftRotation(struct Node *node);
void rightRotation(struct Node *node);


void replaceWithChild (struct Node* node, struct Node* child);


struct Node* deleteNode (struct Node* node);
int deleteTheOnlyChild(struct Node* node);
void delete_case1 (struct Node* node);
void delete_case2 (struct Node* node);
void delete_case3 (struct Node* node);
void delete_case4 (struct Node* node);
void delete_case5 (struct Node* node);
void delete_case6 (struct Node* node);


void insert (struct Node* parent, struct Node* node);
void insert_case1(struct Node* node);
void insert_case2(struct Node* node);
void insert_case3(struct Node* node);
void insert_case4(struct Node* node);
void insert_case5(struct Node* node);


void printTreeWithIndents (struct Node* tree, int indents);

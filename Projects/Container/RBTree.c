//
// Created by artem on 07.02.2021.
//

#include "RBTree.h"

struct Node* findTop (struct Node* node) {

    if (node == NULL)
        return NULL;

    struct Node* res = node;

    while (res->parent)
        res = res->parent;

    return res;
}


struct Node* findParent (struct Node* tree, int item) {

    if (tree == NULL)
        return NULL;

    if (tree->data >= item) {
        if (tree->left)
            return findParent(tree->left, item);
        else
            return tree;
    } else {
        if (tree->right)
            return findParent(tree->right, item);
        else
            return tree;
    }
}

struct Node* findGrandparent(struct Node *node) {

    if ((node != NULL) && (node->parent != NULL))
        return node->parent->parent;
    else
        return NULL;
}

struct Node* findUncle(struct Node* node) {

    struct Node* grandpa = findGrandparent(node);

    if (grandpa == NULL)
        return NULL;

    if (node->parent == grandpa->left)
        return grandpa->right;
    else
        return grandpa->left;
}

void leftRotation(struct Node *node) {

    struct Node* pivot = node->right;

    pivot->parent = node->parent; /* при этом, возможно, pivot становится корнем дерева */
    if (node->parent != NULL) {
        if (node->parent->left == node)
            node->parent->left = pivot;
        else
            node->parent->right = pivot;
    }

    node->right = pivot->left;
    if (pivot->left != NULL)
        pivot->left->parent = node;

    node->parent = pivot;
    pivot->left = node;
}

void rightRotation(struct Node *node) {

    struct Node* pivot = node->left;

    pivot->parent = node->parent; /* при этом, возможно, pivot становится корнем дерева */
    if (node->parent != NULL) {
        if (node->parent->left == node)
            node->parent->left = pivot;
        else
            node->parent->right = pivot;
    }

    node->left = pivot->right;
    if (pivot->right != NULL)
        pivot->right->parent = node;

    node->parent = pivot;
    pivot->right = node;
}

/*
 * function inserts new node in tree if it is unique;
 * the top of the tree can change, so it returns the top
 * of the updated tree
 */
struct Node* addItem (struct Node* tree, int item) {

    struct Node* node = (struct Node*) calloc (1, sizeof(struct Node));
    if (node == NULL) {
        return NULL;
    }

    node->data = item;
    node->color = RED;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    struct Node* parent = findParent (tree, item);

    insert (parent, node);

    return findTop (node);
}

void insert (struct Node* parent, struct Node* node) {

    node->parent = parent;

    if (parent == NULL) {
        insert_case1(node);
        return;
    }

    if (parent->data > node->data)
        parent->left = node;
    else
        parent->right = node;

    insert_case2(node);
}

void insert_case1(struct Node* node) {

    if (node->parent == NULL)
        node->color = BLACK;
    else
        insert_case2(node);
}

void insert_case2(struct Node* node) {

    if (node->parent->color == BLACK)
        return; /* Tree is still valid */
    else
        insert_case3(node);
}

void insert_case3(struct Node* node) {

    struct Node *uncle = findUncle(node), *grandpa;

    if ((uncle != NULL) && (uncle->color == RED)) {
        // && (n->parent->color == RED) Второе условие проверяется в insert_case2, то есть родитель уже является красным.
        node->parent->color = BLACK;
        uncle->color = BLACK;
        grandpa = findGrandparent(node);
        grandpa->color = RED;
        insert_case1(grandpa);
    } else {
        insert_case4(node);
    }
}

void insert_case4(struct Node* node) {

    struct Node* grandpa = findGrandparent(node);

    if ((node == node->parent->right) && (node->parent == grandpa->left)) {
        leftRotation(node->parent);

        /*
         * rotate_left может быть выполнен следующим образом, учитывая что уже есть *g =  grandparent(n)
         *
         * struct node *saved_p=g->left, *saved_left_n=n->left;
         * g->left=n;
         * n->left=saved_p;
         * saved_p->right=saved_left_n;
         *
         */

        node = node->left;
    } else if ((node == node->parent->left) && (node->parent == grandpa->right)) {
        rightRotation(node->parent);

        /*
         * rotate_right может быть выполнен следующим образом, учитывая что уже есть *g =  grandparent(n)
         *
         * struct node *saved_p=g->right, *saved_right_n=n->right;
         * g->right=n;
         * n->right=saved_p;
         * saved_p->left=saved_right_n;
         *
         */

        node = node->right;
    }
    insert_case5(node);
}


void insert_case5(struct Node* node) {

    struct Node* grandpa = findGrandparent(node);

    node->parent->color = BLACK;
    grandpa->color = RED;

    if ((node == node->parent->left) && (node->parent == grandpa->left)) {
        rightRotation(grandpa);
    } else { /* (n == n->parent->right) && (n->parent == g->right) */
        leftRotation(grandpa);
    }
}

void deleteTree (struct Node* tree) {

    if (tree == NULL)
        return;

    if (tree->left)
        deleteTree(tree->left);
    if (tree->right)
        deleteTree(tree->right);

    free (tree);
}


static void writeIndents (int indents) {
    for (int i = 0; i < indents; ++i)
        printf (":::|");
}

static void printNode (struct Node* node, int indents) {
    writeIndents(indents);
    printf ("%d\n", node->data);
}


void printTree (struct Node* tree) {
    printTreeWithIndents(tree, 0);
}

void printTreeWithIndents (struct Node* tree, int indents) {

    if (tree == NULL)
        return;

    printNode(tree, indents);

    if (tree->left)
        printTreeWithIndents(tree->left, indents + 1);
    if (tree->right)
        printTreeWithIndents(tree->right, indents + 1);

}


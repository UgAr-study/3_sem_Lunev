//
// Created by artem on 07.02.2021.
//

#include "RBTree.h"
#include "helper.h"

//                              Hash function
//==========================================================================================

unsigned getHash (int* data) {
    return (unsigned long)data;
}

//                              Find functions
//==========================================================================================

int isInTree (struct Node* node, int* item) {
    return findItem(node, item) != NULL;
}


struct Node* findItem (struct Node* node, int* item) {

    if (item == NULL || node == NULL)
        return NULL;

    struct Node* tree = findTop(node);

    struct Node* parent = findParent(tree, getHash(item));

    if (parent == NULL) {
        if (tree->data == item)
            return tree;
        else
            return NULL;
    }

    if (parent->right && parent->right->data == item)
        return parent->right;

    if (parent->left && parent->left->data == item)
        return parent->left;

    return NULL;
}


struct Node* findTop (struct Node* node) {

    if (node == NULL)
        return NULL;

    struct Node* res = node;

    while (res->parent)
        res = res->parent;

    return res;
}


struct Node* findParent (struct Node* tree, unsigned key) {

    if (tree == NULL)
        return NULL;

    if (tree->key > key) {
        if (tree->left)
            return findParent(tree->left, key);
        else
            return tree;
    } else if (tree->key < key){
        if (tree->right)
            return findParent(tree->right, key);
        else
            return tree;
    } else
        return tree->parent;
}

struct Node* findGrandparent(struct Node *node) {

    if ((node != NULL) && (node->parent != NULL))
        return node->parent->parent;
    else
        return NULL;
}

struct Node* findUncle (struct Node* node) {

    struct Node* grandpa = findGrandparent(node);

    if (grandpa == NULL)
        return NULL;

    if (node->parent == grandpa->left)
        return grandpa->right;
    else
        return grandpa->left;
}

struct Node* findBrother (struct Node* node) {

    if (node == NULL || node->parent == NULL)
        return NULL;

    if (node == node->parent->left)
        return node->parent->right;
    else
        return node->parent->left;
}

struct Node* findMax (struct Node* tree) {

    if (tree == NULL)
        return NULL;

    if (tree->right)
        return findMax (tree->right);
    else
        return tree;
}

struct Node* findMin (struct Node* tree) {

    if (tree == NULL)
        return NULL;

    if (tree->left)
        return findMin (tree->left);
    else
        return tree;
}


//                              Rotate functions
//==========================================================================================




void leftRotation (struct Node* node) {

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




//                              Inserting element
//==========================================================================================



/*
 * function inserts new node in tree if it is unique;
 * the top of the tree can change, so it returns the top
 * of the updated tree
 */
struct Node* addItem (struct Node* tree, int* item) {

    tree = findTop(tree);

    if (item == NULL)
        return tree;

    if (isInTree(tree, item)) {
        return tree;
    }

    struct Node* node = (struct Node*) calloc (1, sizeof(struct Node));
    if (node == NULL) {
        return NULL;
    }

    node->data = item;
    node->color = RED;
    node->key = getHash(item);
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    struct Node* parent = findParent (tree, node->key);

    insert (parent, node);

    return findTop (node);
}

void insert (struct Node* parent, struct Node* node) {

    node->parent = parent;

    if (parent == NULL) {
        insert_case1(node);
        return;
    }

    if (parent->key > node->key)
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




//                              Deleting element
//==========================================================================================

struct Node* deleteItem (struct Node* tree, int *item) {

    tree = findTop(tree);

    if (item == NULL || tree == NULL)
        return tree;

    struct Node* node = findItem(tree, item);

    if (node)
        return deleteNode (node);
    else
        return NULL;
}

struct Node* deleteNode (struct Node* node) {

    if (node == NULL)
        return NULL;

    struct Node* M, *leaf = NULL;
    struct Node* tmp = node;

    if (node->left)
        M = findMax (node->left);
    else if (node->right)
        M = findMin (node->right);
    else {
        tmp = node->parent;
        M = node;
        //TODO:
        printf ("I got in TODO if!\n");
        //return NULL; // not final version, need to be done!
    }

    if (M->left == NULL && M->right == NULL) {

        leaf = (struct Node*) calloc (1, sizeof(struct Node));

        M->right = leaf;

        if (M->right == NULL)
            return node; //return the error code

        M->right->right = NULL;
        M->right->left  = NULL;
        M->right->data  = NULL;
        M->right->key = 0;
        M->right->parent = node;
        M->right->color = BLACK;
    }

    node->data = M->data;
    node->key  = M->key;

    deleteTheOnlyChild(M);
    //TODO:
    if (leaf) {

        if (leaf->parent) {
            if (leaf == leaf->parent->left)
                leaf->parent->left = NULL;
            else
                leaf->parent->right = NULL;
        }

        free(leaf);
    }

    return findTop(tmp);
}


int deleteTheOnlyChild(struct Node* node) {
    /*
     * Условие: n имеет не более одного ненулевого потомка.
     */

    if (node->left && node->right)
        return NOT_THE_ONLY_CHILD;

    struct Node *child;

    if (node->right == NULL)
        child = node->left;
    else
        child = node->right;

    replaceWithChild (node, child);
    if (node->color == BLACK) {
        if (child->color == RED)
            child->color = BLACK;
        else
            delete_case1(child);
    }

    free(node);
    return 0;
}


void replaceWithChild (struct Node* node, struct Node* child) {

    if (node == NULL)
        return;

    /*if (child != node->left && child != node->right)
        return;*/

    child->parent = node->parent;

    if (node->parent == NULL) {
        //TODO:
        printf ("I got in TODO if!\n");
        return;
    }

    if (node == node->parent->left) {
        node->parent->left = child;
    } else {
        node->parent->right = child;
    }
}


void delete_case1 (struct Node* node)
{
    if (node->parent != NULL)
        delete_case2(node);
}

void delete_case2 (struct Node* node) {

    struct Node* brother = findBrother (node);

    if (brother->color == RED) {
        node->parent->color = RED;
        brother->color = BLACK;

        if (node == node->parent->left)
            leftRotation (node->parent);
        else
            rightRotation (node->parent);
    }

    delete_case3 (node);
}

void delete_case3 (struct Node* node) {

    struct Node *brother = findBrother (node);

    if ((node->parent->color == BLACK) &&
        (brother->color == BLACK) &&
        (brother->left == NULL || brother->left->color == BLACK) &&
        (brother->right == NULL || brother->right->color == BLACK)) {

        brother->color = RED;
        delete_case1(node->parent);
    } else
        delete_case4(node);
}

void delete_case4 (struct Node* node) {

    struct Node *brother = findBrother (node);

    if ((node->parent->color == RED) &&
        (brother->color == BLACK) &&
        (brother->left == NULL  || brother->left->color == BLACK) &&
        (brother->right == NULL || brother->right->color == BLACK))  {

        brother->color = RED;
        node->parent->color = BLACK;
    } else
        delete_case5(node);
}

void delete_case5 (struct Node* node) {

    struct Node *brother = findBrother (node);

    if  (brother->color == BLACK) { /* this if statement is trivial,
due to case 2 (even though case 2 changed the sibling to a sibling's child,
the sibling's child can't be red, since no red parent can have a red child). */
/* the following statements just force the red to be on the left of the left of the parent,
   or right of the right, so case six will rotate correctly. */
        if ((node == node->parent->left) &&
            (brother->right == NULL || brother->right->color == BLACK) &&
            (brother->left && brother->left->color == RED)) { /* this last test is trivial too due to cases 2-4. */

            brother->color = RED;
            brother->left->color = BLACK;
            rightRotation(brother);

        } else if ((node == node->parent->right) &&
                   (brother->left == NULL || brother->left->color == BLACK) &&
                   (brother->right && brother->right->color == RED)) { /* this last test is trivial too due to cases 2-4. */

            brother->color = RED;
            brother->right->color = BLACK;
            leftRotation(brother);
        }
    }

    delete_case6(node);
}

void delete_case6 (struct Node* node) {

    struct Node* brother = findBrother (node);

    brother->color = node->parent->color;
    node->parent->color = BLACK;

    if (node == node->parent->left) {
        brother->right->color = BLACK;
        leftRotation (node->parent);
    } else {
        brother->left->color = BLACK;
        rightRotation (node->parent);
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

//                              Foreach functions
//==========================================================================================

int foreach (struct Node* tree, int (*foo)(struct Node el, void* data), void* data) {

    if (tree == NULL)
        return 1;

    foreach(tree->left, foo, data);
    foreach(tree->right, foo, data);
    return foo (*tree, data);
}


//                              Dump functions
//==========================================================================================



static void writeIndents (int indents) {
    for (int i = 0; i < indents; ++i)
        printf (":::|");
}

static void printNode (struct Node* node, int indents) {
    writeIndents(indents);
    if (node == NULL)
        printf ("NULL\n");
    else
        printf ("%d\n", *node->data);
}


void printTree (struct Node* tree) {
    printTreeWithIndents(tree, 0);
}

void printTreeWithIndents (struct Node* tree, int indents) {

    printNode(tree, indents);

    if (tree == NULL)
        return;

    printTreeWithIndents(tree->left, indents + 1);
    printTreeWithIndents(tree->right, indents + 1);
}


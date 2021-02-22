//
// Created by artem on 16.02.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#ifdef TEST
#define CALLOC(x, y) calloc_h(x, y)
#else
#define CALLOC(x, y) calloc(x, y)
#endif

void* calloc_h(size_t nnum, size_t size) {
    static int a = 0;

    if (a == 0 || a == 2) {
        a++;
        return NULL;
    }

    a++;
    return calloc (nnum,size);
}



enum error_t {SUCCESS = 0, LACK_OF_MEMORY};
enum color_t {BLACK, RED};

struct Node {
    struct Node *parent, *left, *right;
    enum color_t color;
    int value;
    int key;
};

struct Map {
    struct Node* treeRoot;
};


struct Node* findItem (struct Map* map, int* item);
struct Node* findTop (struct Node* node);
struct Node* findParent (struct Node* tree, unsigned key);
struct Node* findGrandparent(struct Node *node);
struct Node* findUncle(struct Node *node);
struct Node* findBrother (struct Node* node);


int isInTree (struct Map* map, int* item);
void leftRotation(struct Node *node);
void rightRotation(struct Node *node);


void replaceWithChild (struct Node* node, struct Node* child);


struct Node* deleteNode (struct Node* node, int* error);
void deleteTheOnlyChild(struct Node* node);
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


int foreach_h (struct Node* tree, int (*foo)(struct Node* el, void* data), void* data);
void deleteTree (struct Node* tree);
void printTreeWithIndents (struct Node* tree, int indents);


//                              Hash function
//==========================================================================================

/*
unsigned getHash (int* data) {
    return (unsigned long)data;
}*/


unsigned getHash (const int * const  data) {
    return (unsigned) *data;
}



//                              Find functions
//==========================================================================================

int isInTree (struct Map* map, int* item) {
    return findItem(map, item) != NULL;
}


struct Node* findTop (struct Node* node) {

    if (node == NULL)
        return NULL;

    struct Node* res = node;

    while (res->parent)
        res = res->parent;

    return res;
}

/*
 * parameters:
 *      {tree} the root
 *      {key}  the key of item, which parent we want to find
 *
 * returns:
 *      the pointer to appropriate node, if the the parent was found,
 *      the NULL vice versa.
 *
 */
struct Node* findParent (struct Node* tree, unsigned key) {

    if (tree == NULL)
        return NULL;

    if (tree->key > key) {
        if (tree->left)
            return findParent(tree->left, key);
        else
            return tree;
    } else {
        if (tree->right)
            return findParent(tree->right, key);
        else
            return tree;
    }
}

struct Node* findGrandparent(struct Node *node) {

    //if ((node != NULL) && (node->parent != NULL))
        return node->parent->parent;
    //else
    //    return NULL;
}

struct Node* findUncle (struct Node* node) {

    struct Node* grandpa = findGrandparent(node);

    /*if (grandpa == NULL)
        return NULL;
*/
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

    /*if (tree == NULL)
        return NULL;
*/
    if (tree->right)  //TODO: may be it is better to use "while" here
        return findMax (tree->right);
    else
        return tree;
}

struct Node* findMin (struct Node* tree) {

    if (tree == NULL)
        return NULL;

    if (tree->left) //TODO: may be it is better to use "while" here
        return findMin (tree->left);
    else
        return tree;
}


//                              Rotate functions
//==========================================================================================




void leftRotation (struct Node* node) {

    struct Node* pivot = node->right;

    pivot->parent = node->parent; // and pivot can become the root of tree
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

    pivot->parent = node->parent; // and pivot can become the root of tree
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
        node = node->left;
    } else if ((node == node->parent->left) && (node->parent == grandpa->right)) {

        rightRotation(node->parent);
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
    } else {
        leftRotation(grandpa);
    }
}





//                              Deleting element
//==========================================================================================




struct Node* deleteNode (struct Node* node, int* error) {

    struct Node* M, *leaf = NULL;
    struct Node* tmp = node;

    if (node->right)
        M = findMin (node->right);
    else if (node->left)
        M = findMax (node->left);
    else {
        tmp = node->parent;
        M = node;
    }

    /*if (M->left == NULL && M->right == NULL) {

        leaf = (struct Node*) CALLOC(1, sizeof(struct Node));

        M->right = leaf;

        if (M->right == NULL) {
            *error = LACK_OF_MEMORY;
            return findTop(node);
        }

        M->right->parent = node;
        M->right->right  = NULL;
        M->right->left   = NULL;
        M->right->data   = NULL;
        M->right->key    = 0;
        M->right->color  = BLACK;
    }*/

    node->data = M->data;
    node->key  = M->key;

    deleteTheOnlyChild(M);

    /*if (leaf) {

        if (leaf->parent) {
            if (leaf == leaf->parent->left)
                leaf->parent->left = NULL;
            else
                leaf->parent->right = NULL;
        }

        free(leaf);
    }*/

    return findTop(tmp);
}


void deleteTheOnlyChild(struct Node* node) {

#ifdef DEBUG
    assert (node->left == NULL || node->right == NULL);
#endif

    struct Node *child;

    if (node->left == NULL && node->right == NULL) {

        if (node->color == BLACK)
            delete_case1(node);

        if (node->parent) {
            if (node == node->parent->left)
                node->parent->left = NULL;
            else
                node->parent->right = NULL;
        }

        free(node);
        return;
    }

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
}


void replaceWithChild (struct Node* node, struct Node* child) {

    /*if (node == NULL)
        return;
*/
#ifdef DEBUG
    assert (child);
#endif

    child->parent = node->parent;

    if (node->parent == NULL) {
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

    if  (brother->color == BLACK) {

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


int foreach_h (struct Node* tree, int (*foo)(struct Node* el, void* data), void* data) {

    if (tree == NULL)
        return 1;

    foreach_h(tree->left, foo, data);
    foreach_h(tree->right, foo, data);
    return foo (tree, data);
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
        printf ("NULL(B)\n");
    else {
        printf("%d", *(node->data));

        if (node->color == RED)
            printf("(R)\n");
        else
            printf("(B)\n");
    }
}

void printTreeWithIndents (struct Node* tree, int indents) {

    printNode(tree, indents);

    if (tree == NULL)
        return;

    printTreeWithIndents(tree->left, indents + 1);
    printTreeWithIndents(tree->right, indents + 1);
}

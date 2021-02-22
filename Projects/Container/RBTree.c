//
// Created by artem on 07.02.2021.
//

#include "RBTree.h"
#include "helper.c"




//                              Create function
//==========================================================================================

/*
 * function creates Map with elements from data.
 * if LACK_OF_MEMORY occurs, then it will return NULL,
 * otherwise the pointer on the created Map will be returned
 */
struct Map* createMap (struct Pair* data, size_t size) {

    struct Map* map = (struct Map*) CALLOC(1, sizeof(struct Map));

    if (map == NULL)
        return NULL;

    map->treeRoot = NULL;

    if (data == NULL || size == 0)
        return map;

    for (size_t i = 0; i < size; ++i) {

        if (addItem(map, data[i]) != 0) {
            deleteMap(map);
            return NULL;
        }
    }

    return map;
}


//                              Find functions
//==========================================================================================

/*
 * function tries to find item in map.
 * If it is found, then the pointer on it will be returned,
 * otherwise NULL will be returned
 */

struct Node* findItem (struct Map* map, int key) {

    if (map->treeRoot == NULL)
        return NULL;

    struct Node* tree = map->treeRoot;

    struct Node* tmp = tree;

    while (tmp) {
        if (tmp->key > key)
            tmp = tmp->left;
        else if (tmp->key < key)
            tmp = tmp->right;
        else
            return tmp;
    }

    return NULL;
}


//                              Inserting element
//==========================================================================================



/*
 * function inserts new item in map if it is unique;
 */
int addItem (struct Map* map, struct Pair item) {

    if (map == NULL)
        return -1;

    struct Node* tree = map->treeRoot;

    if (isInTree(map, item)) {
        return 0;
    }

    struct Node* node = (struct Node*) CALLOC(1, sizeof(struct Node));

    if (node == NULL) {
        map->error = LACK_OF_MEMORY;
        return;
    }

    node->data = item;
    node->color = RED;
    node->key = getHash(item);
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    struct Node* parent = findParent (tree, node->key);

    insert (parent, node);

    map->treeRoot = findTop(node);
}





//                              Deleting element
//==========================================================================================


/*
 * function deletes item from the map;
 */

void deleteItem (struct Map* map, int* item) {

    if (item == NULL || map == NULL)
        return;

    struct Node* tree = map->treeRoot;

    if (tree == NULL)
        return;

    struct Node* node = findItem(map, item);

    if (node) {
        int error = SUCCESS;
        map->treeRoot = deleteNode(node, &error);
        map->error = error;
    }
    else // TODO: may be return the error code
        return;
}


void deleteMap (struct Map* map) {
    if (map == NULL)
        return;

    deleteTree(map->treeRoot);
    free (map);
}


//                              Foreach functions
//==========================================================================================

int foreach (struct Map* map, int (*foo)(struct Node* el, void* data), void* data) {

    if (map == NULL || foo == NULL)
        return -1;

    return foreach_h(map->treeRoot, foo, data);
}


//                              Dump functions
//==========================================================================================

void printError (struct Map* map) {
    if (map == NULL)
        return;

    switch (map->error) {

        case SUCCESS:
            fprintf (stderr, "No errors\n");
            break;
        case LACK_OF_MEMORY:
            fprintf (stderr, "Lack of memory\n");
            break;
    }
}

void printMap (struct Map* map) {
    if (map == NULL)
        return;

    printTreeWithIndents(map->treeRoot, 0);
}


//                              Interface functions
//==========================================================================================



int* getData (struct Node* node) {

    if (node == NULL)
        return NULL;

    return node->data;
}

int isEmpty (struct Map* map) {

    if (map == NULL)
        return 1;

    return map->treeRoot == NULL;
}

void clearError (struct Map* map) {

    if (map == NULL)
        return;

    map->error = SUCCESS;
}

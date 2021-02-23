//
// Created by artem on 07.02.2021.
//

#include "../include/RBTree.h"
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

        if (addItem(map, data[i]) != SUCCESS) {
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
        return INVALID_ARG;

    struct Node* tree = map->treeRoot;

    if (isInTree(map, item.key)) {
        return SUCCESS;
    }

    struct Node* node = (struct Node*) CALLOC(1, sizeof(struct Node));

    if (node == NULL) {
        return LACK_OF_MEMORY;
    }

    node->value = item.value;
    node->key = item.key;
    node->color = RED;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    struct Node* parent = findParent (tree, node->key);

    insert (parent, node);

    map->treeRoot = findTop(node);

    return SUCCESS;
}





//                              Deleting element
//==========================================================================================


/*
 * function deletes item from the map;
 * if the map or tree root is NULL then it returns INVALID_ARG error
 * else SUCCESS is returned
 */

int deleteItem (struct Map* map, int key) {

    if (map == NULL)
        return INVALID_ARG;

    struct Node* tree = map->treeRoot;

    if (tree == NULL)
        return INVALID_ARG;

    struct Node* node = findItem(map, key);

    if (node)
        map->treeRoot = deleteNode(node);

    return SUCCESS;
}

int clearMap (struct Map* map) {

    if (map == NULL)
        return INVALID_ARG;

    deleteTree(map->treeRoot);
    map->treeRoot = NULL;
    return SUCCESS;
}

int deleteMap (struct Map* map) {

    if (map == NULL)
        return INVALID_ARG;

    deleteTree(map->treeRoot);
    free (map);
    return SUCCESS;
}


//                              Foreach functions
//==========================================================================================

int foreach (struct Map* map, void (*foo)(struct Node* el, void* data), void* data) {

    if (map == NULL || foo == NULL)
        return INVALID_ARG;

    foreach_h(map->treeRoot, foo, data);
    return SUCCESS;
}


//                              Dump functions
//==========================================================================================


int printMap (struct Map* map) {

    if (map == NULL)
        return INVALID_ARG;

    printTreeWithIndents(map->treeRoot, 0);

    return SUCCESS;
}


//                              Interface functions
//==========================================================================================



int getValue (struct Node* node, int* error) {

    if (node == NULL) {
        if (error)
            *error = INVALID_ARG;
        return 0;
    }

    return node->value;
}

int setValue (struct Node* node, int value) {

    if (node == NULL)
        return INVALID_ARG;

    node->value = value;
    return SUCCESS;
}

int isEmpty (struct Map* map) {

    if (map == NULL)
        return INVALID_ARG;

    return map->treeRoot == NULL;
}

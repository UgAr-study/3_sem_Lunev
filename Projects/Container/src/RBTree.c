//
// Created by artem on 07.02.2021.
//

#include "RBTree.h"
#include "helper.h"




//                              Create function
//==========================================================================================

/*
 * function creates Map with elements from data.
 * if LACK_OF_MEMORY occurs, then it will return NULL,
 * otherwise the pointer on the created Map will be returned
 */
struct Map* createMap (struct Pair* data, size_t size) {

    struct Map* map = createMap_h();

    if (map == NULL)
        return NULL;

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

struct Node* findItem (struct Map* map, int key, int* error) {

    if (map == NULL) {

        if (error)
            *error = INVALID_ARG;

        return NULL;
    }

    struct Node* tree = getTreeRoot(map);

    if (tree == NULL)
        return NULL;

    struct Node* tmp = tree;

    while (tmp) {
        if (getKey_h(tmp) > key)
            tmp = getLeftChild(tmp);
        else if (getKey_h(tmp) < key)
            tmp = getRightChild(tmp);
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

    struct Node* tree = getTreeRoot(map);

    if (isInTree(map, item.key)) {
        return SUCCESS;
    }

    struct Node* node = createNode_h (item.key, item.value, NULL, NULL, NULL);

    if (node == NULL) {
        return LACK_OF_MEMORY;
    }

    struct Node* parent = findParent (tree, getKey_h(node));

    insert (parent, node);

    setTreeRoot(map, findTop(node));

    return SUCCESS;
}





//                              Deleting function
//==========================================================================================


/*
 * function deletes item from the map;
 * if the map or tree root is NULL then it returns INVALID_ARG error
 * else SUCCESS is returned
 */

int deleteItem (struct Map* map, int key) {

    if (map == NULL)
        return INVALID_ARG;

    struct Node* tree = getTreeRoot(map);

    if (tree == NULL)
        return INVALID_ARG;

    struct Node* node = findItem(map, key, NULL);

    if (node)
        setTreeRoot(map, deleteNode(node));

    return SUCCESS;
}

int clearMap (struct Map* map) {

    if (map == NULL)
        return INVALID_ARG;

    deleteTree(getTreeRoot(map));
    setTreeRoot(map, NULL);
    return SUCCESS;
}

int deleteMap (struct Map* map) {

    if (map == NULL)
        return INVALID_ARG;

    deleteTree(getTreeRoot(map));
    free (map);
    return SUCCESS;
}


//                              Foreach functions
//==========================================================================================

int foreach (struct Map* map, void (*foo)(struct Node* el, void* data), void* data) {

    if (map == NULL || foo == NULL)
        return INVALID_ARG;

    foreach_h(getTreeRoot(map), foo, data);
    return SUCCESS;
}


//                              Dump functions
//==========================================================================================


int printMap (struct Map* map) {

    if (map == NULL)
        return INVALID_ARG;

    printTreeWithIndents(getTreeRoot(map), 0);

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

    return getValue_h (node);
}

int getKey (struct Node* node, int* error) {

    if (node == NULL) {
        if (error)
            *error = INVALID_ARG;
        return 0;
    }

    return getKey_h (node);
}

int setValue (struct Node* node, int value) {

    if (node == NULL)
        return INVALID_ARG;

    setValue_h (node, value);
    return SUCCESS;
}

int isEmpty (struct Map* map) {

    if (map == NULL)
        return INVALID_ARG;

    return getTreeRoot(map) == NULL;
}

#include <assert.h>
#include "RBTree.h"
#include "tester.h"

int sumAll (struct Node el, void *data) {
    *((int*)data) += *(el.data);
    return 0;
}

int main() {

    int data1[5] = {11, 10, 13, 12, 14};

    struct Array data_1;
    data_1.size = 5;
    data_1.arr = data1;

    struct Map map = createMap(data_1);
    struct Node* tree1 = NULL;
    for (int i = 0; i < 5; ++i) {
        tree1 = addItem(tree1, &data1[i], NULL);
    }

    tree1->right->color = RED;
    tree1->right->left->color = BLACK;
    tree1->right->right->color = BLACK;
    //printTree(tree1);
    //return 0;
    tree1 = deleteItem(tree1, &data1[1], NULL);
    deleteTree(tree1);

    struct array data = testInput();
    assert (data.arr);

    int a = 1, b = 3;

    struct Node* tree = NULL;

    int error = 0;

    tree = addItem(tree, NULL, &error);

    tree = addItem(tree, &a, &error);
    tree = addItem(tree, &b, &error);
    tree = deleteItem(tree, &a, &error);
    tree = deleteItem(tree, &b, &error);
    tree = deleteItem(NULL, NULL, NULL);


    for (int i = 0; i < data.size; ++i)
        tree = addItem(tree, &data.arr[i], &error);

    tree = deleteItem(tree, &error, &error);
    tree = addItem(tree, &data.arr[0], &error);

    for (int i = 0; i < data.size; ++i)
        tree = deleteItem(tree, &data.arr[i], &error);

    for (int i = 0; i < data.size; ++i)
        tree = addItem(tree, &data.arr[i], &error);

    int sum = 0;
    foreach(tree, sumAll, &sum);
    printf ("sum = %d\n", sum);

    if (tree)
        printTree(tree);

    free(data.arr);
    deleteTree (tree);
    deleteTree (NULL);
    return 0;
}

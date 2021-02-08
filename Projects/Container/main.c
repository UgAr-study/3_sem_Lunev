#include <assert.h>
#include "RBTree.h"
#include "tester.h"

int main() {

    struct array data = testInput();
    assert (data.arr);

    struct Node* tree = NULL;

    for (int i = 0; i < data.size; ++i)
        tree = addItem(tree, data.arr[i]);

    if (!tree)
        printf ("Ooops...\n");
    else
        printTree(tree);


    deleteTree (tree);
    return 0;
}

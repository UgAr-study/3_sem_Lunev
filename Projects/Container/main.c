#include <assert.h>
#include "RBTree.h"
#include "tester.h"

int sumAll (struct Node el, void *data) {
    *((int*)data) += *(el.data);
    return 0;
}

int main() {

    struct array data = testInput();
    assert (data.arr);

    int a = 9;

    struct Node* tree = NULL;

    for (int i = 0; i < data.size; ++i)
        tree = addItem(tree, &data.arr[i]);

    int sum = 0;
    foreach(tree, sumAll, &sum);
    printf ("sum = %d\n", sum);


    free(data.arr);
    deleteTree (tree);
    return 0;
}

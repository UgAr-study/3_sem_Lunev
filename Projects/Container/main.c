#include <stdio.h>
#include "RBTree.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return 0;
    }

    int* arr = testFromFile (argv[1]);

    struct Node* tree;

    tree = addItem(NULL, 10);


    if (!tree)
        printf ("Ooops...\n");
    else
        printTree(tree);

    deleteTree (tree);
    return 0;
}

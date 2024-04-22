#include <stdio.h>

#include "ds/bp.h"

int main(void) {
    // Test Insertion
    bp_tree_t *tree = BPT_CreateTree();

    BPT_Insert(tree, 1, "Hello", 5);
    BPT_Insert(tree, 2, "World", 5);
    BPT_Insert(tree, 3, "Foo", 3);
    BPT_Insert(tree, 4, "Bar", 3);
    BPT_Insert(tree, 5, "Baz", 3);
    BPT_Insert(tree, 6, "Qux", 3);
    BPT_Insert(tree, 7, "Quux", 4);
    BPT_Insert(tree, 8, "Corge", 5);

    BPT_DisplayTree(tree);
}
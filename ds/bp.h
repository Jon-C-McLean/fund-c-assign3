/**
 * @file bp.h
 * @author Jon McLean (13515869)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAX_DEGREE 3U
#define MAX_TREE_ENTRIES (2 * MAX_DEGREE)
#define MAX_TREE_CHILDREN (MAX_TREE_ENTRIES + 1)

typedef struct {
    int key;
    char* value;
    uint64_t length;
} bp_entry_t;

typedef struct bp_node {
    bp_entry_t *entries[MAX_TREE_ENTRIES];
    struct bp_node *nodes[MAX_TREE_CHILDREN];
    int nextIndex;
    int isLeaf;
} bp_node_t;

typedef struct {
    bp_node_t *root;
} bp_tree_t;

bp_tree_t* BPT_CreateTree(void);
int BPT_FreeTree(bp_tree_t *tree);
bp_entry_t* BPT_SearchKey(bp_tree_t *tree, int key);
void BPT_Insert(bp_tree_t *tree, int key, char* value, uint64_t length);

void BPT_DisplayTree(bp_tree_t *tree);

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

/**
 * @brief Create a new B+ Tree
 * @author Jon McLean (13515869)
 * 
 * @return bp_tree_t* The resulting B+ Tree
 */
bp_tree_t* BPT_CreateTree(void);

/**
 * @brief Free a B+ Tree and all associated memory
 * @author Jon McLean (13515869)
 * 
 * @param[in] tree The tree to free
 * @return int Status of the operation
 */
int BPT_FreeTree(bp_tree_t *tree);

/**
 * @brief Search for a key in the B+ Tree
 * @author Jon McLean (13515869)
 * 
 * @param[in] tree The tree to search
 * @param[in] key The key to search for
 * @return bp_entry_t* The entry found, or NULL if not found
 */
bp_entry_t* BPT_SearchKey(bp_tree_t *tree, int key);

/**
 * @brief Insert a new key into the B+ Tree
 * @author Jon McLean (13515869)
 * 
 * @param[in] tree The tree to insert into
 * @param[in] key The key to insert
 * @param[in] value The value to insert
 * @param[in] length The length of the value
 */
void BPT_Insert(bp_tree_t *tree, int key, char* value, uint64_t length);

/**
 * @brief Display the B+ Tree
 * @author Jon McLean (13515869)
 * 
 * @note This is for debugging purposes only
 * @param[in] tree The tree to display
 */
void BPT_DisplayTree(bp_tree_t *tree);

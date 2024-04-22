/**
 * @file bp.c
 * 
 */
#include "bp.h"

/*
 * Private Functions
*/
bp_node_t* BPT_CreateNode(void) {
    bp_node_t *node = (bp_node_t *)malloc(sizeof(bp_node_t));
    node->nextIndex = 0;
    node->isLeaf = 1;
    
    int i;
    for(i = 0; i < MAX_TREE_ENTRIES; i++) {
        node->entries[i] = NULL;
    }

    for(i = 0; i < MAX_TREE_CHILDREN; i++) {
        node->nodes[i] = NULL;
    }

    return node;
}

int BPT_FreeNode(bp_node_t *node) {
    if(node != NULL) {
        int i = 0;
        for(i = 0; i < MAX_TREE_ENTRIES; i++) {
            if(node->entries[i] != NULL) {
                free(node->entries[i]);
            }
        }

        for(i = 0; i < MAX_TREE_CHILDREN; i++) {
            if(node->nodes[i] != NULL) {
                BPT_FreeNode(node->nodes[i]);
            }
        }
    }

    free(node);
    return 0;
}

bp_entry_t* BPT_RecursiveSearch(bp_node_t *node, int key) {
    if(node == NULL) return NULL;

    int i = 0;
    while(i < MAX_TREE_ENTRIES && node->entries[i] != NULL && node->entries[i]->key < key) {
        i++;
    }

    if(i < MAX_TREE_ENTRIES && node->entries[i] != NULL && node->entries[i]->key == key) {
        return node->entries[i];
    }

    bp_node_t *child = node->nodes[i]; /* Don't need to check for NULL as this is handled at top of function */
    return BPT_RecursiveSearch(child, key);
}

void BPT_InsertIntoLeaf(bp_node_t *leafNode, bp_entry_t *newEntry) {
    int i = leafNode->nextIndex - 1;
    while (i >= 0 && (leafNode->entries[i] == NULL || leafNode->entries[i]->key > newEntry->key)) {
        if (i + 1 < MAX_TREE_ENTRIES) {
            leafNode->entries[i + 1] = leafNode->entries[i];
        }
        i--;
    }
    if (i + 1 < MAX_TREE_ENTRIES) {
        leafNode->entries[i + 1] = newEntry;
        leafNode->nextIndex++;
    }
}


void BPT_SplitLeafNode(bp_node_t *parentNode, int index, bp_node_t *leafNode) {
    bp_node_t *newLeafNode = BPT_CreateNode();
    newLeafNode->isLeaf = 1;

    int splitIndex = leafNode->nextIndex / 2;

    // Copy entries to the new leaf node
    int i = 0;
    for (i = splitIndex; i < leafNode->nextIndex; i++) {
        newLeafNode->entries[i - splitIndex] = leafNode->entries[i];
        leafNode->entries[i] = NULL;
    }

    newLeafNode->nextIndex = leafNode->nextIndex - splitIndex;
    leafNode->nextIndex = splitIndex;

    // Move child nodes in the parent node
    for (i = parentNode->nextIndex; i > index + 1; i--) {
        parentNode->nodes[i] = parentNode->nodes[i - 1];
    }

    parentNode->nodes[index + 1] = newLeafNode;
    parentNode->entries[index] = newLeafNode->entries[0];
    parentNode->nextIndex++;

    // Update parent node flag if necessary
    if (parentNode->isLeaf) {
        parentNode->isLeaf = 0; // Update parent node to indicate it's not a leaf anymore
    }
}


void BPT_DisplayTreeLevel(bp_node_t *node, int level) {
    if (node == NULL)
        return;

    int i, j;

    if (!node->isLeaf) {
        for (i = node->nextIndex; i >= 0; i--) {
            BPT_DisplayTreeLevel(node->nodes[i], level + 1);
            if (i < node->nextIndex) {
                for (j = 0; j < level; j++)
                    printf("\t");
                printf("|\n");
            }
        }
    }

    for (j = 0; j < level - 1; j++)
        printf("\t");

    if (level > 0)
        printf("|--");

    for (i = node->nextIndex - 1; i >= 0; i--) {
        printf("(%d, %s)", node->entries[i]->key, node->entries[i]->value);
        if (i > 0)
            printf(",");
    }
    printf("\n");
}

/*
 * Public Functions
*/
bp_tree_t* BPT_CreateTree(void) {
    bp_tree_t *tree = (bp_tree_t *)malloc(sizeof(bp_tree_t));
    tree->root = BPT_CreateNode(); /* Create root node */
    return tree;
}

int BPT_FreeTree(bp_tree_t *tree) {
    if (tree != NULL) {
        BPT_FreeNode(tree->root);
        free(tree);
    }
}

bp_entry_t* BPT_SearchKey(bp_tree_t *tree, int key) {
    return BPT_RecursiveSearch(tree->root, key); /* Perform recursive search of the tree */
}

void BPT_Insert(bp_tree_t *tree, int key, char* value, uint64_t length) {
    int i;

    if(tree->root == NULL) {
        tree->root = BPT_CreateNode();
    }

    bp_entry_t *entry = (bp_entry_t *)malloc(sizeof(bp_entry_t));
    entry->key = key;
    entry->value = value;
    entry->length = length;

    bp_node_t *currentNode = tree->root;
    if(currentNode->nextIndex == MAX_TREE_ENTRIES) {
        bp_node_t *newRoot = BPT_CreateNode();
        newRoot->isLeaf = 0;
        newRoot->nodes[0] = tree->root;
        tree->root = newRoot;
        BPT_SplitLeafNode(newRoot, 0, currentNode);
        currentNode = newRoot;
    }

    while(!currentNode->isLeaf) {
        i = currentNode->nextIndex - 1;

        while(i >= 0 && currentNode->entries[i]->key > entry->key) {
            i--;
        }

        i++;
        if(currentNode->nodes[i]->nextIndex == MAX_TREE_ENTRIES) {
            BPT_SplitLeafNode(currentNode, i, currentNode->nodes[i]);
            if(currentNode->entries[i]->key < entry->key) {
                i++;
            }
        }

        currentNode = currentNode->nodes[i];
    }

    // Check for duplicate key
    for (i = 0; i < currentNode->nextIndex; i++) {
        if (currentNode->entries[i]->key == entry->key) {
            // Duplicate key found, overwrite the value
            currentNode->entries[i]->value = entry->value;
            currentNode->entries[i]->length = length;
            free(entry); // Free the allocated memory for the new entry
            return;
        }
    }

    // If the key is not found, insert the new entry
    if(currentNode->nextIndex < MAX_TREE_ENTRIES) {
        BPT_InsertIntoLeaf(currentNode, entry);
    } else {
        bp_node_t *parent = tree->root;
        bp_node_t *current = tree->root;

        while(!current->isLeaf) {
            parent = current;
            current = current->nodes[0];
        }

        BPT_SplitLeafNode(parent, 0, current);
        BPT_Insert(tree, key, value, length);
    }
}


void BPT_DisplayTree(bp_tree_t *tree) {
    if(tree == NULL || tree->root == NULL) {
        printf("Tree is empty\n");
        return;
    }

    BPT_DisplayTreeLevel(tree->root, 0);
}
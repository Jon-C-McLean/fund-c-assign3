/**
 * @file huff.h
 * @author Jon McLean (13515869)
 */

#ifndef COMPR_H_
#define COMPR_H_

#include <stdlib.h>
#include <stdio.h>
#include "../ds/db.h"

typedef struct hc_node {
    int key;
    char* value;
    struct hc_node *next;
    struct hc_node *prev;
} hc_node_t;

/**
 * @brief Compresses a database into a buffer using the Huffman coding algorithm.
 * 
 * @param[in] db The database to compress
 * @param[out] buffer The resulting buffer
 * @param[out] length The length of the resulting buffer
 * @return status_t Status of the operation
 */
status_t HUFF_Compress(database_t *db, char **buffer, unsigned long long *length);

/**
 * @brief Decompresses a buffer into a database using the Huffman coding algorithm.
 * 
 * @param[out] db The resulting database
 * @param[in] buffer The buffer to decompress
 * @param[in] length The length of the buffer
 * @return status_t Status of the operation
 */
status_t HUFF_Decompress(database_t **db, char *buffer, unsigned long long length);

#endif
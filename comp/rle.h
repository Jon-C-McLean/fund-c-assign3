/**
 * @file rle.h
 * @author Jon McLean (13515869)
 */
#ifndef RLE_H_
#define RLE_H_

#include <stdlib.h>

/**
 * @brief Perform run length encoding on a given input buffer
 * @author Jon McLean (13515869)
 * 
 * @param[in] input The input buffer to compress
 * @param[in] dataSize The size of the data in the input buffer
 * @param[out] output The pointer to the output buffer (will be allocated by
 * this function)
 * @param[inout] length The length of the output buffer
 */
void RLE_Compress(char *input, int dataSize, char **output, int *length);

/**
 * @brief Perform run length decoding on a given input buffer
 * @author Jon McLean (13515869)
 * 
 * @param[in] input The input buffer to decompress
 * @param[in] dataSize The size of the data in the input buffer
 * @param[out] output The pointer to the output buffer (will be allocated by
 * this function)
 * @param[inout] length The length of the output buffer
 */
void RLE_Decompress(char *input, int dataSize, char **output, int *length);

#endif
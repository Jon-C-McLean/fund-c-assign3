/**
 * @file rle.h
 * @author Jon McLean (13515869)
 */
#ifndef RLE_H_
#define RLE_H_

#include <stdlib.h>

void RLE_Compress(char *input, int dataSize, char **output, int *length);

void RLE_Decompress(char *input, int dataSize, char **output, int *length);

#endif
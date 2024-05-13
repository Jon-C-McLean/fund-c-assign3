/**
 * @file rle.c
 * @author Jon McLean (13515869)
 */

#include "rle.h"
#include <stdio.h>

void RLE_Compress(char *input, int dataSize, char **output, int *length) {
    char *outputBuffer = (char *)malloc(dataSize * 2); /* Worst case sizing */
    int outputLength = 0;

    int count = 1;
    char last = input[0];
    int i = 0;

    for(i = 1; i < dataSize; i++) {
        if(input[i] == last) {
            count++;
        } else {
            outputBuffer[outputLength++] = count;
            outputBuffer[outputLength++] = last;

            count = 1;
            last = input[i];
        }
    }

    outputBuffer[outputLength++] = count;
    outputBuffer[outputLength++] = last;

    /* Realloc the buffer to the correct size */
    outputBuffer = (char *)realloc(outputBuffer, outputLength);

    *output = outputBuffer;
    *length = outputLength;

}

void RLE_Decompress(char *input, int dataSize, char **output, int *length) {
    char *outputBuffer = (char *)malloc(dataSize * 2); /* Worst case sizing */
    int outputLength = 0;

    int i = 0;
    for(i = 0; i < dataSize; i += 2) {
        int count = input[i];
        char value = input[i + 1];

        int j = 0;
        for(j = 0; j < count; j++) {
            outputBuffer[outputLength++] = value;

            /* Reallocate if at end of buffer */
            if(outputLength % dataSize == 0) {
                outputBuffer = (char *)realloc(outputBuffer, outputLength + dataSize);
            }
        }
    }

    /* Realloc the buffer to the correct size */
    outputBuffer = (char *)realloc(outputBuffer, outputLength);

    *output = outputBuffer;
    *length = outputLength;
}
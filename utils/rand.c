/**
 * @file rand.c
 * @author Jon McLean (13515869)
 */

#include "rand.h"

void RAND_GenerateInitVector(unsigned char *ivTable, int blockSize) {
    int i = 0;
    for(; i < blockSize; i++) {
        ivTable[i] = rand() % 256;
    }
}
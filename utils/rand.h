/**
 * @file rand.h
 * @author Jon McLean (13515869)
 */

#ifndef RAND_H_
#define RAND_H_

#include <stdlib.h>

/**
 * @brief Generates a random initialization vector for use in AES encryption.
 * 
 * @param[inout] ivTable The table to store the IV in (must be allocated by the caller) 
 * @param[in] blockSize The size of the IV table
 */
void RAND_GenerateInitVector(unsigned char *ivTable, int blockSize);

#endif
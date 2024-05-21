/**
 * @file misc.h
 * @author Jon McLean (13515869)
 */

#ifndef MISC_H
#define MISC_H

#define DEBUG 1
#if defined(DEBUG) && DEBUG == 1
    #include <stdio.h>
    #define DEBUG_PRINT(args) (printf("DEBUG: %s:%d ", __FILE__, __LINE__), printf args)
#else
    #define DEBUG_PRINT(args) __asm__("nop")
#endif

/**
 * @brief Get the maximum of two integers
 * @author Jon McLean (13515869)
 * 
 * @param[in] a
 * @param[in] b 
 * @return int Maximum value
 */
int max(int a, int b);

#endif
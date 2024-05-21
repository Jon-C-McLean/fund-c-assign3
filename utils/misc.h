/**
 * @file misc.h
 * @author Jon McLean (13515869)
 */

#ifndef MISC_H
#define MISC_H

#if defined(DEBUG) && DEBUG == 1
    #include <stdio.h>
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
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
/**
 * @file input.h
 * @author Jon McLean (13515869)
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <stdio.h>

/**
 * @brief Clears the input buffer of any hanging characters (up to and
 * including the newline character)
 */
void INPUT_ClearInputBuffer(void);

/**
 * @brief Get the user's input (as an integer)
 * 
 * @param[out] value The retrieved integer value
 */
void INPUT_GetInteger(int* value);

#endif

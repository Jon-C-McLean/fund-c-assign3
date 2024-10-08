/**
 * @file input.h
 * @author Jon McLean (13515869)
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <stdio.h>
#include <string.h>
#include "screen.h"

/**
 * @brief Clears the input buffer of any hanging characters (up to and
 * including the newline character)
 * 
 * @author Jon McLean (13515869)
 */
void INPUT_ClearInputBuffer(void);

/**
 * @brief Get the user's input (as an integer)
 * @author Jon McLean (13515869)
 * 
 * @param[out] value The retrieved integer value
 */
void INPUT_GetInteger(int* value);

/**
 * @brief Gets the user's input string and ensures that the string is not empty
 * 
 * @note This function will ensure the string is not empty and will remove the
 * newline character from the end of the string
 * 
 * @warning The function will not allocate space for the string, it is assumed
 * that the str param is already allocated
 * 
 * @author Jon McLean (13515869) (taken from A2 Submission)
 * 
 * @param[out] str The string to store the user's input in
 * @param[in] size The maximum size of the string
 * @return int The length of the string
 */
int INPUT_GetString(char* str, int size);

/**
 * @brief Waits for the user to press any key
 * 
 * @param[in] prompt The promt to display to the user before waiting
 */
void INPUT_WaitForAnyKey(char* prompt);

#endif

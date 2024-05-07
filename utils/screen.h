/**
 * @file screen.h
 * @author Jon McLean (13515869)
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdio.h>

typedef enum {
    kColor_Black,
    kColor_Red,
    kColor_Green,
    kColor_Yellow,
    kColor_Blue,
    kColor_Magenta,
    kColor_Cyan,
    kColor_White
} text_color_t;

/**
 * @brief Clears the terminal screen (using ANSI escape codes)
 * @author Jon McLean (13515869)
 */
void SCREEN_ClearScreen(void); /* printf("\033[2J\033[1;1H"); */

/**
 * @brief Prints text to the terminal with a specified color (using ANSI escape codes)
 * @author Jon McLean (13515869)
 * @note This does not support variadic arguments, so you must use sprintf to 
 * format the string before passing it to this function
 * 
 * @param[in] text Text to print
 * @param[in] color The desired text color
 */
void SCREEN_Print(char *text, text_color_t color);

/**
 * @brief Prints an error message to the terminal with a specified error
 * code
 * 
 * @param[in] text Error message
 */
void SCREEN_PrintError(char *text);

#endif
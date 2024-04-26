/**
 * @file screen.h
 * @author Jon McLean (13515869)
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdio.h>

/**
 * @brief Clears the terminal screen (using ANSI escape codes)
 */
void SCREEN_ClearScreen(void); /* printf("\033[2J\033[1;1H"); */

#endif
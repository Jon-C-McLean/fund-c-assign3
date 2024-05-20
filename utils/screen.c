/**
 * @file screen.c
 * @author Jon McLean (13515869)
 */
#include "screen.h"

/* Private Functions */
static char* SCREEN_GetColorCode(text_color_t color) {
    switch(color) {
        case kColor_Black:
            return "\033[0;30m";
        case kColor_Red:
            return "\033[0;31m";
        case kColor_Green:
            return "\033[0;32m";
        case kColor_Yellow:
            return "\033[0;33m";
        case kColor_Blue:
            return "\033[0;34m";
        case kColor_Magenta:
            return "\033[0;35m";
        case kColor_Cyan:
            return "\033[0;36m";
        case kColor_White:
            return "\033[0;37m";
        default:
            return "\033[0;37m";
    }
}

/* Public Functions*/

void SCREEN_ClearScreen(void) {
    printf("\033[2J\033[1;1H");
}

void SCREEN_Print(char *text, text_color_t color) {
    printf("%s%s\033[0m", SCREEN_GetColorCode(color), text);
}

void SCREEN_PrintError(char *text) {
    SCREEN_Print(text, kColor_Red);
}

void SCREEN_PrintInput(char *text) {
    SCREEN_Print(text, kColor_Blue);
}
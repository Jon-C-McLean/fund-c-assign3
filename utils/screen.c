#include "screen.h"

void SCREEN_ClearScreen(void) {
    printf("\033[2J\033[1;1H");
}
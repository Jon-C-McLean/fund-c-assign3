#include "input.h"

void INPUT_ClearInputBuffer(void) {
    char c;
    while((c = getchar()) != '\n' && c != EOF);
}

void INPUT_GetInteger(int* value) {
    scanf("%d", value);

    INPUT_ClearInputBuffer();
}
/**
 * @file input.c
 * @author Jon McLean (13515869)
 */

#include "input.h"

void INPUT_ClearInputBuffer(void) {
    char c;
    while((c = getchar()) != '\n' && c != EOF);
}

void INPUT_GetInteger(int* value) {
    scanf("%d", value);

    INPUT_ClearInputBuffer();
}

int INPUT_GetString(char* str, int size) { /* Taken from A2 (Jon McLean) */
    int length = 0;
    /* 
    If maximum size is less than two then immediately return 
    as we need at least 2 bytes to store the newline and null terminator 
    */
    if(size <= 2) {
        return 0;
    }

    /* 
    Loop through input until a non-empty string is passed by the user. A string
    is considered "not empty" when the length is greater than 2. The fgets
    function requires at least 2 bytes to store the newline and null terminator
    therefore if the length is <= 2 we can assume the string is empty
    */
    do {
        fgets(str, size, stdin);
    } while (strlen((const char*)str) < 2);

    /* Get length of non-empty string */
    length = strlen((const char*)str);
    
    /*
    If the last value in the array is not a new line character we need
    to clear the stdin buffer so any future fgets are not invalidated by a
    hanging newline character
    */
    if(str[length - 1] != '\n') {
        INPUT_ClearInputBuffer();
    }
    
    /* If the last character is a new line replace it with a null terminator */
    if(str[length - 1] == '\n') {
        str[length - 1] = '\0';
    }

    return length; /* Return user input length */
}

void INPUT_WaitForAnyKey(char* prompt) {
    SCREEN_PrintInput(prompt);
    getchar();
}
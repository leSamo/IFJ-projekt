/* This is just a mock parser file which just keep requesting tokens and printing them until EOF */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "leScanner.c"

int main(int argc, char* argv[]) {

    printf("\n----- TOKENS -----\n");

    Token *currentToken;
    while((currentToken = getToken()) != NULL) {
        printToken(currentToken);
    }

    printf("------------------\n\n");

    return 0;
}
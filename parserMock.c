/* ========= parserMock.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: This is just a mock parser file which just keeps requesting tokens and printing them until EOF 
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "leScanner.c"
#include "expressions.c"

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("\n----- TOKENS -----\n");

    Token *currentToken;
    Token tokensForExpression[50];
    int position = 0;

    while((currentToken = getToken()) != NULL) {
        if (currentToken->type == TOK_Error) {
            throwError("Lexical error\n");
            return LEXICAL_ERROR;
        }

        // mock for expression testing
        tokensForExpression[position++] = *currentToken;

        printToken(currentToken);
        free(currentToken);
    }

    printf("\n------------------\n");

    // if you want to handle input as an expression uncomment below line
    // handleExpression(tokensForExpression, position);

    printf("------------------\n\n");

    return 0;
}

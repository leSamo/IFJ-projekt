/* ========= parserMock.c ==========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00), Michal Findra (xfindr00)
 * Date: November 2020
 * Description: Syntactic analysis using recursive descent
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "leParser.h"
#include "leScanner.c"
#include "expressions.c"

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); // for debug, remove before submitting

    //printf("\n----- TOKENS -----\n");

    Token *currentToken;
    Token tokensForExpression[1000];
    int position = 0;

    /*
    while((currentToken = getToken()) != NULL) {
        // mock for expression testing
        tokensForExpression[position++] = *currentToken;

        printToken(currentToken);
        free(currentToken);
    }
    */

    if (NT_Prog()) {
        printf("alles klar\n");
    }
    else {
        printError("Syntactic error\n");
        return SYNTAX_ERROR;
    }

    //printf("\n------------------\n");

    // if you want to handle input as an expression uncomment below line
    // handleExpression(tokensForExpression, position);

    //printf("------------------\n\n");

    return 0;
}

// initial non-terminal
bool NT_Prog() {
    bool ret = false;

    ret = NT_Prolog() && NT_Main() && getToken()->type == TOK_EOF;

    return ret;
}

bool NT_Prolog() {
    bool ret = false;

    if (getToken()->type == TOK_Package_Keyword) {
        Token *nextToken = getToken();
        if (nextToken->type == TOK_Identifier && (strcmp(nextToken->str, "main") == 0)) {
            ret = true;
        }
    }

    return ret;
}

bool NT_Main() {
    bool ret = false;

    if (getToken()->type == TOK_Func_Keyword) {
        Token *nextToken = getToken();
        if (nextToken->type == TOK_Identifier && (strcmp(nextToken->str, "main") == 0)) {
            if (getToken()->type == TOK_L_Paren) {
                if (getToken()->type == TOK_R_Paren) {
                    if (getToken()->type == TOK_L_Brace) {
                        ret = NT_Stat();
                    }
                }
            }
        }
    }

    return ret;
}

bool NT_Stat() {
    bool ret = false;

    Token *nextToken = getToken();

    if (nextToken->type == TOK_Identifier) {
        ret = NT_Variable() && NT_Stat();
    }
    else if (nextToken->type == TOK_R_Brace) {
        ret = true;
    }
    else if (nextToken->type == TOK_If_Keyword) {
        ret = NT_If_Else() && NT_Stat();
    }
    else if (nextToken->type == TOK_For_Keyword) {
        ret = NT_For_Decl() && NT_For_Exp() && NT_For_Assign() && NT_Stat() && NT_Stat();
    }

    return ret;
}

bool NT_Variable() {
    bool ret = false;

    Token *nextToken = getToken();

    if (nextToken->type == TOK_Define) {
        ret = NT_Define(); // decl
    }
    else if (nextToken->type == TOK_Assign) {
        ret = NT_Exp(); // assign one
    }
    else if (nextToken->type == TOK_Comma) {
        if (getToken()->type == TOK_Identifier) {
            ret = NT_Assign_N() && NT_Exp() && getToken()->type == TOK_Comma && NT_Exp();
        }
    }

    return ret;
}

bool NT_Assign_N() {
    bool ret = false;

    Token *nextToken = getToken();

    if (nextToken->type == TOK_Assign) {
        ret = true;
    }
    else if (nextToken->type == TOK_Comma) {
        if (getToken()->type == TOK_Identifier) {
            ret = NT_Assign_N() && NT_Exp() && getToken()->type == TOK_Comma;
        }
    }

    return ret;
}

bool NT_Define() {
    return NT_Exp();
}

bool NT_Exp() {
    // pass it to expression parser
    // ret = handleExpression();

    // for now just get one token
    getToken();

    return true;
}

bool NT_If_Else() {
    bool ret = false;

    if (NT_Exp()) {
        if (getToken()->type == TOK_L_Brace) {
            if (NT_Stat()) {
                if (getToken()->type == TOK_Else_Keyword) {
                    if (getToken()->type == TOK_L_Brace) {
                        if (NT_Stat()) {
                            ret = true;
                        }
                    }
                }
            }
        }
    }

    return ret;
}

bool NT_For_Decl() {
    bool ret = false;

    Token *nextToken = getToken();

    if (nextToken->type == TOK_Semicolon) {
        ret = true;
    }
    else if (nextToken->type == TOK_Identifier) {
        ret = getToken()->type == TOK_Define && NT_Exp() && getToken()->type == TOK_Semicolon;
    }

    return ret;
}

bool NT_For_Exp() {
    bool ret = false;

    Token *nextToken = getToken();

    if (nextToken->type == TOK_Semicolon) {
        ret = true;
    }
    // expression

    return ret;
}

bool NT_For_Assign() {
    bool ret = false;

    Token *nextToken = getToken();

    if (nextToken->type == TOK_L_Brace) {
        ret = true;
    }
    else if (nextToken->type == TOK_Identifier) {
        ret = NT_Assign_N() && NT_Exp() && getToken()->type == TOK_L_Brace;
    }

    return ret;
}

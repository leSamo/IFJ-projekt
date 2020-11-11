/* ========= expressions.c =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Precedence syntactic analysis for expressions using shunting-yard algorithm
 * ================================= */

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "expressions.h"

void handleExpression(Token tokens[], int length) {
    TokenStack *operatorStack = malloc(sizeof(TokenStack));
    TokenStack *outputQueue = malloc(sizeof(TokenStack));

    for (int i = 0; i < length; i++) {
        Token currentToken = tokens[i];

        if (currentToken.type == TOK_Int_Literal) {
            TokenStackPush(outputQueue, currentToken);
        }
        else if (currentToken.type == TOK_L_Paren) {
            TokenStackPush(operatorStack, currentToken);
        }
        else if (currentToken.type == TOK_Mul || currentToken.type == TOK_Div ||
                 currentToken.type == TOK_Add || currentToken.type == TOK_Sub) {
            if (operatorStack->count > 0) {
                Token previous = TokenStackPop(operatorStack);

                if (previous.type != TOK_L_Paren && getPriority(currentToken.type) <= getPriority(previous.type)) {
                    TokenStackPush(outputQueue, previous);
                }
                else {
                    TokenStackPush(operatorStack, previous);
                }
            }

            TokenStackPush(operatorStack, currentToken);
        }
        else if (currentToken.type == TOK_R_Paren) {
            if (operatorStack->count == 0) {
                printError("Unpaired parentheses error.\n");
                exit(SYNTAX_ERROR);
            }

            while (TokenStackTop(operatorStack).type != TOK_L_Paren) {
                Token token = TokenStackPop(operatorStack);
                TokenStackPush(outputQueue, token);

                if (operatorStack->count == 0) {
                    printError("Unpaired parentheses error.\n");
                    exit(SYNTAX_ERROR);
                }
            }

            // pop left paren
            TokenStackPop(operatorStack);
        }        
    }

    while (operatorStack->count > 0) {
        Token token = TokenStackPop(operatorStack);

        if (token.type == TOK_L_Paren) {
            printError("Unpaired parentheses error.\n");
            exit(SYNTAX_ERROR);
        }

        TokenStackPush(outputQueue, token);
    }

    TokenStackPrint(outputQueue);

    free(operatorStack);
    free(outputQueue);
}

void TokenStackPush(TokenStack *stack, Token token) {
    stack->tokens[stack->count++] = token;
}

Token TokenStackTop(TokenStack *stack) {
    if (stack->count > 0) {
        return stack->tokens[stack->count - 1];
    }
    else {
        // throw error
    }
}

Token TokenStackPop(TokenStack *stack) {
    if (stack->count > 0) {
        return stack->tokens[--stack->count];
    }
    else {
        // throw error
    }
}

void TokenStackPrint(TokenStack *stack) {
    for (int i = 0; i < stack->count; i++) {
        Token currentToken = stack->tokens[i];
        if (currentToken.type == TOK_Int_Literal) {
            printf("%s: %d\n", getTokenName(currentToken.type), currentToken.i);
        }
        else {
            printf("%s\n", getTokenName(currentToken.type));
        }
    }
}

// priorities are reversed to assignment
int getPriority(tokenType type) {
    switch (type) {
        case TOK_Mul:
        case TOK_Div:
            return 3;
        case TOK_Add:
        case TOK_Sub:
            return 2;
        case TOK_Less_Then:
        case TOK_Less_Equal_Then:
        case TOK_More_Then:
        case TOK_More_Equal_Then:
        case TOK_Equal:
        case TOK_Not_Equal:
            return 1;
    }
}

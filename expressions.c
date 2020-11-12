/* ========= expressions.c =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Emma Krompaščíková (xkromp00), Samuel Olekšák (xoleks00)
 * Date: November 2020
 * Description: Precedence syntactic analysis for expressions using shunting-yard algorithm
 * ================================= */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"
#include "expressions.h"
#include "leScanner.h"

bool handleExpression(Token *overlapTokenIn, Token *overlapTokenOut) {
    TokenStack *operatorStack = malloc(sizeof(TokenStack));
    operatorStack->count = 0;
    TokenStack *outputQueue = malloc(sizeof(TokenStack));
    outputQueue->count = 0;

    for (int i = 0; ; i++) {
        Token currentToken;

        if (i == 0 && overlapTokenIn != NULL){
            currentToken = *overlapTokenIn;
        }
        else {
            currentToken = *getToken();
        }

        if (!isValidExpToken(currentToken.type)) {
            // I got one more token then I should have, give it back
            printf("encountered %s\n", getTokenName(currentToken.type));
            *overlapTokenOut = currentToken;
            break;
        }

        if (currentToken.type == TOK_Int_Literal || currentToken.type == TOK_Float_Literal) {
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

                // TODO: memory cleanup
                return false;
            }

            while (TokenStackTop(operatorStack).type != TOK_L_Paren) {
                Token token = TokenStackPop(operatorStack);
                TokenStackPush(outputQueue, token);

                if (operatorStack->count == 0) {
                    printError("Unpaired parentheses error.\n");

                    // TODO: memory cleanup
                    return false;
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

            // TODO: memory cleanup
            return false;
        }

        TokenStackPush(outputQueue, token);
    }

    // TokenStackPrint(outputQueue);

    bool ret = verifyOutput(outputQueue);

    free(operatorStack);
    free(outputQueue);

    return ret;
}

bool verifyOutput(TokenStack *outputQueue) {
    printf("s1:\n");
    TokenStackPrint(outputQueue);
    printf("---\n");

    for (int i = 0; outputQueue->count > 1; i++) {
        Token currentToken = outputQueue->tokens[i];

        if (isOperator(currentToken.type)) {
            if (!TokenStackCollapse(outputQueue, i)) {
                return false;
            }
            i = 0;
        }
    }    

    printf("s2:\n");
    TokenStackPrint(outputQueue);
    printf("---\n");

    if (outputQueue->count == 1) {
        return true;
    }
    else {
        return false;
    }
}

bool TokenStackCollapse(TokenStack *stack, int pos) {
    if (pos < 2 || stack->count < 2) {
        return false;
    }
    else {
        // both should be numbers
        if (isOperator(stack->tokens[pos - 2].type) || isOperator(stack->tokens[pos - 1].type)) {
            printf("here");
            return false;
        }
    
        for (int i = pos + 1; i < stack->count; i++) {
            stack->tokens[i - 2] = stack->tokens[i]; 
        }
        /*
        stack->tokens[pos - 1].type = TOK_Int_Literal;
        stack->tokens[pos - 1].i = 0;
        */

        stack->count -= 2;

        printf("s cont:\n");
        TokenStackPrint(stack);
        printf("---\n");

        return true;
    }
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

// priorities are reversed to project assignment
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

bool isValidExpToken(tokenType type) {
    switch (type) {
        case TOK_Identifier:
        case TOK_Int_Literal:
        case TOK_Float_Literal:
        case TOK_L_Paren:
        case TOK_R_Paren:
        case TOK_Mul:
        case TOK_Div:
        case TOK_Add:
        case TOK_Sub:
        case TOK_Less_Then:
        case TOK_Less_Equal_Then:
        case TOK_More_Then:
        case TOK_More_Equal_Then:
        case TOK_Equal:
        case TOK_Not_Equal:
            return true;
        default:
            return false;
    }
}

bool isOperator(tokenType type) {
    switch (type) {
        case TOK_Add:
        case TOK_Sub:
        case TOK_Mul:
        case TOK_Div:
            return true;
        
        default:
            return false;
    }
}
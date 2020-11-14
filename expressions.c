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

bool handleExpression(Token overlapTokenIn, Token *overlapTokenOut) {
    TokenBuffer *operatorStack = TokenBufferCreate();
    TokenBuffer *outputQueue = TokenBufferCreate();

    for (int i = 0; ; i++) {
        Token currentToken;

        if (i == 0 && overlapTokenIn.type != TOK_Empty){
            currentToken = overlapTokenIn;
        }
        else {
            currentToken = getToken();
        }

        if (!isValidExpToken(currentToken.type)) {
            // I got one more token then I should have, give it back
            *overlapTokenOut = currentToken;
            break;
        }

        if (currentToken.type == TOK_Int_Literal || currentToken.type == TOK_Float_Literal || currentToken.type == TOK_Identifier) {
            TokenBufferPush(outputQueue, currentToken);
        }
        else if (currentToken.type == TOK_L_Paren) {
            TokenBufferPush(operatorStack, currentToken);
        }
        else if (isOperator(currentToken.type)) {
            if (operatorStack->count > 0) {
                Token previous = TokenBufferPop(operatorStack);

                if (previous.type != TOK_L_Paren && getPriority(currentToken.type) <= getPriority(previous.type)) {
                    TokenBufferPush(outputQueue, previous);
                }
                else {
                    TokenBufferPush(operatorStack, previous);
                }
            }

            TokenBufferPush(operatorStack, currentToken);
        }
        else if (currentToken.type == TOK_R_Paren) {
            if (operatorStack->count == 0) {
                printError("Unpaired parentheses error.\n");

                TokenBufferDispose(operatorStack);
                TokenBufferDispose(outputQueue);

                return false;
            }

            while (TokenBufferTop(operatorStack).type != TOK_L_Paren) {
                Token token = TokenBufferPop(operatorStack);
                TokenBufferPush(outputQueue, token);

                if (operatorStack->count == 0) {
                    printError("Unpaired parentheses error.\n");

                    TokenBufferDispose(operatorStack);
                    TokenBufferDispose(outputQueue);

                    return false;
                }
            }

            // pop left paren
            TokenBufferPop(operatorStack);
        }        
    }

    while (operatorStack->count > 0) {
        Token token = TokenBufferPop(operatorStack);

        if (token.type == TOK_L_Paren) {
            printError("Unpaired parentheses error.\n");

            TokenBufferDispose(operatorStack);
            TokenBufferDispose(outputQueue);

            return false;
        }

        TokenBufferPush(outputQueue, token);
    }

    bool ret = verifyOutput(outputQueue);

    TokenBufferDispose(operatorStack);
    TokenBufferDispose(outputQueue);

    return ret;
}

bool verifyOutput(TokenBuffer *outputQueue) {
    for (int i = 0; outputQueue->count > 1; i++) {
        Token currentToken = outputQueue->tokens[i];

        if (isOperator(currentToken.type)) {
            if (!TokenBufferCollapse(outputQueue, i)) {
                return false;
            }
            i = 0;
        }
    }    

    if (outputQueue->count == 1) {
        return true;
    }
    else {
        return false;
    }
}

TokenBuffer* TokenBufferCreate() {
    TokenBuffer *buffer = malloc(sizeof(TokenBuffer));
    buffer->tokens = malloc(sizeof(Token) * INITIAL_BUFFER_LENGTH);

    if (buffer == NULL) {
        printError("Memory allocation error\n");
        exit(INTERNAL_ERROR);
    }

    buffer->count = 0;
    buffer->capacity = INITIAL_BUFFER_LENGTH;

    return buffer;
}

bool TokenBufferCollapse(TokenBuffer *buffer, int pos) {
    if (pos < 2 || buffer->count < 2) {
        return false;
    }
    else {
        // both should be numbers
        if (isOperator(buffer->tokens[pos - 2].type) || isOperator(buffer->tokens[pos - 1].type)) {
            return false;
        }
    
        for (int i = pos + 1; i < buffer->count; i++) {
            buffer->tokens[i - 2] = buffer->tokens[i]; 
        }

        buffer->count -= 2;

        return true;
    }
}

void TokenBufferPush(TokenBuffer *buffer, Token token) {
    if (buffer->count + 1 == buffer->capacity) {
        buffer->capacity *= 2;
        Token* newArray = realloc(buffer->tokens, sizeof(Token) * buffer->capacity);

        if (newArray == NULL) {
            TokenBufferDispose(buffer);
            printError("Memory allocation error\n");
            exit(INTERNAL_ERROR);
        }
        else {
            buffer->tokens = newArray;
        }

        buffer->tokens[buffer->count++] = token;
    }
    else {
        buffer->tokens[buffer->count++] = token;
    }
}

Token TokenBufferTop(TokenBuffer *buffer) {
    if (buffer->count > 0) {
        return buffer->tokens[buffer->count - 1];
    }
    else {
        // throw error
    }
}

Token TokenBufferPop(TokenBuffer *buffer) {
    if (buffer->count > 0) {
        return buffer->tokens[--buffer->count];
    }
    else {
        // throw error
    }
}

void TokenBufferPrint(TokenBuffer *buffer) {
    for (int i = 0; i < buffer->count; i++) {
        Token currentToken = buffer->tokens[i];
        if (currentToken.type == TOK_Int_Literal) {
            printf("%s: %d\n", getTokenName(currentToken.type), currentToken.i);
        }
        else {
            printf("%s\n", getTokenName(currentToken.type));
        }
    }
}

void TokenBufferDispose(TokenBuffer *buffer) {
    free(buffer->tokens);
    free(buffer);
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
        case TOK_Equal:
        case TOK_Not_Equal:
        case TOK_More_Equal_Then:
        case TOK_More_Then:
        case TOK_Less_Equal_Then:
        case TOK_Less_Then:
            return true;
        
        default:
            return false;
    }
}
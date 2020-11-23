/* ========= tokenBuffer.c =========
 * Project: IFJ 2020/21 project
 * Team: 067, variant I
 * Author: Samuel Olekšák (xoleks00)
 * Date: November 2020
 * ================================= */

#include <stdlib.h>
#include <stdbool.h>

#include "tokenBuffer.h"
#include "expressions.h"

TokenBuffer* TokenBufferCreate() {
    TokenBuffer *buffer = malloc(sizeof(TokenBuffer));
    buffer->tokens = malloc(sizeof(Token) * INITIAL_TOKEN_BUFFER_SIZE);

    if (buffer == NULL) {
        printError(INTERNAL_ERROR, "Memory allocation error\n");
        deallocateAll();
        exit(INTERNAL_ERROR);
    }

    buffer->count = 0;
    buffer->capacity = INITIAL_TOKEN_BUFFER_SIZE;

    return buffer;
}

bool TokenBufferCollapse(TokenBuffer *buffer, int pos) {
    if (pos < 2 || buffer->count < 3) {
        return false;
    }
    else {
        // both should be numbers
        if (isOperator(buffer->tokens[pos - 2].type) || isOperator(buffer->tokens[pos - 1].type)) {
            return false;
        }

        Token leftOperand  = buffer->tokens[pos - 2];
        Token rightOperand = buffer->tokens[pos - 1];
        Token operator     = buffer->tokens[  pos  ];
    
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
            printError(INTERNAL_ERROR, "Memory allocation error\n");
            deallocateAll();
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
